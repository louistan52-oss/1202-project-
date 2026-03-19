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
class QMS_Venue;

class Queue {
private:
    size_t space;
    string time;
    char venue;
    set<string> queue;

public:
    Queue(); // default ctor
    Queue(const string& time); // non-default ctor w parameter time
    Queue(const char&, const string& time); // non-default ctor w parameter venue and time

    string getTime() const { return time;}
    char getVenue() const { return venue;}
    int addToQueue(string IC); // add to the queue depending on IC, returns the queue number, otherwise if no space returns 0
    void removeFromQueue(string IC);
    bool isFull() const; // if there is no space, return true;
    
    // Declarations for the getters used in .cpp
    const set<string>& getQueue() const; // get all data in queue and return it
    friend ostream& operator<<(ostream& os, const Queue& queue); // operator overload for <<
    friend bool checkQueueStatus(string IC, const Queue& obj); // check if current IC is in queue
    friend void printQueueDetails(string IC, const Queue& obj); // Prints queue details to std output
    friend bool save_curr_timeslots(const map<char, class QMS_Venue>&);
    friend bool load_curr_timeslots(map<char, class QMS_Venue>&, map<int, string>&);

    
};

class QMS_Venue { // temporary class
    map<string, Queue> timeslots;

public:
    QMS_Venue(); // default ctor
    QMS_Venue(map<string, Queue>& times); // non-default ctor
    ~QMS_Venue(); 

    void push_time(char, string);
    Queue& operator[](const string&);
    const Queue& operator[](const string&) const;
    map<string, Queue>& getTimeSlots();
    const map<string, Queue>& getTimeSlots() const; // gets all timeslots
};

// Function prototypes updated to QMS_Venue
bool save_curr_timeslots(const map<char, QMS_Venue>& venues);
bool load_curr_timeslots(map<char, QMS_Venue>& venues, map<int, string>& timeslots);
int QMSMenu(string IC, map<char, QMS_Venue>& venues, const map<int, string>& timeslots);

#endif