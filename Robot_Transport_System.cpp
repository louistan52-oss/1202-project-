#include "Robot_Transport_System.h"
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

std::atomic<bool> systemRunning(true);
using namespace std::chrono_literals;

// --- SHELVES & LOCATION ---
Shelves::Shelves(int shelfId, int initialBooks, int cap) : id(shelfId), currentBooks(initialBooks), maxCapacity(cap) {}
bool Shelves::isFull() const { return currentBooks >= maxCapacity; }

Location::Location(char vid, string name, string sPath) : idChar(vid), locationName(name), stockFilePath(sPath) {
    ifstream inFile(stockFilePath);
    if (inFile.is_open()) { inFile >> stockAmount; inFile.close(); }
    else { stockAmount = 100; }
}
Location::~Location() {}

bool Location::identifyAndExecute(char input) {
    if (toupper(input) == idChar) { this->processRequest(); return true; }
    return false;
}

void Location::saveStockToFile() {
    lock_guard<mutex> lock(stockMtx);
    ofstream outFile(stockFilePath);
    if (outFile.is_open()) { outFile << stockAmount; outFile.close(); }
}

// --- ROBOT LOGIC ---
Robot::Robot(string n) : name(n), battery(rand() % 20 + 75) {}
Robot::Robot(Robot&& other) noexcept {
    lock_guard<mutex> lock(other.rMtx);
    name = move(other.name); battery = other.battery; inventory = other.inventory; status = other.status;
}
State Robot::getStatus() const { lock_guard<mutex> lock(rMtx); return status; }
int Robot::getBattery() const { lock_guard<mutex> lock(rMtx); return battery; }

void Robot::display() const {
    lock_guard<mutex> lock(rMtx);
    string s = (status == State::WORKING) ? "\033[1;32mWORKING\033[0m" : 
               (status == State::CHARGING) ? "\033[1;33mCHARGING\033[0m" :
               (status == State::REFILLING) ? "\033[1;34mREFILLING\033[0m" : "IDLE";
    cout << left << setw(10) << name << " | Bat: " << setw(3) << battery << "% | Inv: " << inventory << "/5 | " << s << endl;
}

void Robot::startWorking(vector<Shelves>& venueShelves, int& vStock, mutex& sMtx, Location& loc) {
    { lock_guard<mutex> lock(rMtx); status = State::WORKING; }
    
    thread t([this, &venueShelves, &vStock, &sMtx, &loc]() {
        while (systemRunning) {
            this_thread::sleep_for(1s); 
            lock_guard<mutex> lock(rMtx);

            // 1. BATTERY HANDOVER: If < 20%, offload and go charge.
            if (battery <= 20) {
                if (inventory > 0) { lock_guard<mutex> sLock(sMtx); vStock += inventory; inventory = 0; }
                status = State::CHARGING;
                thread(&Robot::startCharging, this).detach();
                return; // End thread to let the next robot take the "token".
            }

            // 2. SMART CROSS-REFERENCE: How many books are missing total?
            int current = 0, maxTotal = 0;
            for (auto& s : venueShelves) { current += s.currentBooks; maxTotal += s.maxCapacity; }
            int deficit = maxTotal - current;

            // 3. REFILL CHECK: Only take what the shelves actually need.
            if (inventory <= 0) {
                if (deficit > 0) {
                    status = State::REFILLING;
                    thread(&Robot::startRefilling, this, ref(vStock), ref(sMtx), ref(loc), deficit).detach();
                    return;
                } else { status = State::IDLE; return; }
            }

            // 4. DELIVERY ACTION
            for (auto& s : venueShelves) {
                if (!s.isFull()) {
                    this_thread::sleep_for(600ms); 
                    s.currentBooks++; inventory--; battery -= 2;
                    break;
                }
            }
        }
    });
    t.detach();
}

void Robot::startRefilling(int& vStock, mutex& sMtx, Location& loc, int needed) {
    this_thread::sleep_for(1s); 
    int toTake = 0;
    {
        lock_guard<mutex> sLock(sMtx);
        // Take the SMALLEST of: Robot Max (5), Warehouse Stock, or What Shelves need.
        toTake = min({5 - inventory, vStock, needed});
        vStock -= toTake;
    }
    for (int i = 0; i < toTake; ++i) {
        if (!systemRunning) break;
        this_thread::sleep_for(800ms);
        { lock_guard<mutex> rLock(rMtx); inventory++; battery -= 1; }
    }
    { lock_guard<mutex> rLock(rMtx); status = State::IDLE; }
    loc.saveStockToFile();
}

void Robot::startCharging() {
    while (systemRunning) {
        this_thread::sleep_for(1s);
        lock_guard<mutex> lock(rMtx);
        battery += 15;
        if (battery >= 100) { battery = 100; status = State::IDLE; return; }
    }
}

// --- VENUE & MONITOR ---
Venue::Venue(char vid, string name, string stockFile, string shelfFile) : Location(vid, name, stockFile), shelfDataPath(shelfFile) {
    loadShelves(); robots.emplace_back("Alpha"); robots.emplace_back("Bravo");
}

void Venue::loadShelves() {
    ifstream inFile(shelfDataPath); allShelves.clear(); int val;
    for(int i = 0; i < 5; i++) {
        if (inFile >> val) allShelves.emplace_back(i+1, val);
        else allShelves.emplace_back(i+1, 0);
    }
}

void Venue::saveShelves() {
    ofstream outFile(shelfDataPath);
    if (outFile.is_open()) for (auto& s : allShelves) outFile << s.currentBooks << " ";
}

string Venue::getQuickStatus() {
    int total = 0; for (auto& s : allShelves) total += s.currentBooks;
    string stockCol = (stockAmount < 10) ? "\033[1;31m" : "\033[1;32m";
    return "Stock: [" + stockCol + to_string(stockAmount) + "\033[0m] | Progress: " + to_string(total) + "/20";
}

void Venue::processRequest() {
    while (systemRunning) {
        cout << "\033[2J\033[3J\033[H" << flush;
        saveShelves(); loadShelves();
        int total = 0; for (auto& s : allShelves) total += s.currentBooks;

        cout << "LIVE MONITOR: " << locationName << " | CENTRAL STOCK: " << stockAmount << "\n" << endl;
        for (auto& s : allShelves) {
            cout << " Shelf " << s.id << " [";
            for(int i=0; i<s.maxCapacity; i++) cout << (i < s.currentBooks ? "■" : ".");
            cout << "] (" << s.currentBooks << "/4)" << endl;
        }

        // --- SINGLE DISPATCHER TOKEN ---
        bool moving = false;
        for (const auto& r : robots) if (r.getStatus() == State::WORKING || r.getStatus() == State::REFILLING) moving = true;
        
        if (!moving && total < 20 && stockAmount > 0) {
            for (auto& r : robots) if (r.getStatus() == State::IDLE) { r.startWorking(allShelves, stockAmount, stockMtx, *this); break; }
        }

        cout << "\nROBOTS (Q to Exit):\n"; for (auto& r : robots) r.display();
        if (keyPressed('q')) break;
        this_thread::sleep_for(500ms);
    }
}

// --- SYSTEM CONTROLLER ---
SystemController::SystemController() {
    createDummyEnvironment();
    fleet.push_back(new Venue('A', "Venue_A", "vA_stock.txt", "vA_shelves.txt"));
    fleet.push_back(new Venue('B', "Venue_B", "vB_stock.txt", "vB_shelves.txt"));
    fleet.push_back(new Venue('C', "Venue_C", "vC_stock.txt", "vC_shelves.txt"));
}

SystemController::~SystemController() { for (auto loc : fleet) delete loc; }

void SystemController::run() {
    string in;
    while (true) {
        cout << "\033[2J\033[3J\033[H\033[1;36m╔══════════════════════════════════════════════════╗\n║           ROBOT TRANSPORT DASHBOARD              ║\n╚══════════════════════════════════════════════════╝\033[0m\n\n";
        char label = 'A';
        for (auto loc : fleet) { cout << " [" << label << "] Venue " << label << " -> " << loc->getQuickStatus() << endl; label++; }
        cout << "\n [Q] SHUTDOWN\n\n Selection > ";
        if (!(cin >> in)) break; char c = toupper(in[0]); if (c == 'Q') break;
        bool found = false; for (auto loc : fleet) if (loc->identifyAndExecute(c)) found = true;
        if (!found) { cout << "Invalid Selection."; this_thread::sleep_for(1s); }
    }
    systemRunning = false;
}

void SystemController::createDummyEnvironment() {
    auto init = [](string p, string v) { ifstream c(p); if(!c) { ofstream f(p); f << v; } };
    init("vA_stock.txt", "100"); init("vA_shelves.txt", "3 4 0 2 0");
    init("vB_stock.txt", "50");  init("vB_shelves.txt", "3 0 1 1 2");
    init("vC_stock.txt", "60");  init("vC_shelves.txt", "0 1 3 4 2");
}

bool keyPressed(char target) {
    struct termios oldt, newt; int ch, oldf; tcgetattr(STDIN_FILENO, &oldt); newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0); fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar(); tcsetattr(STDIN_FILENO, TCSANOW, &oldt); fcntl(STDIN_FILENO, F_SETFL, oldf);
    return (ch != EOF && (tolower(ch) == tolower(target)));
}