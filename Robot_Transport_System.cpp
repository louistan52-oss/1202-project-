#include "Robot_Transport_System.h"

using namespace std::chrono_literals;

atomic<bool> systemRunning(true);

// --- SHELVES ---
Shelves::Shelves(int shelfId, int initialBooks, int cap) 
    : id(shelfId), currentBooks(initialBooks), maxCapacity(cap) {}

// --- LOCATION ---
Location::Location(char vid, string name, string sPath) 
    : idChar(vid), locationName(name), stockFilePath(sPath) {
    ifstream inFile(stockFilePath);
    if (inFile.is_open()) { inFile >> stockAmount; inFile.close(); }
    else { stockAmount = 100; }
}

bool Location::identifyAndExecute(char input) {
    if (toupper(input) == idChar) {
        this->processRequest();
        return true;
    }
    return false;
}

void Location::saveStockToFile() {
    // CRITICAL: Scoped lock so file isn't held open
    lock_guard<mutex> lock(stockMtx);
    ofstream outFile(stockFilePath);
    if (outFile.is_open()) { 
        outFile << stockAmount; 
        outFile.close(); 
    }
}

// --- ROBOT ---
Robot::Robot(string n) : name(n), battery(rand() % 40 + 60) {}

Robot::Robot(Robot&& other) noexcept {
    lock_guard<mutex> lock(other.rMtx);
    name = move(other.name);
    battery = other.battery;
    inventory = other.inventory;
    status = other.status;
}

State Robot::getStatus() const { 
    lock_guard<mutex> lock(rMtx); 
    return status; 
}

void Robot::display() const {
    lock_guard<mutex> lock(rMtx);
    string s;
    if (status == State::WORKING) s = "WORKING";
    else if (status == State::CHARGING) s = "CHARGING";
    else if (status == State::REFILLING) s = "REFILLING";
    else s = "IDLE";

    cout << left << setw(10) << name << " | Bat: " << setw(3) << battery 
         << "% | Inv: " << inventory << "/5 | Status: " << s << endl;
}

void Robot::startWorking(vector<Shelves>& venueShelves, int& venueStock, mutex& sMtx, Location& loc) {
    { lock_guard<mutex> lock(rMtx); status = State::WORKING; }
    
    thread t([this, &venueShelves, &venueStock, &sMtx, &loc]() {
        while (systemRunning) {
            this_thread::sleep_for(1s); // Sleep UNLOCKED to prevent hanging

            {
                lock_guard<mutex> lock(rMtx);
                
                if (battery <= 10) {
                    status = State::CHARGING;
                    thread(&Robot::startCharging, this).detach();
                    return; 
                }
                
                if (inventory <= 0) {
                    status = State::REFILLING;
                    thread(&Robot::startRefilling, this, ref(venueStock), ref(sMtx), ref(loc)).detach();
                    return; 
                }

                Shelves* target = nullptr;
                for (auto& s : venueShelves) {
                    if (!s.isFull()) { target = &s; break; }
                }

                if (target) {
                    target->currentBooks++;
                    inventory--;
                    battery -= 2;
                } else {
                    status = State::IDLE;
                    return; 
                }
            } 
        }
    });
    t.detach();
}

void Robot::startRefilling(int& venueStock, mutex& sMtx, Location& loc) {
    this_thread::sleep_for(2s); 
    if (systemRunning) {
        {
            lock_guard<mutex> sLock(sMtx);
            lock_guard<mutex> rLock(rMtx);
            int take = min(5, venueStock);
            venueStock -= take;
            inventory = take;
            status = State::IDLE; 
        }
        loc.saveStockToFile(); // Save AFTER releasing robot mutex
    }
}

void Robot::startCharging() {
    while (systemRunning) {
        this_thread::sleep_for(1s);
        lock_guard<mutex> lock(rMtx);
        battery += 20;
        if (battery >= 100) {
            battery = 100;
            status = State::IDLE;
            return;
        }
    }
}

// --- VENUE ---
Venue::Venue(char vid, string name, string stockFile, string shelfFile) 
    : Location(vid, name, stockFile), shelfDataPath(shelfFile) {
    loadShelves();
    robots.emplace_back("Alpha");
    robots.emplace_back("Bravo");
}

void Venue::loadShelves() {
    ifstream inFile(shelfDataPath);
    if (!inFile) return;

    allShelves.clear();
    int val;
    for(int i = 0; i < 5; i++) {
        if (inFile >> val) allShelves.emplace_back(i+1, val);
        else allShelves.emplace_back(i+1, 0);
    }
    inFile.close();
}

void Venue::saveShelves() {
    ofstream outFile(shelfDataPath);
    if (outFile.is_open()) {
        for (auto& s : allShelves) outFile << s.currentBooks << " ";
        outFile.close();
    }
}

void Venue::processRequest() {
    // 1. Update files and load state
    saveShelves(); 
    saveStockToFile();
    loadShelves(); 
    
    cout << "\n--- " << locationName << " Report ---" << endl;
    cout << " Central Stock: " << stockAmount << " units" << endl;
    
    int total = 0;
    for (auto& s : allShelves) {
        cout << " Shelf " << s.id << ": [" << s.currentBooks << "/4]  ";
        total += s.currentBooks;
    }
    cout << "\n----------------------------------------" << endl;
    
    bool anyActive = false;
    for (auto& r : robots) {
        r.display();
        if (r.getStatus() != State::IDLE) anyActive = true;
    }

    // 2. Logic: If work is needed and no robot is busy, start one.
    if (!anyActive && total < 20 && stockAmount > 0) {
        for (auto& r : robots) {
            if (r.getStatus() == State::IDLE) {
                r.startWorking(allShelves, stockAmount, stockMtx, *this);
                break;
            }
        }
    }
}

// --- SYSTEM CONTROLLER ---
SystemController::SystemController() {
    ifstream check("venues_config.txt");
    if (!check) createDummyEnvironment();
    else check.close();

    ifstream configFile("venues_config.txt");
    if (configFile.is_open()) {
        char vChar; string vName, sFile, shFile;
        while (configFile >> vChar >> vName >> sFile >> shFile) {
            fleet.push_back(new Venue(vChar, vName, sFile, shFile));
        }
        configFile.close();
    }
}

SystemController::~SystemController() {
    for (auto loc : fleet) delete loc;
}

void SystemController::createDummyEnvironment() {
    ofstream config("venues_config.txt");
    config << "A Venue_A vA_stock.txt vA_shelves.txt\n"
           << "B Venue_B vB_stock.txt vB_shelves.txt\n"
           << "C Venue_C vC_stock.txt vC_shelves.txt";
    config.close();
    
    auto init = [](string p, string v) { ofstream f(p); f << v; f.close(); };
    init("vA_stock.txt", "100"); init("vA_shelves.txt", "4 4 0 0 0");
    init("vB_stock.txt", "50");  init("vB_shelves.txt", "0 0 0 0 0");
    init("vC_stock.txt", "10");  init("vC_shelves.txt", "4 4 4 4 4");
}

void SystemController::run() {
    char input;
    while (true) {
        cout << "\nEnter Venue ID (A-C) or 'Q' to Quit: ";
        if (!(cin >> input)) break;
        if (toupper(input) == 'Q') break;
        
        bool matched = false;
        for (auto loc : fleet) {
            if (loc->identifyAndExecute(input)) {
                matched = true;
                break;
            }
        }
        if (!matched) cout << "ID not recognized." << endl;
    }
    shutdown();
}

    // GRACEFUL SHUTDOWN
    // Setting this to false causes all while(systemRunning) loops to end.
    systemRunning = false;
    this_thread::sleep_for(500ms);
}

int main() {
    srand(static_cast<unsigned int>(time(0)));
    SystemController controller;
    controller.run();
    return 0;
}