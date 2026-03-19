#include "QMS.h"

//********************************************************* */
//
//              Queue Class Implementation
//
//********************************************************* */

Queue::Queue() : space{10}, time{}, venue{}, queue{}{} // default ctor

Queue::Queue(const std::string& time) : space{10}, time{time}, venue{}, queue{}{} // non-default ctor w parameter time

Queue::Queue(const char& v, const std::string& time) : space{10}, time{time}, venue{v}, queue{}{} // non-default ctor

int Queue::addToQueue(std::string IC){ // add to the queue depending on IC, returns the queue number, otherwise if no space returns 0
    if (queue.count(IC)){
        std::cout << "You are already registered!" << std::endl;
        return 0;
    }
    if (!isFull()) {
        space--;
        queue.insert(IC);
        return 10-space;
    }
    std::cout << "There is no more space!" << std::endl;
    return 0;
} 

void Queue::removeFromQueue(std::string IC){
    queue.erase(IC);
}

bool Queue::isFull() const { return space == 0; } // if there is no space, return true;

const std::set<std::string>& Queue::getQueue() const{
    return queue;
}

std::ostream& operator<<(std::ostream& os, const Queue& queue){
    os << queue.time;
    return os;
}

bool checkQueueStatus(std::string IC, const Queue& obj){ // checks IC against queue, returns if in queue or not
    return (obj.queue.find(IC) != obj.queue.end());
}

void printQueueDetails(std::string IC, const Queue& obj){
    std::cout << "Queue Details:" << std::endl;
    for (const auto& e : obj.queue){
        if (e == IC)
        {
            std::cout << "IC: " << e << " Venue: " << obj.venue << " Time: " << obj.time << std::endl;
        }
    }
}

//********************************************************* */
//
//          Venue Temporary Class Implementation
//
//********************************************************* */

Venue::Venue(){} // default ctor
Venue::Venue(std::map<std::string, Queue>& times){ // non-default copy ctor
    for (const auto& [key, val] : times){
        timeslots.insert({key, val});
    }
}
Venue::~Venue(){} // dtor
std::map<std::string, Queue>& Venue::getTimeSlots(){
    return timeslots;
}
const std::map<std::string, Queue>& Venue::getTimeSlots() const{ // gets all timeslots
    return timeslots;
}

void Venue::push_time(char v, std::string time){
    timeslots.insert({time, Queue{v, time}});
}

Queue& Venue::operator[](const std::string& index){
    return timeslots[index];
}

const Queue& Venue::operator[](const std::string& index) const{
    return timeslots.at(index);
}
//********************************************************* */
//
//                  Helper Functions
//
//********************************************************* */

bool isValidVenue(char val){
    val = toupper(val);
    if (val >= 'A' && val <='C'){ return true; }
    return false;
}

bool isValidTime(std::string val, const std::map<int, std::string>& timeslots){
    if (timeslots.count(std::stoi(val))) { return true; }
    if (val.length() != 4) return false;
    if (val.find_first_not_of("0123456789") != std::string::npos) return false;
    
    return true;
}
bool isValidIC(std::string nric){
    if (nric.length() < 9){
        return false;
    }
    // whatever else IC needs
    return true;
}

//********************************************************* */
//
//                  Extra Functions
//
//********************************************************* */
char venueSelection(){
    char choice;
    std::cout << "Select a venue: A, B or C"<< std::endl;
    while(!(std::cin >> choice) || !isValidVenue(choice)){
        std::cout << "Please enter a valid input" << std::endl;
        std::cin.clear();
        std::cin.ignore(1000, '\n');
    }
    return choice;
}
std::string timeSelection(const Venue& venue, const std::map<int, std::string>& timeslots){

    std::cout << "Select a timeslot: " << std::endl;

    int numTimeSlots{1}; // for display purposes
    int numIndex{};
    std::vector<int> open_timeslots; // vector to store indexes of available timeslots

    for (const auto& [time, queue] : venue.getTimeSlots()){
        if (!queue.isFull()) {
            std::cout << numTimeSlots++ << ". " << time << std::endl; // display available timeslots
            open_timeslots.push_back(numIndex); // push_back the index of the available timeslot
        }
        ++numIndex;
    }
    std::cout << "Selection (e.g 0900): ";
    std::string choice;
    while(!(std::cin >> choice) || !isValidTime(choice, timeslots)){
        std::cout << "Please enter a valid input" << std::endl;
        std::cin.clear();
        std::cin.ignore(1000, '\n');
    }
    return choice;
}

bool save_curr_timeslots(const std::map<char, Venue>& venues){
    std::ofstream file{"timeslots.txt"};
    if(!file){
        std::cout << "Unable to write to timeslots-output.txt\n Closing Program...\n";
        return false;
    }

    for (const auto& [name, venue] : venues){
        file << "venue = " << name << std::endl << std::endl;
        for (const auto& [time, queue] : venue.getTimeSlots()){
            file << "time = " << time << std::endl;
            for (const std::string& ic : queue.getQueue()){
                file << ic << std::endl;
            }
            file << std::endl;
        }
    }
    return true;
}

bool load_curr_timeslots(std::map<char, Venue>& venues, std::map<int, std::string>& timeslots){
    std::ifstream file{"timeslots.txt"};
    if(!file){
        std::cout << "Unable to read timeslots.txt\n Closing Program...\n";
        return false;
    }

    std::string line;
    char v{};
    std::string time{};
    while (std::getline(file, line)){
        if (line == ""){
            continue;
        }
        std::stringstream ss{line};
        std::string q_choice{};
        std::string tmp{};
        if (ss >> q_choice){
            if (q_choice == "venue"){
                ss >> tmp >> v;
                venues.insert({v, Venue{}});
            }
            else if (q_choice == "time"){
                ss >> tmp >> time;
                venues[v].push_time(v, time);
                bool is_in{false};
                for (const auto& [index, t] : timeslots){
                    if (t == time) { is_in = true; break;}
                }
                if (!is_in){
                    timeslots[timeslots.size()+1] = time;
                }
            }
            else {
                venues[v][time].addToQueue(q_choice);
            }
        }
    }
    return true;
}

bool check_booking(std::string IC, std::map<char, Venue>& venues){
    for (auto& [v, venue] : venues){
        for (auto& [time, queue] : venue.getTimeSlots()){
            if (queue.getQueue().count(IC)){
                std::cout << "You already have a booking under:\nVenue: " << v << " Time: " << time << std::endl;
                char choice;
                std::cout << "Would you like to cancel your booking?: ";
                std::cin >> choice;
                if (choice == 'Y' || choice == 'y'){
                    queue.removeFromQueue(IC);
                    std::cout << "Cancellation successful." << std::endl;
                }
                return true;
            }
        }
    }
    return false;
}

bool enter_venue(std::string IC, std::map<char, Venue>& venues, char gantry_venue, std::string gantry_time){
    for (auto& [v, venue] : venues){
        for (auto& [time, queue] : venue.getTimeSlots()){
            if (v == gantry_venue && time == gantry_time && queue.getQueue().count(IC)){
                std::cout << "You are entering:\nVenue: " << v << " Time: " << time << std::endl;
                return true;
            }
        }
    }
    return false;
}

bool leave_venue(std::string IC, std::map<char, Venue>& venues){
    for (auto& [v, venue] : venues){
        for (auto& [time, queue] : venue.getTimeSlots()){
            if (queue.getQueue().count(IC)){
                queue.removeFromQueue(IC);
                std::cout << "Do you have any books? (Y/N)\n";
                return true;
            }
        }
    }
    return false;
}

bool QMSMenu(std::string IC, std::map<char, Venue>& venues, const std::map<int, std::string>& timeslots){
    bool to_stop{};
    bool entered{};

    while (!to_stop){

        char choice;

        std::cout << "\n--- Queue Manager System ---" << std::endl;
        std::cout << "1. Book/Cancel Timeslot" << std::endl;
        std::cout << "2. Enter Venue" << std::endl;
        std::cout << "3. Leave Venue" << std::endl;
        std::cout << "Q. Quit QMS" << std::endl;
        std::cout << "Selection: ";
        std::cin >> choice;

        if (choice == '1'){

            if (!check_booking(IC, venues)){
                char venue_sel = venueSelection(); // A, B or C

                std::string time_sel = timeSelection(venues[venue_sel], timeslots); // 0900, 1000, 1100, etc.

                if (timeslots.count(std::stoi(time_sel))) time_sel = timeslots.at(std::stoi(time_sel));

                venues[venue_sel][time_sel].addToQueue(IC);

                printQueueDetails(IC, venues[venue_sel][time_sel]);
            }
            else {
                continue;
            }  
        }

        else if (choice == '2'){
            if (enter_venue(IC, venues, 'A', "1500") && !entered){
                std::cout << "You may proceed into the venue" << std::endl;
                entered = true;
            }
            else if(entered){
                std::cout << "You have already entered the library" << std::endl;
            }
            else {
                std::cout << "Check failed. Talk to librarian" << std::endl;
            }
        }

        else if (choice == '3'){
            if (!entered){
                std::cout << "You cannot leave what you haven't entered" << std::endl;
            }
            else{
                leave_venue(IC, venues);
                std::string have_book;
                std::cin >> have_book;
                if (have_book == "Y" || have_book == "y") {
                    std::cout << "You have left the venue. GOODBYE\n";
                    return true;
                };
                entered = false;
            }
        }
        else if (choice == 'Q' || choice == 'q'){
            to_stop = true;
            std::cout << "Program closing..." << std::endl;
        }
        else{
            std::cout << "Choose again: ";
        }
    }
    return false;
}
