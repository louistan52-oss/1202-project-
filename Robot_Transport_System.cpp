#include "Robot_Transport_System.h"

using namespace std::chrono_literals;

// Global flag to tell background threads when to quit.
atomic<bool> systemRunning(true);

// --- SYSTEM CONTROLLER: Initialization and Lifecycle ---

SystemController::SystemController() {
    // Process: Load the configuration file to build the venue list.
    // Reason: Decouples code from data; adding a venue only requires a text edit.
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
    // Process: Iterate through pointers and free memory.
    // Reason: Prevents memory leaks (Heap management).
    for (auto loc : fleet) delete loc;
}

void SystemController::run() {
    char userInput;
    while (true) {
        cout << "\nEnter Venue ID (A-C) or 'Q' to Quit: ";
        if (!(cin >> userInput)) break;
        if (toupper(userInput) == 'Q') break;

        bool matched = false;
        for (auto loc : fleet) {
            // Process: Polymorphism.
            // Reason: Let the object decide if it matches the input and run its own code.
            if (loc->identifyAndExecute(userInput)) {
                matched = true;
                break;
            }
        }
        if (!matched) cout << "ID '" << userInput << "' not recognized." << endl;
    }
    shutdown();
}

void SystemController::shutdown() {
    // Process: Signal threads and wait briefly.
    // Reason: Ensures robots finish their last write-to-file before the program closes.
    systemRunning = false;
    this_thread::sleep_for(500ms);
}

// --- VENUE: Display and Auto-Management ---

void Venue::processRequest() {
    saveShelves();      // Save current status to file.
    saveStockToFile();  // Save stock levels to file.

    cout << "\n--- " << locationName << " Report ---" << endl;
    cout << " Central Stock: " << stockAmount << " units" << endl;
    
    int totalOnShelves = 0;
    for (auto& s : allShelves) {
        cout << " Shelf " << s.id << ": [" << s.currentBooks << "/4] ";
        totalOnShelves += s.currentBooks;
    }
    cout << "\n----------------------------------------" << endl;

    bool active = false;
    for (auto& r : robots) {
        r.display();
        if (r.getStatus() != State::IDLE) active = true;
    }

    // Auto-Deployment:
    // If no one is working and shelves need books, trigger an Idle robot.
    if (!active && totalOnShelves < 20) {
        for (auto& r : robots) {
            if (r.getStatus() == State::IDLE) {
                r.startWorking(allShelves, stockAmount, stockMtx, *this);
                break;
            }
        }
    }
}

// --- ROBOT: Background Threading ---

void Robot::startWorking(vector<Shelves>& venueShelves, int& venueStock, mutex& sMtx, Location& loc) {
    { lock_guard<mutex> lock(rMtx); status = State::WORKING; }
    
    // Process: Lambda Threading with Detach.
    // Reason: Allows the robot to "think" and "move" without blocking the user's menu.
    thread t([this, &venueShelves, &venueStock, &sMtx, &loc]() {
        while (systemRunning) {
            this_thread::sleep_for(1s); // Simulated labor time.
            
            lock_guard<mutex> lock(rMtx); // Protect robot data during update.
            
            // Logic: Find first available shelf.
            Shelves* target = nullptr;
            for (auto& s : venueShelves) if (!s.isFull()) { target = &s; break; }

            if (!target) { status = State::IDLE; break; } // Work finished.

            if (inventory <= 0) {
                status = State::REFILLING;
                // Chain: If empty, start a new refill thread and exit this one.
                thread(&Robot::startRefilling, this, ref(venueStock), ref(sMtx), ref(loc)).detach();
                break;
            }

            target->currentBooks++;
            inventory--;
            battery -= 2;
        }
    });
    t.detach();
}

// --- MAIN: The Clean Entry Point ---

int main() {
    // Process: Seed random for unique battery levels each run.
    srand(static_cast<unsigned int>(time(0)));

    // Process: Instantiate the controller.
    // Reason: All system complexity is hidden inside the SystemController class.
    SystemController controller;
    controller.run();

    return 0;
}