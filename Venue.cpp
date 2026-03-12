#ifndef VENUE_H
#define VENUE_H

#include <string>
#include <thread> // For background processes
#include <chrono> // For handling time units
#include <vector>
#include <iostream>
#include <atomic> // Ensures multiple threads can safely update data

// Enumeration categorizes different locations A, B, and C
enum class LocationType { VENUE_A, VENUE_B, VENUE_C, Unknown};

class Venue {
private:
    std::string venueName;
    int waitTime;
    std::atomic<int> currentCapacity; // atomic prevents "Race conditions" where two threads try to change and update number of entries at the same time
    const int maxCapacity = 20; // 20 people max per venue
    LocationType selectedLocation;

public:
    //Constructor: Initialize the venues at 0 people
    Venue() : currentCapacity(0), waitTime(0), venueName("Unknown") {}

    void init(std::string name) // Setup function to name the venue and state the "natural exit" simulation
    {
        venueName = name;
        
        // Starts a background threat that runs "leaveVenue" independently
        std::thread exitThread(&Venue::leaveVenue, this); // Simulates people leaving the library naturally over time
        exitThread.detach(); // detach() lets it run without blocking the main menu code execution
    }
    
    void leaveVenue() // Backend logic: Every 30 seconds, 1 person leaves the venue automatically
    {
        while(true)
        {
            std::this_thread::sleep_for(std::chrono::seconds(30));
            if (currentCapacity > 0)
            {
                currentCapacity--; // Reduce count by 1
            }
        }
    }

    void displayStatus() const // Signboard Logic: Shows users the current status of the library
    {
        int nextQueue, estWait, waitlistcount;
        std::cout << venueName << std::endl;
        if (currentCapacity < maxCapacity) // Condition 1: There is space inside
        {
            std::cout << "Remaining Entry: " << (maxCapacity - currentCapacity) << std::endl;
        }
        else if (currentCapacity == maxCapacity) // Condition 2: Exactly 20 people; Next person will be the start of a queue
        {
            std::cout << "STATUS: FULL" << std::endl;
        }
        else //Condition 3: More than 20 people; calculate waitlist and time
        {
            waitlistcount = currentCapacity - maxCapacity;
            nextQueue = waitlistcount;
            estWait = nextQueue * 30; // Formula: (Number in line) * 30 seconds
            std::cout << "FULL (Waitlist: " << waitlistcount << ") | Est. Wait: " << estWait << "s" << std::endl;
        }
        std::cout << "-------------------------------" << std::endl;
    }

    void QueueTime() // Core logic for when a new user tries to enter
    {
        currentCapacity++; // Add the person to the venue count
        if (currentCapacity > maxCapacity) //If they are the 21st person or beyond, they must wait
        {
            int queuePos = currentCapacity - maxCapacity;
            waitTime = queuePos * 30; // 30 seconds per person

            std::cout << "\n" << venueName << " Queue #" << queuePos << std::endl;
            
            //Create a temporary "Timer thread" specifically for users from 21st onwards
            std::thread timerThread(&Venue::runCountdown, this, waitTime); // This allows the user to go back to the menu while they wait
            timerThread.detach();

        }
        else
        {
            std::cout << "Entry permitted. Please proceed to sign up." << std::endl;
        }
    }

    void runCountdown(int seconds) // Logic for the individual user's wait time
    {
        std::this_thread::sleep_for(std::chrono::seconds(seconds)); // Thread pauses here for the calculated duration
        currentCapacity--; // Once time is up, logically assume a person left and user waiting at front line can enter the venue
    }
};
#endif

int main (){
    Venue venues[3]; //Initialize an array of 3 venue objects
    venues[0].init("Venue A");
    venues[1].init("Venue B");
    venues[2].init("Venue C");
    
    int choice;
    while (true) // Main Program loop
    {
        std::cout << "\n===============================" << std::endl;
        std::cout << "         LIBRARY VENUE           " << std::endl;
        std::cout << "===============================" << std::endl;
        for (int i = 0; i < 3; i++) // Loop through the array to show all signboards at once
        {
            std::cout << (i + 1) << ". ";
            venues[i].displayStatus();
        }
        std::cout << "4. Exit System" << std::endl;
        std::cout << "Choose a venue to enter: ";
        std::cin >> choice;

        if (choice == 4) break; // Close the program
        if (choice >= 1 && choice <= 3) // Input validation: ensures user picks 1, 2, or 3
        {
            venues[choice-1].QueueTime(); // Trigger entry or queue logic
        }
        else
        {
            std::cout << "Invalid choice. Try again." << std::endl;
        }
    }
    return 0;
 }