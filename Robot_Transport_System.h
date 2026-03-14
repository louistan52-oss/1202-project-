#ifndef ROBOT_TRANSPORT_SYSTEM_H
#define ROBOT_TRANSPORT_SYSTEM_H

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <fstream>
#include <iomanip>
#include <algorithm>

using namespace std;

// --- GLOBAL ATOMIC FLAG ---
// We use 'atomic' because multiple threads check this flag simultaneously.
// It prevents "data races" where one thread reads the flag while another writes to it.
extern atomic<bool> systemRunning;

enum class State { IDLE, WORKING, CHARGING, REFILLING };

// --- CLASS: SHELVES ---
// Purpose: Minimal data structure to track individual shelf capacity.
class Shelves {
public:
    int id;
    int currentBooks;
    int maxCapacity;
    Shelves(int shelfId, int initialBooks, int cap = 4);
    bool isFull() const { return currentBooks >= maxCapacity; }
};

// --- BASE CLASS: LOCATION (Abstraction) ---
// We use inheritance here to define common properties of any "place" in the system.
class Location {
protected:
    char idChar;            // Symbolic ID (A, B, C)
    string locationName;
    int stockAmount;        // Shared resource for the venue
    string stockFilePath;   // File persistence path
    
    // 'mutable' allows us to lock the mutex even inside 'const' functions.
    mutable mutex stockMtx; 

public:
    Location(char vid, string name, string sPath);
    virtual ~Location() {} // Virtual destructor is CRITICAL for safe inheritance cleanup.

    // This function encapsulates the selection logic inside the object itself.
    bool identifyAndExecute(char input);
    
    // 'pure virtual' (= 0) makes this an Abstract Class. 
    // It forces Derived classes to implement their own display logic.
    virtual void processRequest() = 0; 
    
    char getID() const { return idChar; }
    void saveStockToFile();
};

// --- CLASS: ROBOT (Worker) ---
class Robot {
private:
    string name;
    int battery;
    int inventory;
    State status;
    mutable mutex rMtx; // Protects robot's internal state from UI thread interference.

public:
    Robot(string n);
    // Move constructor: required to put objects with mutexes into a std::vector.
    Robot(Robot&& other) noexcept;
    
    // Function prototypes for background tasks
    void startWorking(vector<Shelves>& venueShelves, int& venueStock, mutex& sMtx, Location& loc);
    void startRefilling(int& venueStock, mutex& sMtx, Location& loc);
    void startCharging();
    void display() const;
    State getStatus() const;
};

// --- DERIVED CLASS: VENUE ---
// Uses 'public' inheritance to represent a "is-a" relationship with Location.
class Venue : public Location {
private:
    vector<Shelves> allShelves; // Composition: Venue "has" multiple shelves.
    vector<Robot> robots;
    string shelfDataPath;

public:
    Venue(char vid, string name, string stockFile, string shelfFile);
    
    // 'override' helps the compiler catch errors if the signature doesn't match the base.
    void processRequest() override; 
    
    void loadShelves();
    void saveShelves();
};

class SystemController {
private:
    vector<Location*> fleet;
    void createDummyEnvironment();

public:
    SystemController();
    ~SystemController();
    void run();
    void shutdown();
};

#endif