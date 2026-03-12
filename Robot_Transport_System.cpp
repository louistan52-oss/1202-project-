#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <mutex>
#include <atomic>

using namespace std;
using namespace std::chrono_literals;

// Global control flag: When false, all background threads will stop their loops
atomic<bool> systemRunning(true);

// Enum class for strong-typed states. Prevents accidental math on states.
enum class State { IDLE, WORKING, CHARGING, REFILLING };

class Robot {
private:
    string name;
    int battery;
    int inventory = 5;
    State status = State::IDLE;
    
    // Mutex (Mutual Exclusion) ensures only one thread accesses the robot's data at a time.
    // This prevents "Data Races" where the display thread reads while the work thread writes.
    mutable mutex mtx;

public:
    // Constructor initializes the robot with a name and a random battery level
    Robot(string n) : name(n), battery(rand() % 31 + 60) {}

    // Required for the Venue vector. Mutexes cannot be copied, so we move the robot data instead.
    Robot(Robot&& other) noexcept {
        lock_guard<mutex> lock(other.mtx);
        name = move(other.name);
        battery = other.battery;
        inventory = other.inventory;
        status = other.status;
    }

    string getName() const { return name; }

    // Converts the Enum State into a human-readable string with ANSI colors
    string getStatusString() const {
        switch (status) {
            case State::WORKING:  return "\033[1;33mWORKING\033[0m";   // Yellow
            case State::CHARGING: return "\033[1;36mCHARGING\033[0m";  // Cyan
            case State::REFILLING: return "\033[1;35mREFILLING\033[0m"; // Magenta
            default:               return "\033[1;32mIDLE\033[0m";      // Green
        }
    }

    // Task: Drains battery/inventory. If limits hit, triggers next task thread.
    void startWorking() {
        {
            lock_guard<mutex> lock(mtx);
            status = State::WORKING;
        }
        
        // Launch background thread
        thread t([this]() {
            while (systemRunning) {
                this_thread::sleep_for(1s); // Simulate time taken to shelf a book
                
                lock_guard<mutex> lock(mtx);
                battery -= 3;
                inventory -= 1;

                // Threshold Check: Need Charge?
                if (battery <= 20) {
                    status = State::CHARGING;
                    thread(&Robot::startCharging, this).detach();
                    break; // Exit this working thread
                }
                
                // Threshold Check: Need Books?
                if (inventory <= 0) {
                    status = State::REFILLING;
                    thread(&Robot::startRefilling, this).detach();
                    break; // Exit this working thread
                }
            }
        });
        t.detach(); // Let the thread run independently
    }

    // Background task: Increases battery to 100%
    void startCharging() {
        while (systemRunning) {
            this_thread::sleep_for(500ms);
            lock_guard<mutex> lock(mtx);
            battery += 5;
            if (battery >= 100) {
                battery = 100;
                status = State::IDLE;
                break; // Charging complete
            }
        }
    }

    // Background task: Simple delay to simulate manual book reloading
    void startRefilling() {
        this_thread::sleep_for(3s); 
        if (systemRunning) {
            lock_guard<mutex> lock(mtx);
            inventory = 5;
            status = State::IDLE;
        }
    }

    // Formatted output of the robot's current stats
    void display() const {
        lock_guard<mutex> lock(mtx);
        cout << left << setw(10) << name 
             << " | Bat: " << setw(3) << battery << "%"
             << " | Inv: " << inventory << "/5"
             << " | Status: " << getStatusString() << endl;
    }

    State getStatus() const { lock_guard<mutex> lock(mtx); return status; }
};

class Venue {
public:
    int id;
    vector<Robot> robots;

    Venue(int vid) : id(vid) {
        // Build robots directly in the vector to avoid move/copy overhead
        robots.emplace_back("Alpha");
        robots.emplace_back("Bravo");
        robots.emplace_back("Charlie");
    }

    // Core logic: Displays current robots and assigns work if the venue is neglected
    void refreshVenue() {
        cout << "\n--- VENUE " << id << " LIVE DASHBOARD ---" << endl;
        bool anyoneWorking = false;

        for (auto& r : robots) {
            r.display();
            if (r.getStatus() == State::WORKING) anyoneWorking = true;
        }

        // Automatic Manager: If no robot is working, find an idle one to deploy
        if (!anyoneWorking) {
            for (auto& r : robots) {
                if (r.getStatus() == State::IDLE) {
                    cout << ">> Assigning " << r.getName() << " to restocking duty..." << endl;
                    r.startWorking();
                    break; // Only deploy one robot
                }
            }
        }
    }
};

int main() {
    srand(static_cast<unsigned int>(time(0)));
    
    // Create our 3 venues
    vector<Venue> fleet;
    fleet.emplace_back(1);
    fleet.emplace_back(2);
    fleet.emplace_back(3);

    int choice;
    while (true) {
        cout << "\nEnter Venue (1-3) or 0 to Exit: ";
        if (!(cin >> choice)) {
            cin.clear(); // Clear error flag
            cin.ignore(1000, '\n'); // Discard bad input
            continue;
        }
        
        if (choice == 0) break;
        if (choice >= 1 && choice <= 3) {
            fleet[choice - 1].refreshVenue();
        }
    }

    // Graceful Shutdown
    systemRunning = false; 
    cout << "Stopping background systems... Goodbye." << endl;
    this_thread::sleep_for(500ms); // Brief pause to allow threads to close
    
    return 0;
}
