#include "QMS.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

// --- Queue Member Functions ---
Queue::Queue() : space{10}, time{""}, venue{' '}, queue{} {} 
Queue::Queue(const string& time) : space{10}, time{time}, venue{' '}, queue{} {} 
Queue::Queue(const char& v, const string& time) : space{10}, time{time}, venue{v}, queue{} {}

int Queue::addToQueue(string IC){ 
    if (queue.count(IC)){
        cout << "You are already registered!" << endl;
        return 0;
    }
    if (!isFull()) {
        space--;
        queue.insert(IC);
        return 10 - space;
    }
    cout << "There is no more space!" << endl;
    return 0;
} 

void Queue::removeFromQueue(string IC){ queue.erase(IC); }
bool Queue::isFull() const { return space == 0; } 
const set<string>& Queue::getQueue() const { return queue; }
ostream& operator<<(ostream& os, const Queue& queue) { 
    os << queue.time; 
    return os;
}

bool checkQueueStatus(string IC, const Queue& obj){ // checks IC against queue, returns if in queue or not
    return (obj.queue.find(IC) != obj.queue.end());
}
void printQueueDetails(string IC, const Queue& obj) {
    cout <<"\nQueue Details:" << endl;
    for (const auto& e: obj.queue){
        if (e == IC)
        {
            cout << "IC: " << IC << " | Venue: " << obj.getVenue() << " | Time: " << obj.getTime() << endl;
        }
    }
}



//********************************************************* */
//
//          Venue Temporary Class Implementation
//
//********************************************************* */

QMS_Venue::QMS_Venue() {} 
QMS_Venue::QMS_Venue(map<string, Queue>& times) {
    for (const auto& [key, val] : times) {
        timeslots.insert({key, val});
    }
}
QMS_Venue::~QMS_Venue() {} 
map<string, Queue>& QMS_Venue::getTimeSlots() { return timeslots; }
const map<string, Queue>& QMS_Venue::getTimeSlots() const { return timeslots; }
void QMS_Venue::push_time(char v, string time) { timeslots.insert({time, Queue{v, time}}); }
Queue& QMS_Venue::operator[](const string& index) { return timeslots[index]; }
const Queue& QMS_Venue::operator[](const string& index) const { return timeslots.at(index); }

//********************************************************* */
//
//                  Helper Functions
//
//********************************************************* */

bool isValidVenue(char val) {
    val = toupper(val);
    if (val >= 'A' && val <= 'C'){ return true;}
    return false;
}

bool isValidTime(string val, const map<int, string>& timeslots) {
    if (timeslots.count(stoi(val))) { return true; }
    if (val.length() != 4) return false;
    if (val.find_first_not_of("0123456789") != string::npos) return false;
    return true; 
}

bool isValidIC(string nric)
{
    if (nric.length() < 9)
    {
        return false;
    }
    return true; // Whatever else IC needs
}

//********************************************************* */
//
//                  Extra Functions
//
//********************************************************* */

char venueSelection() {
    char choice;
    cout << "Select a venue (A, B, or C): " << endl;
    if (cin.peek() == '\n') cin.ignore();
    while (!(cin >> choice) || !isValidVenue(choice)) {
        cout << "Please enter a valid input" << endl;
        cin.clear();
        cin.ignore(1000, '\n');
    }
    return toupper(choice);
}

string timeSelection(const QMS_Venue& venue, const map<int, string>& timeslots) {
    cout << "Select a timeslots (Limited to 10 reservation per time slot): " << endl;
    int numTimeSlots{1}; // for display purposes
    vector<string> open_timeslots; // vector to store indexes of available timeslots
    for (const auto& [time, queue] : venue.getTimeSlots()) {
        if (!queue.isFull())
        {
            cout << numTimeSlots++ << ". " << time << endl; // Display available timeslots
            open_timeslots.push_back(time); // Save this time string at this position
        }
    }
    cout << "Selection: ";
    string choice;
    while (true) {
        cin >> choice;
        // Check if user typed a valid number from the list (1, 2, 3...)
        try {
            int idx = stoi(choice);
            if (idx >= 1 && idx <= open_timeslots.size()) {
                return open_timeslots[idx - 1]; // Return the actual time (e.g., "1000")
            }
        } catch (...) {}

        // Or check if they typed the 4-digit time directly (e.g., "1000")
        if (find(open_timeslots.begin(), open_timeslots.end(), choice) != open_timeslots.end()) {
            return choice;
        }

        cout << "Please enter a valid number from the list: ";
        cin.clear();
        cin.ignore(1000, '\n');
    }
}

bool save_curr_timeslots(const map<char, QMS_Venue>& venues) {
    ofstream file{"timeslots.txt"};
    if (!file)
    {
        cout << "Unable to write to timeslots-output.txt\n Closing Program...\n";
        return false;
    } 

    for (const auto& [name, venue] : venues) {
        file << "venue = " << name << endl << endl;
        for (const auto& [time, queue] : venue.getTimeSlots()) {
            file << "time = " << time << endl;
            for (const string& ic : queue.getQueue()) {
                file << ic << endl;
            }
            file << endl;
        }
    }
    return true;
}

bool load_curr_timeslots(map<char, QMS_Venue>& venues, map<int, string>& timeslots) {
    ifstream file{"timeslots.txt"};
    if (!file) {
        cout << "Error: Could not find timeslots.txt in the project directory." << endl;
        return false;
    } 

    string line;
    char current_v = '\0';
    string current_time = "";

    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss{line};
        string firstWord;
        ss >> firstWord;

        if (firstWord == "venue") {
            string eq;
            ss >> eq >> current_v; 
            if (venues.find(current_v) == venues.end()) {
                venues[current_v] = QMS_Venue();
            }
        }
        else if (firstWord == "time") {
            string eq, val;
            ss >> eq >> val;
            current_time = val;
            venues[current_v].push_time(current_v, current_time);

            // Add to the global timeslots map if not present
            bool exists = false;
            for (auto const& [index, t] : timeslots) {
                if (t == current_time) { exists = true; break; }
            }
            if (!exists) {
                int nextIndex = timeslots.size() + 1;
                timeslots[nextIndex] = current_time;
            }
        }
        else {
            // It's an NRIC
            if (current_v != '\0' && !current_time.empty()) {
                venues[current_v][current_time].addToQueue(firstWord);
            }
        }
    }
    return true;
}

bool check_booking(string IC, map<char, QMS_Venue>& venues) {
    for (auto& [v, venue] : venues){
        for (auto& [time, queue] : venue.getTimeSlots()){
            if (queue.getQueue().count(IC)){
                cout << "You already have a booking under:\nVenue: " << v << " Time: " << time << endl;
                char choice;
                cout << "Would you like to cancel your booking? (Y/N): ";
                cin >> choice;
                if (choice == 'Y' || choice == 'y'){
                    queue.removeFromQueue(IC);
                    cout << "Cancellation successful." << endl;
                }
                return true;
            }
        }
    }
    return false;
}

bool enter_venue(string IC, map<char, QMS_Venue>& venues, char gantry_venue, string gantry_time) {
    for (auto& [v, venue] : venues){
        if (v != gantry_venue) continue; // Skip venues that don't match the gantry
        for (auto& [time, queue] : venue.getTimeSlots()){
            if (v == gantry_venue && time == gantry_time && queue.getQueue().count(IC)){
                cout << "\nYou are entering:\nVenue: " << v << " Time: " << time << endl;
                return true;
            }
        }
    }
    return false;
}

bool leave_venue(string IC, map<char, QMS_Venue>& venues) {
    for (auto& [v, venue] : venues){
        for (auto& [time, queue] : venue.getTimeSlots()){
            if (queue.getQueue().count(IC)){
                queue.removeFromQueue(IC);
                return true;
            }
        }
    }
    return false;
}

void PressEnterToContinue()
{
    cout << "\nPress Enter to continue...";
    cin.ignore(1000, '\n'); // Clear the buffer
    cin.get();              // Wait for user to press "Enter"
}

int QMSMenu(string IC, map<char, QMS_Venue>& venues, const map<int, string>& timeslots) {
    bool to_stop = false;
    static bool entered = false; // Changing this to static makes the function "remember" the value between different calls.
    int choice;
    while (!to_stop){
        cout << "\033[2J\033[3J\033[H" << flush; // Clear screen
        cout << "--- Queue Manager System ---" << endl;
        cout << "1. Book/Cancel Timeslot (Testcase Purposes: Book Venue A, 1500 to enter venue)" << endl;
        cout << "2. Enter Venue" << endl;
        cout << "3. Leave Venue" << endl;
        cout << "4. Quit QMS" << endl;
        cout << "Selection: ";
        cin >> choice;
        cout << "\033[2J\033[3J\033[H" << flush; // Clear screen
        if (choice == 1){

            if (!check_booking(IC, venues)){
                char venue_sel = venueSelection(); // A, B or C
                cout << "\033[2J\033[3J\033[H" << flush; // Clear screen
                string time_sel = timeSelection(venues[venue_sel], timeslots); // 0900, 1000, 1100, etc.

                venues[venue_sel][time_sel].addToQueue(IC);

                printQueueDetails(IC, venues[venue_sel][time_sel]);
                PressEnterToContinue();
            }
            else {
                continue;
            }  
        }

        else if (choice == 2)
        {
            if (entered) // LAYER 1: Check if the person is physically already inside
            {
                cout << ">> Access Denied: You are already inside the venue. <<" << endl;
                PressEnterToContinue();
                continue; // Go back to the menu selection
            }
            
            if (enter_venue(IC, venues, 'A', "1500"))
            {
                cout << "You may proceed into the venue" << endl << endl;
                PressEnterToContinue();
                entered = true;
                to_stop = true;
            }
            else
            {   // This only triggers if they are NOT inside AND have NO booking
                cout << "Access Denied: No valid reservation found for this time slot." << endl;
                PressEnterToContinue();
            }
        }

        else if (choice == 3){
            if (!entered){
                cout << "You cannot leave what you haven't entered" << endl;
                cout << "\nPress Enter to continue...";
                cin.ignore(1000, '\n'); // Clear the buffer
                cin.get();              // Wait for user to press "Enter"
            }
            else{
                leave_venue (IC, venues);
                cout << "You have left the venue. GOODBYE" << endl;
                PressEnterToContinue();
                to_stop = true;
            }
        }
        else if (choice == 4){
            to_stop = true;
            cout << "Program closing..." << endl;
            PressEnterToContinue();
        }
        else{
            cout << "Invalid choice!" << endl;
            PressEnterToContinue();
        }
    }
    return choice;
}
