#include "Robot_Transport_System.h"

using namespace std::chrono_literals;

// Actual definition of the global flag declared in the header.
atomic<bool> systemRunning(true);

// --- SHELVES IMPLEMENTATION ---
Shelves::Shelves(string path, int cap) : maxCapacity(cap), filePath(path) {
    // Attempt to load the current count from the .txt file on startup
    ifstream inFile(filePath);
    if (inFile.is_open()) {
        inFile >> currentBooks;
        inFile.close();
    } else {
        currentBooks = 0; // Default if file is missing
    }
}

void Shelves::saveToFile() {
    // Overwrites the .txt file with the updated number of books
    ofstream outFile(filePath);
    if (outFile.is_open()) {
        outFile << currentBooks;
        outFile.close();
    }
}

bool Shelves::needsRestock() const { return currentBooks < maxCapacity; }

// --- LOCATION IMPLEMENTATION ---
Location::Location(int vid, string name, string sPath) 
    : id(vid), locationName(name), stockFilePath(sPath) {
    // Load the initial stock pile count from the .txt file
    ifstream inFile(stockFilePath);
    if (inFile.is_open()) {
        inFile >> stockAmount;
        inFile.close();
    } else {
        stockAmount = 100;
    }
}

Location::~Location() {}

void Location::saveStockToFile() {
    // Thread-safe writing of the stock pile to the .txt file
    lock_guard<mutex> lock(stockMtx);
    ofstream outFile(stockFilePath);
    if (outFile.is_open()) {
        outFile << stockAmount;
        outFile.close();
    }
}

// --- ROBOT IMPLEMENTATION ---
Robot::Robot(string n) : name(n), battery(rand() % 31 + 60) {}

// Move constructor logic: safely transfers data between robot objects
Robot::Robot(Robot&& other) noexcept {
    lock_guard<mutex> lock(other.rMtx);
    name = move(other.name);
    battery = other.battery;
    inventory = other.inventory;
    status = other.status;
}

string Robot::getName() const { return name; }
State Robot::getStatus() const { lock_guard<mutex> lock(rMtx); return status; }

void Robot::startWorking(Shelves& venueShelves, int& venueStock, mutex& sMtx, Location& loc) {
    {
        lock_guard<mutex> lock(rMtx);
        status = State::WORKING;
    }
    // Launch background thread: Robot operates independently of the main menu
    thread t([this, &venueShelves, &venueStock, &sMtx, &loc]() {
        while (systemRunning) {
            this_thread::sleep_for(1s); // Simulate time taken to place one book
            lock_guard<mutex> lock(rMtx);

            // EXIT CONDITION 1: Shelf is full
            if (!venueShelves.needsRestock()) {
                status = State::IDLE;
                venueShelves.saveToFile(); // Save shelf progress
                break;
            }
            // EXIT CONDITION 2: Robot out of books
            if (inventory <= 0) {
                status = State::REFILLING;
                venueShelves.saveToFile(); 
                // Start the refill process (moves to another thread)
                thread(&Robot::startRefilling, this, ref(venueStock), ref(sMtx), ref(loc)).detach();
                break;
            }

            // Perform Work
            venueShelves.currentBooks++;
            inventory--;
            battery -= 2;

            // EXIT CONDITION 3: Low battery
            if (battery <= 20) {
                status = State::CHARGING;
                venueShelves.saveToFile();
                thread(&Robot::startCharging, this).detach();
                break;
            }
        }
    });
    t.detach(); // Allow the thread to live on its own
}

void Robot::startRefilling(int& venueStock, mutex& sMtx, Location& loc) {
    this_thread::sleep_for(3s); // Simulated travel time to the stock room
    if (systemRunning) {
        lock_guard<mutex> sLock(sMtx); // Lock the Venue stock pile
        lock_guard<mutex> rLock(rMtx); // Lock the Robot's data
        
        if (venueStock > 0) {
            int take = min(5, venueStock); // Take up to 5 books
            venueStock -= take;
            inventory = take;
            status = State::IDLE;
            loc.saveStockToFile(); // Persist the reduction in stock to .txt
        } else {
            status = State::IDLE; // No stock left to take
        }
    }
}

void Robot::startCharging() {
    while (systemRunning) {
        this_thread::sleep_for(500ms);
        lock_guard<mutex> lock(rMtx);
        battery += 5;
        if (battery >= 100) { 
            battery = 100; 
            status = State::IDLE; 
            break; 
        }
    }
}

void Robot::display() const {
    lock_guard<mutex> lock(rMtx);
    cout << left << setw(10) << name << " | Bat: " << setw(3) << battery 
         << "% | Inv: " << inventory << "/5 | Status: " << (int)status << endl;
}

// --- VENUE IMPLEMENTATION ---
Venue::Venue(int vid, string name, string stockFile, string shelfFile, int shelfCap) 
    : Location(vid, name, stockFile), venueShelves(shelfFile, shelfCap) {
    robots.emplace_back("Alpha");
    robots.emplace_back("Bravo");
    robots.emplace_back("Charlie");
}

void Venue::refreshVenue() {
    cout << "\n--- " << locationName << " (ID: " << id << ") ---" << endl;
    cout << "Stock: " << stockAmount << " | Shelf: " << venueShelves.currentBooks << "/" << venueShelves.maxCapacity << endl;
    
    // Check if any robot is currently active
    bool anyoneWorking = false;
    for (auto& r : robots) {
        r.display();
        if (r.getStatus() == State::WORKING || r.getStatus() == State::REFILLING) 
            anyoneWorking = true;
    }
    
    // Deployment Manager: If work is needed and no one is on it, send an Idle robot
    if (!anyoneWorking && venueShelves.needsRestock()) {
        for (auto& r : robots) {
            if (r.getStatus() == State::IDLE) {
                r.startWorking(venueShelves, stockAmount, stockMtx, *this);
                break;
            }
        }
    }
}

// --- MAIN ENTRY POINT ---
int main() {
    srand(static_cast<unsigned int>(time(0)));
    
    // Initialization of the fleet with specific .txt file paths
    vector<Venue> fleet;
    fleet.emplace_back(1, "Main Lib", "v1_stock.txt", "v1_shelf.txt", 20);
    fleet.emplace_back(2, "Archive", "v2_stock.txt", "v2_shelf.txt", 50);

    int choice;
    while (true) {
        cout << "\nSelect Venue (1-2) or 0 to Exit: ";
        if (!(cin >> choice)) { 
            cin.clear(); 
            cin.ignore(1000, '\n'); 
            continue; 
        }
        if (choice == 0) break;
        if (choice >= 1 && choice <= 2) fleet[choice - 1].refreshVenue();
    }

    // SHUTDOWN: Set the flag to false to stop all background loops
    systemRunning = false;
    this_thread::sleep_for(500ms); 
    return 0;
}