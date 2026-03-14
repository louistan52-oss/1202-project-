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

using namespace std;

// --- GLOBAL FLAGS ---
// 'atomic' ensures thread-safe access to this boolean without needing a mutex.
extern atomic<bool> systemRunning;

// State tracking for robot behavior.
enum class State { IDLE, WORKING, CHARGING, REFILLING };

// --- CLASS: SHELVES (Component) ---
// Minimal unit representing a single physical shelf.
class Shelves {
public:
    int id, currentBooks, maxCapacity;
    Shelves(int shelfId, int initialBooks, int cap = 4);
    bool isFull() const { return currentBooks >= maxCapacity; }
};

// --- BASE CLASS: LOCATION (Inheritance Base) ---
// Uses Abstraction to provide a blueprint for all physical sites.
class Location {
protected:
    char idChar;            // Unique identifier (A, B, C)
    string locationName;
    int stockAmount;        // Shared resource pool
    string stockFilePath;
    mutable mutex stockMtx; // 'mutable' allows locking in 'const' methods

public:
    Location(char vid, string name, string sPath);
    virtual ~Location() {} // Virtual destructor prevents memory leaks in derived classes.

    // Core Logic: Checks if the object matches the user's keystroke.
    bool identifyAndExecute(char input);
    
    // Pure virtual: Every Venue must implement its own reporting logic.
    virtual void processRequest() = 0; 
    
    char getID() const { return idChar; }
    void saveStockToFile();
};

// --- CLASS: ROBOT (The Worker) ---
// Handles the asynchronous background tasks.
class Robot {
private:
    string name;
    int battery;
    int inventory = 0;
    State status = State::IDLE;
    mutable mutex rMtx; // Critical for protecting robot stats during UI refreshes.

public:
    Robot(string n);
    Robot(Robot&& other) noexcept; // Required for storing objects with mutexes in vectors.
    
    void startWorking(vector<Shelves>& venueShelves, int& venueStock, mutex& sMtx, Location& loc);
    void startRefilling(int& venueStock, mutex& sMtx, Location& loc);
    void startCharging();
    void display() const;
    State getStatus() const;
};

// --- DERIVED CLASS: VENUE (The Implementation) ---
// Manages the relationship between shelves and robots at a specific site.
class Venue : public Location {
private:
    vector<Shelves> allShelves;
    vector<Robot> robots;
    string shelfDataPath;

public:
    Venue(char vid, string name, string stockFile, string shelfFile);
    void processRequest() override; // Implements the specific display for a venue.
    void loadShelves();
    void saveShelves();
};

// --- SYSTEM CONTROLLER (The Manager) ---
// New class that replaces the logic previously held in main().
class SystemController {
private:
    vector<Location*> fleet; // Polymorphic collection of venues.

public:
    SystemController();  // Loads settings.
    ~SystemController(); // Cleans memory.
    void run();          // Entry point for the user loop.
    void shutdown();     // Graceful thread termination.
};

#endif