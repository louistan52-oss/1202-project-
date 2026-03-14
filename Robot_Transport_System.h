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

extern atomic<bool> systemRunning;
enum class State { IDLE, WORKING, CHARGING, REFILLING };

class Shelves {
public:
    int id, currentBooks, maxCapacity;
    Shelves(int shelfId, int initialBooks, int cap = 4);
    bool isFull() const { return currentBooks >= maxCapacity; }
};

class Location {
protected:
    char idChar; 
    string locationName;
    int stockAmount;
    string stockFilePath;
    mutable mutex stockMtx;

public:
    Location(char vid, string name, string sPath);
    virtual ~Location() {}
    bool identifyAndExecute(char input);
    virtual void processRequest() = 0; 
    void saveStockToFile();
};

class Robot {
private:
    string name;
    int battery;
    int inventory = 0;
    State status = State::IDLE;
    mutable mutex rMtx;

public:
    Robot(string n);
    Robot(Robot&& other) noexcept;
    void startWorking(vector<Shelves>& venueShelves, int& venueStock, mutex& sMtx, Location& loc);
    void startRefilling(int& venueStock, mutex& sMtx, Location& loc);
    void startCharging();
    void display() const;
    State getStatus() const;
};

class Venue : public Location {
private:
    vector<Shelves> allShelves;
    vector<Robot> robots;
    string shelfDataPath;

public:
    Venue(char vid, string name, string stockFile, string shelfFile);
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