#pragma once
#ifndef QMS_H
#define QMS_H

#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <initializer_list>
#include <fstream>
#include <string>
#include <sstream>

class Venue;

class Queue{
private:
    size_t space;
    std::string time;
    char venue;
    std::set<std::string> queue;
public:
    Queue(); // default ctor

    Queue(const std::string& time); // non-default ctor w parameter time

    Queue(const char&, const std::string& time); // non-default ctor w parameter venue and time

    int addToQueue(std::string IC); // add to the queue depending on IC, returns the queue number, otherwise if no space returns 0

    void removeFromQueue(std::string IC);
        
    bool isFull() const; // if there is no space, return true;

    const std::set<std::string>& getQueue() const; // get all data in queue and return it

    friend std::ostream& operator<<(std::ostream& os, const Queue& queue); // operator overload for <<

    friend bool checkQueueStatus(std::string IC, const Queue& obj); // check if current IC is in queue
    
    friend void printQueueDetails(std::string IC, const Queue& obj); // Prints queue details to std output
    
    friend bool save_curr_timeslots(const std::map<char, Venue>&);
    
    friend bool load_curr_timeslots(std::map<char, Venue>&, std::map<int, std::string>&);
};

class Venue{ // temporary class
    private:
    // ...
    std::map<std::string, Queue> timeslots;
    public:
    Venue(); // default ctor

    Venue(std::map<std::string, Queue>& times); // non-default ctor

    ~Venue(); // dtor

    void push_time(char, std::string);

    Queue& operator[](const std::string&);

    const Queue& operator[](const std::string&) const;

    std::map<std::string, Queue>& getTimeSlots();
    
    const std::map<std::string, Queue>& getTimeSlots() const; // gets all timeslots
};

bool save_curr_timeslots(const std::map<char, Venue>& venues);

bool load_curr_timeslots(std::map<char, Venue>& venues, std::map<int, std::string>& timeslots);

bool QMSMenu(std::string, std::map<char, Venue>&, const std::map<int, std::string>&);

#endif