#ifndef QMS_H
#define QMS_H

#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <initializer_list>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;
class QMS_Venue; // Forward declaration of QMS_Venue so Queue can reference it in friend functions

class Queue {
private:
    size_t space; // Tracks remaining slots (max 10)
    string time; // 24hr format string (e.g., "1500")
    char venue; // Venue identifier (A, B, or C)
    set<string> queue; // Set of NRICs to ensure unique bookings per slot

public:
    // Constructors
    Queue(); // default ctor
    Queue(const string& time); // non-default ctor w parameter time
    Queue(const char&, const string& time); // non-default ctor w parameter venue and time

    // Getters
    string getTime() const { return time;}
    char getVenue() const { return venue;}
    const set<string>& getQueue() const; // get all data in queue and return it

    // Queue Management
    int addToQueue(string IC); // Adds user; returns queue position or 0 if full/exists
    void removeFromQueue(string IC); // Removes user from this specific slot
    bool isFull() const; // Checks if capacity has reached 0
    
    // Friend functions for external access to private members (Logic & I/O)
    friend ostream& operator<<(ostream& os, const Queue& queue); // operator overload for <<
    friend bool checkQueueStatus(string IC, const Queue& obj); // check if current IC is in queue
    friend void printQueueDetails(string IC, const Queue& obj); // Prints queue details to std output
    
    // Friends for File Persistence
    friend bool save_curr_timeslots(const map<char, class QMS_Venue>&);
    friend bool load_curr_timeslots(map<char, class QMS_Venue>&, map<int, string>&);

    
};

class QMS_Venue { // temporary class
    map<string, Queue> timeslots;

public:
    QMS_Venue(); // default ctor
    QMS_Venue(map<string, Queue>& times); // non-default ctor
    ~QMS_Venue(); 

    // Management & Access
    void push_time(char, string); // Initializes a new slot
    
    // Operator overloads for easy timeslot lookup: venue["1500"]
    Queue& operator[](const string&); 
    const Queue& operator[](const string&) const;

    // Data retrieva
    map<string, Queue>& getTimeSlots();
    const map<string, Queue>& getTimeSlots() const; // gets all timeslots
};

// --- Global System Function Prototypes ---

// File Handling: Persistence of the booking database
bool save_curr_timeslots(const map<char, QMS_Venue>& venues);
bool load_curr_timeslots(map<char, QMS_Venue>& venues, map<int, string>& timeslots);

// Logic: Handles the "Check-out" process
bool leave_venue(std::string IC, std::map<char, QMS_Venue>& venues);

// UI: Main interaction loop for the Queue Management System
int QMSMenu(string IC, map<char, QMS_Venue>& venues, const map<int, string>& timeslots);

#endif
