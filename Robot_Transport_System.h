#ifndef ROBOT_TRANSPORT_SYSTEM_H
#define ROBOT_TRANSPORT_SYSTEM_H

#include <iostream>
#include <vector>
#include <string>
#include <mutex>
#include <atomic>
#include <thread>
#include <chrono>

using namespace std;

// State Machine: Allows the system to track exactly what a robot is doing.
enum class State { IDLE, WORKING, CHARGING, REFILLING };

// Global shutdown flag: 'atomic' ensures all threads see the "stop" signal simultaneously.
extern atomic<bool> systemRunning;

class Shelves {
public:
    int id, currentBooks, maxCapacity;
    Shelves(int shelfId, int initialBooks, int cap = 4);
    bool isFull() const;
};

// Base Class: Uses Polymorphism so the Controller can manage any Venue generically.
class Location {
protected:
    char idChar;
    string locationName, stockFilePath;
    int stockAmount;
    mutex stockMtx; // Protects shared stock from simultaneous robot access.
public:
    Location(char vid, string name, string sPath);
    virtual ~Location();
    virtual void processRequest() = 0; // The Live Monitor UI.
    virtual string getQuickStatus() = 0; // The Dashboard HUD summary.
    bool identifyAndExecute(char input);
    void saveStockToFile();
};

class Robot {
private:
    string name;
    int battery;
    int inventory = 0;
    State status = State::IDLE;
    mutable mutex rMtx; // Protections for robot-specific data.
public:
    Robot(string n);
    Robot(Robot&& other) noexcept;
    State getStatus() const;
    int getBattery() const;
    string getName() const { return name; }
    void display() const;
    
    // Core Logic
    void startWorking(vector<Shelves>& venueShelves, int& vStock, mutex& sMtx, Location& loc);
    // 'needed' parameter ensures the robot only takes what is missing from shelves.
    void startRefilling(int& vStock, mutex& sMtx, Location& loc, int needed);
    void startCharging();
};

class Venue : public Location {
private:
    string shelfDataPath;
    vector<Shelves> allShelves;
    vector<Robot> robots;
public:
    Venue(char vid, string name, string stockFile, string shelfFile);
    void loadShelves();
    void saveShelves();
    void processRequest() override;
    string getQuickStatus() override;
};

class SystemController {
private:
    vector<Location*> fleet;
public:
    SystemController();
    ~SystemController();
    void createDummyEnvironment();
    void run();
};

// Utility: Allows the program to listen for 'Q' without pausing the simulation.
bool keyPressed(char target);

#endif