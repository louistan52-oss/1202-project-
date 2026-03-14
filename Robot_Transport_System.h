#ifndef ROBOT_TRANSPORT_SYSTEM_H
#define ROBOT_TRANSPORT_SYSTEM_H

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <fstream>

using namespace std;

// --- STEP 1: GLOBAL EXTERNAL FLAG ---
// 'extern' tells the compiler that 'systemRunning' is defined in the .cpp file.
// This allows background threads to check this flag to see if they should stop.
extern atomic<bool> systemRunning;

// State tracking for the robot's finite state machine logic.
enum class State { IDLE, WORKING, CHARGING, REFILLING };

// --- STEP 2: SHELVES CLASS (Composition Element) ---
class Shelves {
public:
    int currentBooks;
    int maxCapacity;
    string filePath; // Path to the .txt file storing the shelf count

    Shelves(string path, int cap = 20);
    void saveToFile();           // Writes currentBooks to the .txt file
    bool needsRestock() const;    // Returns true if shelf isn't full
};

// --- STEP 3: LOCATION CLASS (Base Class for Inheritance) ---
class Location {
protected:
    int id;
    string locationName;
    int stockAmount;      // Total books available in the "warehouse" for this location
    string stockFilePath; // Path to the .txt file storing the stock amount
    mutable mutex stockMtx; // Protects stockAmount during simultaneous refills

public:
    Location(int vid, string name, string sPath);
    virtual ~Location();         // Virtual destructor ensures derived classes clean up correctly
    void saveStockToFile();      // Writes stockAmount to the .txt file
};

// --- STEP 4: ROBOT CLASS (The Worker) ---
class Robot {
private:
    string name;
    int battery;
    int inventory = 0;           // Books currently held by the robot
    State status = State::IDLE;
    mutable mutex rMtx;          // Protects robot's internal data (battery, state)

public:
    Robot(string n);
    Robot(Robot&& other) noexcept; // Move constructor needed to store robots in a vector
    
    string getName() const;
    State getStatus() const;
    
    // Background execution methods
    void startWorking(Shelves& venueShelves, int& venueStock, mutex& sMtx, Location& loc);
    void startRefilling(int& venueStock, mutex& sMtx, Location& loc);
    void startCharging();
    void display() const;
};

// --- STEP 5: VENUE CLASS (Derived Class) ---
// Inherits from Location. A Venue "is-a" Location and "has-a" Shelf + Robots.
class Venue : public Location {
private:
    Shelves venueShelves;
    vector<Robot> robots;

public:
    Venue(int vid, string name, string stockFile, string shelfFile, int shelfCap);
    void refreshVenue(); // Updates the UI and triggers the auto-deployment logic
};

#endif