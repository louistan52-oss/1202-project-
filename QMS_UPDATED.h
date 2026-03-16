#include <iostream>
#include <map>
#include <vector>
#include <initializer_list>

class Queue{
    private:
        int space;
        int venue_id;
        std::string time;
        std::map<std::string, int> queue;
    public:

        Queue() : space{10}, venue_id{}, time{}, queue{}{} // default ctor
        Queue(std::string time) : space{10}, venue_id{}, time{time}{} // non-default ctor w parameter time

        int addToQueue(std::string IC){ // add to the queue depending on IC, returns the queue number, otherwise if no space returns 0
            if (queue.count(IC)){
                std::cout << "You are already registered!" << std::endl;
                return 0;
            }
            if (space-- > 0) {
                queue.insert({IC, 10-space});
                std::cout << "IC: " << IC << " | Timeslot: " << time << " | Venue: " << venue_id << std::endl;
                std::cout << "Space Left: " << space << std::endl;
                return 10-space;
            }
            std::cout << "There is no more space!" << std::endl;
            return 0;
        } 
        bool isFull() const { return space == 0; } // if there is no space, return true;

        std::vector<Queue> getQueue(){
            std::vector<Queue> ret_queue;
            for (const auto& element : queue){
                ret_queue.push_back(element.first);
            }
            return ret_queue;
        }

        friend std::ostream& operator<<(std::ostream& os, const Queue& queue){
            os << queue.time;
            return os;
        }

        friend bool checkQueueStatus(std::string IC, const Queue& obj){ // checks IC against queue, returns if in queue or not
            for (const auto& element : obj.queue){
                if (element.first == IC){
                    return true;
                }
            }
            return false;
        }
        
        friend void printQueueDetails(const Queue& obj){
            std::cout << "Queue Details:" << std::endl;
            for (const auto& e : obj.queue){
                std::cout << e.second << ". " << e.first << std::endl;
            }
        }
};

class Venue{ // temporary class
    private:
    // ...
    std::vector<Queue> timeslots;
    public:
    Venue(){} // default ctor
    Venue(std::vector<Queue>& times){ // non-default copy ctor
        for (Queue e : times){
            timeslots.push_back(e);
        }
    }
    ~Venue(){} // dtor
    std::vector<Queue>& getTimeSlots(){
    return timeslots;
    }
    const std::vector<Queue>& getTimeSlots() const{ // gets all timeslots
        return timeslots;
    }
};

void leaveVenue(std::string IC, const Queue& queue){
    //check IC vector, if it matches, it will then check if they do want to take a book, (no book taken) allow user to leave, space++
    //(book taken) go to book giveaway function, ensure function success, allow user to leave, space++ 
    
    if (checkQueueStatus(IC, queue)){
        std::cout << "You are free to leave" << std::endl;
    }
    else{
        std::cout << "Please approach a librarian for assistance" << std::endl;
    }
}

bool isValidVenue(char val){
    if (val >= 'a' && val <='c'){ return true; }
    return false;
}

bool isValidTime(char val){
    if (isdigit(val)) {return true;}
    return false;
}
bool isValidIC(std::string nric){
    if (nric.length() < 9){
        return false;
    }
    // whatever else IC needs
    return true;
}
char venueSelection(){
    char choice;
    std::cout << "Select a venue: A, B or C"<< std::endl;
    
    while(!(std::cin >> choice) || !isValidVenue(choice)){
        choice = toupper(choice); 
        std::cout << "Please enter a valid input" << std::endl;
        std::cin.clear();
        std::cin.ignore(1000, '\n');
    }
    return choice;
}
char timeSelection(const std::vector<Queue>& all_timeslots){

    std::cout << "Select a timeslot: " << std::endl;

    int numTimeSlots{1}; // for display purposes
    int numIndex{};
    std::vector<int> open_timeslots; // vector to store indexes of available timeslots

    for (const Queue& time : all_timeslots){
        if (!time.isFull()) {
            std::cout << numTimeSlots++ << ". " << time << std::endl; // display available timeslots
            open_timeslots.push_back(numIndex); // push_back the index of the available timeslot
        }
        ++numIndex;
    }
    
    char choice_two;
    while(!(std::cin >> choice_two) || !isValidTime(choice_two)){
        std::cout << "Please enter a valid input" << std::endl;
        std::cin.clear();
        std::cin.ignore(1000, '\n');
    }
    return choice_two;
}

int QueueUser(){
    // main application should give User IC
    std::string IC;
    Queue first_time{"13:30"};
    Queue second_time{"14:30"};
    Queue third_time{"15:30"};

    std::vector<Queue> timeslots{first_time, second_time, third_time};

    Venue A{timeslots}; 
    Venue B{timeslots};
    Venue C{timeslots};

    //std::vector<Venue> venues{A,B,C};
    std::map<char, Venue> venues{{'A', A}, {'B', B}, {'C', C}};

    bool to_stop{};

    while (!to_stop){
        
        do{
            std::cout << "Enter IC: ";
            std::cin >> IC;
            std::cout << std::endl;
        } while(!isValidIC(IC));

        char choice = venueSelection();

        //std::cout << "Choice: " << choice << std::endl;

        Venue& selected_Venue{venues[choice]}; // a reference/alias of the selected venue

        std::vector<Queue>& all_timeslots{selected_Venue.getTimeSlots()};

        // for (const Queue& e : all_timeslots){
        //     std::cout << e << ' ';
        // }

        char time_chosen{timeSelection(all_timeslots)};
        time_chosen-='0'; // converts from ASCII value of number to int number

        Queue& selected_time{all_timeslots[time_chosen-1]}; // selects timeslot within venue
        // std::cout << "Time Chosen: " << int(time_chosen) << std::endl;
        // std::cout << "Time Chosen: " << all_timeslots[time_chosen].isFull() << std::endl;
        // std::cout << "Is Empty: " << all_timeslots. << std::endl;

        selected_time.addToQueue(IC);

        checkQueueStatus(IC, selected_time);

        leaveVenue(IC, selected_time);

        // std::vector<Queue> temp_queue = selected_time.getQueue();

        // for (auto& e : temp_queue){
        //     std::cout << e << ' ';
        // } 

        // std::cout << std::endl;

        printQueueDetails(selected_time);

        char val{};
        std::cout << "Do you want to continue? ";
        std::cin >> val;
        if (val == 'N'){
            to_stop = true;
        }
    }
    return 0;
}
