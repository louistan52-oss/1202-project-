#include "Robot_Transport_System.h"
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <conio.h>
#include <map>
#include <vector>
#include "BMS.h"

std::atomic<bool> systemRunning(true);
using namespace std::chrono_literals;

Books b;

// --- SHELVES & LOCATION ---
Shelves::Shelves(int shelfId, int initialBooks, int cap) : id(shelfId), currentBooks(initialBooks), maxCapacity(cap) {}
bool Shelves::isFull() const { return (currentBooks >= maxCapacity); }

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
            unique_lock<mutex> lock(rMtx);

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
                    lock.unlock();                  // release rMtx first
                    {
                        lock_guard<mutex> sLock(sMtx);
                        if (!s.isFull()) {          // re-check under lock
                            s.currentBooks = min(s.currentBooks + 1, s.maxCapacity); // safe increment
                            lock.lock();
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

// --- Book Data ---
map<int,vector<int>> venue_data(){
    string genre[5];
    Books::BookData data = b.loadBooks();
    map<int,vector<int>> venue_books; //1st value is cat, value2 is book count per genre type
    venue_books[0] = vector<int>(6,0);
    venue_books[1] = vector<int>(6,0);
    venue_books[2] = vector<int>(6,0);
            for (int i=0; i<data.book.size(); i++){
                if (data.book[i].getVenue() == "A"){
                    venue_books[0][5]++;
                    for (int f=0; f<5;f++){
                        if (genre[f].empty()) {
                            genre[f] = data.book[i].getGenre();
                            venue_books[0][f]++;
                            break;
                        }
                        else if (genre[f]==data.book[i].getGenre()) {
                            venue_books[0][f]++;
                            break;
                        }
                    }
                }
                else if (data.book[i].getVenue() == "B"){
                    venue_books[1][5]++;
                    for (int f=0; f<5;f++){
                        if (genre[f].empty()) {
                            genre[f] = data.book[i].getGenre();
                            venue_books[1][f]++;
                            break;
                        }
                        else if (genre[f]==data.book[i].getGenre()) {
                            venue_books[1][f]++;
                            break;
                        }
                    }
                }
                else if (data.book[i].getVenue() == "C"){
                    venue_books[2][5]++;
                    for (int f=0; f<5;f++){
                        if (genre[f].empty()) {
                            genre[f] = data.book[i].getGenre();
                            venue_books[2][f]++;
                            break;
                        }
                        else if (genre[f]==data.book[i].getGenre()){
                            venue_books[2][f]++;
                            break;
                        }
                    }
                }
            }
    return venue_books;
}

// --- VENUE & MONITOR ---
Venue::Venue(char vid, string name, string stockFile, string shelfFile) : Location(vid, name, stockFile), shelfDataPath(shelfFile) {
    loadShelves(); robots.emplace_back("Alpha"); robots.emplace_back("Bravo");
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
    int total = 0; for (auto& s : allShelves) total += s.currentBooks;
    string stockCol = (stockAmount < 10) ? "\033[1;31m" : "\033[1;32m";
    return "Stock: [" + stockCol + to_string(stockAmount) + "\033[0m] | Progress: " + to_string(total) + "/" + to_string(total);
}

void Venue::syncShelvesFromFile() {
    map<int,vector<int>> live = venue_data();
    int venueIndex = (locationName == "Venue_A") ? 0 
                   : (locationName == "Venue_B") ? 1 : 2;

    int total = live[venueIndex][5]; // total books for this venue
    
    // Redistribute total across shelves, capped at maxCapacity
    lock_guard<mutex> lock(stockMtx);
    int remaining = total;
    for (auto& s : allShelves) {
        s.currentBooks = min(remaining, s.maxCapacity);
        remaining -= s.currentBooks;
        if (remaining < 0) remaining = 0;
    }
    stockAmount = remaining; // leftover = warehouse stock
}

void Venue::processRequest() {
    while (systemRunning) {
        cout << "\033[2J\033[3J\033[H" << flush;

        map<int,vector<int>> live = venue_data();
        int venueIndex = (locationName == "Venue_A") ? 0 
                       : (locationName == "Venue_B") ? 1 : 2;
        {
            lock_guard<mutex> lock(stockMtx);
            stockAmount = live[venueIndex][5]; // total books for this venue
        }

        syncShelvesFromFile();
        int total = 0; for (auto& s : allShelves) total += s.currentBooks;

        cout << "LIVE MONITOR: " << locationName << " | CENTRAL STOCK: " << stockAmount << "\n" << endl;
        for (auto& s : allShelves) {
            cout << " Shelf " << s.id << " [";
            for(int i=0; i<s.maxCapacity; i++) cout << (i < s.currentBooks ? "=" : ".");
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
    auto init = [](string p, string v) { { ofstream f(p); f << v; } };
    map<int,vector<int>> count = venue_data();
    init("vA_stock.txt", to_string(count[0][5])); init("vA_shelves.txt", to_string(count[0][0]) + " " 
          + to_string(count[0][1]) + " " + to_string(count[0][2]) + " " +to_string(count[0][3]) + " " +
          to_string(count[0][4]));
    init("vB_stock.txt", to_string(count[1][5])); init("vB_shelves.txt", to_string(count[1][0]) + " " 
          + to_string(count[1][1]) + " " + to_string(count[1][2]) + " " +to_string(count[1][3]) + " " +
          to_string(count[1][4]));
    init("vC_stock.txt", to_string(count[2][5])); init("vC_shelves.txt", to_string(count[2][0]) + " " 
          + to_string(count[2][1]) + " " + to_string(count[2][2]) + " " +to_string(count[2][3]) + " " +
          to_string(count[2][4]));
}

bool keyPressed(char target) {
    if (_kbhit()) {
        char ch = (char)_getch();
        return (tolower(ch) == tolower(target));
    }
    return false;
}