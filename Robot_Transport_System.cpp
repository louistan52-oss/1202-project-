#include "Robot_Transport_System.h"
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <conio.h>
#include <map>
#include <vector>
#include "BMS.h"

// Global flag to control background threads
std::atomic<bool> systemRunning(true);
using namespace std::chrono_literals;

Books b;

// --- SHELVES & LOCATION ---
Shelves::Shelves(int shelfId, int initialBooks, int cap) : id(shelfId), currentBooks(initialBooks), maxCapacity(cap) {}
bool Shelves::isFull() const { return (currentBooks >= maxCapacity); }

Location::Location(char vid, string name, string sPath) : idChar(vid), locationName(name), stockFilePath(sPath) {
    ifstream inFile(stockFilePath);
    if (inFile.is_open()) { inFile >> stockAmount; inFile.close(); }
    else { stockAmount = 100; } // Default fallback stock
}
Location::~Location() {}

bool Location::identifyAndExecute(char input) {
    if (toupper(input) == idChar) { this->processRequest(); return true; }
    return false;
}

void Location::saveStockToFile() { // Thread-safe save to persist current warehouse stock levels.
    lock_guard<mutex> lock(stockMtx);
    ofstream outFile(stockFilePath);
    if (outFile.is_open()) { outFile << stockAmount; outFile.close(); }
}

// --- ROBOT LOGIC ---
Robot::Robot(string n) : name(n), battery(rand() % 20 + 75) {} // Handles the logic of delivering books from inventory to shelves.
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
            unique_lock<mutex> lock(rMtx);

            // 1. BATTERY HANDOVER: If < 40%, offload and go charge.
            if (battery <= 40) { // If low, return stock to warehouse and switch to charging thread
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
                    this_thread::sleep_for(600ms); // Simulated travel time
                    lock.unlock();                  // release rMtx first
                    {
                        lock_guard<mutex> sLock(sMtx); // Lock shared Shelf resource
                        if (!s.isFull()) {          // re-check under lock
                            s.currentBooks = min(s.currentBooks + 1, s.maxCapacity); // safe increment
                            lock.lock(); // Re-lock Robot to update its own state
                            inventory--; battery -= 2;
                        } else {
                            lock.lock();
                        }
                    }
                    break;
                }
            }
        }
    });
    t.detach();
}

// Refilling Logic
void Robot::startRefilling(int& vStock, mutex& sMtx, Location& loc, int needed) { // Picks up books from the warehouse stock based on shelf requirements
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

void Robot::startCharging() { // Charing Logic. Incremental battery recovery until 100%
    while (systemRunning) {
        this_thread::sleep_for(1s);
        lock_guard<mutex> lock(rMtx);
        battery += 2;
        if (battery >= 100) { battery = 100; status = State::IDLE; return; }
    }
}

// --- Book Data ---
vector<int> venue_data(){
    string genre[5];
    Books::BookData data = b.loadBooks();
    vector<int> venue_books(3,0); //grabs no. of books per venue
            for (int i=0; i<data.book.size(); i++){
                if (data.book[i].getVenue() == "A") venue_books[0]++;
                else if (data.book[i].getVenue() == "B") venue_books[1]++;
                else if (data.book[i].getVenue() == "C") venue_books[2]++;
            }
    return venue_books;
}

// --- VENUE & MONITOR ---
Venue::Venue(char vid, string name, string stockFile, string shelfFile) : Location(vid, name, stockFile), shelfDataPath(shelfFile) {
    loadShelves(); 
    vector<int> count=venue_data();
    int index = vid - 'A';
    totalVenueBooks = (index >= 0 && index < count.size()) ? count[index] : 0;
    robots.emplace_back("Alpha"); robots.emplace_back("Bravo");
}

void Venue::loadShelves() {
    ifstream inFile(shelfDataPath); allShelves.clear(); int val;
    for(int i = 0; i < 5; i++) {
        if (inFile >> val) allShelves.emplace_back(i+1, min(val,4));
        else allShelves.emplace_back(i+1, 0);
    }
}

void Venue::saveShelves() {
    ofstream outFile(shelfDataPath);
    if (outFile.is_open()) for (auto& s : allShelves) outFile << s.currentBooks << " ";
}

string Venue::getQuickStatus() {
    int totalShelves = 0; for (auto& s : allShelves) totalShelves += s.currentBooks;
    string stockCol = (totalVenueBooks < 10) ? "\033[1;31m" : "\033[1;32m"; //if less than 10 in storage, show warning
    return "Stock: [" + stockCol + to_string(totalVenueBooks) + "\033[0m] | Progress: " + to_string(min(totalShelves,stockAmount)) + "/" + to_string(stockAmount);
    //color code, total books at venue, progress: min of how many at shelves vs how many in storage / stock remain in storage
}

void Venue::processRequest() { // MONITORING & CONTROL
    while (systemRunning) {
        cout << "\033[2J\033[3J\033[H" << flush; // Clear screen

        saveShelves(); loadShelves();
        int total = 0; for (auto& s : allShelves) total += s.currentBooks;
        int inventory; bool hasInventory=false, anyActive=false;

        for (const auto& r : robots) {
            if (r.getInventory()>0) hasInventory = true;
            if (r.getStatus()==State::REFILLING || r.getStatus()==State::WORKING) anyActive=true;
        };

        cout << "LIVE MONITOR: " << locationName << " | CENTRAL STOCK: " << stockAmount << "\n" << endl;
        for (auto& s : allShelves) {
            cout << " Shelf " << s.id << " [";
            for(int i=0; i<s.maxCapacity; i++) cout << (i < s.currentBooks ? "=" : ".");
            cout << "] (" << s.currentBooks << "/4)" << endl;
        }

        // DISPATCHER: Assigns IDLE robots to work if shelves aren't full
        bool moving = false;
        for (const auto& r : robots) if (r.getStatus() == State::WORKING || r.getStatus() == State::REFILLING) moving = true;
        
        if (!moving && (total < 20 && stockAmount > 0) || hasInventory && !anyActive) {
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
    systemRunning = true;
    while (true) {
        for (int i = 0; i<48; i++) cout << "=";
        cout << endl;
        for (int i = 0; i <11; i++) cout <<"=";
        cout << "ROBOT TRANSPORT DASHBOARD";
        for (int i = 0; i < 12; i++) cout << "=";
        cout << endl;
        for (int i = 0; i<48; i++) cout << "=";
        cout << endl;
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
    vector<int> count = venue_data();
    init("vA_stock.txt", to_string(count[0]));
    init("vA_shelves.txt", "0 0 0 0 0");  // ← was spreading books across shelves

    init("vB_stock.txt", to_string(count[1]));
    init("vB_shelves.txt", "0 0 0 0 0");

    init("vC_stock.txt", to_string(count[2]));
    init("vC_shelves.txt", "0 0 0 0 0");
}

bool keyPressed(char target) {
    if (_kbhit()) {
        char ch = (char)_getch();
        return (tolower(ch) == tolower(target));
    }
    return false;
}