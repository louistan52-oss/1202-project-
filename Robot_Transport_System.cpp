#include "Robot_Transport_System.h"

using namespace std::chrono_literals;

// Define the global shutdown flag.
atomic<bool> systemRunning(true);

// --- LOCATION IMPLEMENTATION ---
Location::Location(char vid, string name, string sPath) 
    : idChar(vid), locationName(name), stockFilePath(sPath) {
    // We load from file immediately in the constructor to ensure data persistence.
    ifstream inFile(stockFilePath);
    if (inFile.is_open()) { inFile >> stockAmount; inFile.close(); }
    else { stockAmount = 100; } // Default if file doesn't exist.
}

// Logic: Instead of a 'switch' in main, each object decides if it should run.
bool Location::identifyAndExecute(char input) {
    if (toupper(input) == idChar) {
        this->processRequest(); // Calls the specific Venue display logic.
        return true;
    }
    return false;
}

void Location::saveStockToFile() {
    // lock_guard automatically unlocks the mutex when it goes out of scope (RAII).
    lock_guard<mutex> lock(stockMtx);
    ofstream outFile(stockFilePath);
    if (outFile.is_open()) { outFile << stockAmount; outFile.close(); }
}

// --- VENUE IMPLEMENTATION ---
void Venue::processRequest() {
    saveShelves();      // Save current state to disk before viewing.
    saveStockToFile();

    // UI Formatting using <iomanip> for clean alignment.
    cout << "\n========================================" << endl;
    cout << " STATUS REPORT: " << locationName << " (" << idChar << ")" << endl;
    cout << "========================================" << endl;
    cout << " [CENTRAL STOCK]: " << stockAmount << " books" << endl;
    cout << "----------------------------------------" << endl;

    int totalBooks = 0;
    for (size_t i = 0; i < allShelves.size(); ++i) {
        cout << "  Shelf " << (i + 1) << ": [";
        // Visual indicator loop.
        for (int b = 0; b < allShelves[i].maxCapacity; ++b) {
            cout << (b < allShelves[i].currentBooks ? "■" : " ");
        }
        cout << "] (" << allShelves[i].currentBooks << "/4)" << endl;
        totalBooks += allShelves[i].currentBooks;
    }

    cout << "----------------------------------------" << endl;
    cout << "\n [ROBOT STATUS]:" << endl;
    
    bool active = false;
    for (const auto& r : robots) {
        r.display();
        if (r.getStatus() != State::IDLE) active = true;
    }

    // AUTO-DEPLOYMENT LOGIC:
    // If no robots are busy and shelves aren't full (20 books total), start one.
    if (!active && totalBooks < 20) {
        for (auto& r : robots) {
            if (r.getStatus() == State::IDLE) {
                // Pass refs and pointers so the thread works on the actual data, not a copy.
                r.startWorking(allShelves, stockAmount, stockMtx, *this);
                break;
            }
        }
    }
    cout << "========================================" << endl;
}

// --- ROBOT LOGIC & THREADING ---
void Robot::startWorking(vector<Shelves>& venueShelves, int& venueStock, mutex& sMtx, Location& loc) {
    { lock_guard<mutex> lock(rMtx); status = State::WORKING; }
    
    // We launch a Lambda function in a new thread.
    // We pass dependencies by reference (&) to modify the Venue's data in the background.
    thread t([this, &venueShelves, &venueStock, &sMtx, &loc]() {
        while (systemRunning) {
            this_thread::sleep_for(1s); // Simulate travel/placement time.
            
            lock_guard<mutex> lock(rMtx);
            
            // Search for the next available shelf.
            Shelves* target = nullptr;
            for (auto& s : venueShelves) if (!s.isFull()) { target = &s; break; }

            if (!target) { status = State::IDLE; break; }
            
            // If robot is empty, chain to the Refilling process.
            if (inventory <= 0) {
                status = State::REFILLING;
                // Detach another thread to handle the long refill trip.
                thread(&Robot::startRefilling, this, ref(venueStock), ref(sMtx), ref(loc)).detach();
                break;
            }

            target->currentBooks++;
            inventory--;
            battery -= 2;

            // Low battery logic.
            if (battery <= 20) {
                status = State::CHARGING;
                thread(&Robot::startCharging, this).detach();
                break;
            }
        }
    });
    // .detach() allows the thread to run independently of the main menu loop.
    t.detach();
}

// --- MAIN ENTRY POINT ---
int main() {
    srand(static_cast<unsigned int>(time(0)));
    
    // Polymorphic container: we store base pointers but they point to derived Venues.
    vector<Location*> fleet;

    // Config file allows us to change the system without recompiling.
    ifstream configFile("venues_config.txt");
    if (configFile.is_open()) {
        char vChar; string vName, sFile, shFile;
        while (configFile >> vChar >> vName >> sFile >> shFile) {
            fleet.push_back(new Venue(vChar, vName, sFile, shFile));
        }
        configFile.close();
    }

    char userInput;
    while (true) {
        cout << "\nEnter Venue ID (A-C) or 'Q' to Quit: ";
        if (!(cin >> userInput)) break;
        if (toupper(userInput) == 'Q') break;

        bool matched = false;
        for (auto loc : fleet) {
            // Polymorphism: loc identifies itself and calls its own processRequest.
            if (loc->identifyAndExecute(userInput)) {
                matched = true;
                break;
            }
        }
        if (!matched) cout << "ID '" << userInput << "' not recognized." << endl;
    }

    // GRACEFUL SHUTDOWN
    // Setting this to false causes all while(systemRunning) loops to end.
    systemRunning = false;
    
    // Memory management: clean up the pointers created with 'new'.
    for (auto loc : fleet) delete loc;
    
    this_thread::sleep_for(500ms); // Brief pause to let threads exit before OS kills them.
    return 0;
}