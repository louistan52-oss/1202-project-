#include "Robot_Transport_System.h"

using namespace std::chrono_literals;
atomic<bool> systemRunning(true);

// --- SYSTEM CONTROLLER ---
SystemController::SystemController() {
    ifstream configCheck("venues_config.txt");
    if (!configCheck) {
        createDummyEnvironment();
    } else {
        configCheck.close();
    }

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
    cout << "[INIT] Creating dummy test files..." << endl;
    ofstream config("venues_config.txt");
    config << "A Venue_A vA_stock.txt vA_shelves.txt\n"
           << "B Venue_B vB_stock.txt vB_shelves.txt\n"
           << "C Venue_C vC_stock.txt vC_shelves.txt";
    config.close();

    // Venue A: Half-full shelves
    ofstream sa("vA_stock.txt"); sa << "100"; sa.close();
    ofstream sha("vA_shelves.txt"); sha << "4 4 0 0 0"; sha.close();

    // Venue B: Empty shelves
    ofstream sb("vB_stock.txt"); sb << "50"; sb.close();
    ofstream shb("vB_shelves.txt"); shb << "0 0 0 0 0"; shb.close();

    // Venue C: Full shelves (Robot should stay IDLE)
    ofstream sc("vC_stock.txt"); sc << "10"; sc.close();
    ofstream shc("vC_shelves.txt"); shc << "4 4 4 4 4"; shc.close();
}

void SystemController::run() {
    char userInput;
    while (true) {
        cout << "\nEnter Venue ID (A-C) or 'Q' to Quit: ";
        if (!(cin >> userInput)) break;
        if (toupper(userInput) == 'Q') break;

        bool matched = false;
        for (auto loc : fleet) {
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
    systemRunning = false;
    this_thread::sleep_for(500ms);
}

// --- VENUE & LOCATION ---
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

void Venue::processRequest() {
    loadShelves(); 
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

    if (!active && totalOnShelves < 20) {
        for (auto& r : robots) {
            if (r.getStatus() == State::IDLE) {
                r.startWorking(allShelves, stockAmount, stockMtx, *this);
                break;
            }
        }
    }
}

// --- MAIN ---
int main() {
    srand(static_cast<unsigned int>(time(0)));
    SystemController controller;
    controller.run();
    return 0;
}