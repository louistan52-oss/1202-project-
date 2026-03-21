#include "Users.h"
#include "User_data.h"
#include "BMS.h"
#include "QMS.h"
#include "Venue.h"
#include "Robot_Transport_System.h"
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <iomanip>
using namespace std;

//librarian and user class, database class
Books lib;
BookManagementUser session;

// Users class
Users::Users() {} // Default Constructor
Users::Users(string user_name, string user_NRIC, string user_email, string user_password) { // Constructor
    name = user_name;
    NRIC = user_NRIC;
    email = user_email;
    password = user_password;
}

void Users::set_name(string user_name) { // Set data member "name" of object
    name = user_name;
}

void Users::set_NRIC(string user_NRIC) { // Set data member "NRIC" of object
    NRIC = user_NRIC;
}

void Users::set_email(string user_email) { // Set data member "email" of object
    email = user_email;
}

void Users::set_password(string user_password) { // Set data member "password" of object
    password = user_password;
}

string Users::get_name() const{
    return name;
}

string Users::get_NRIC() const{
    return NRIC;
}

string Users::get_email() const{
    return email;
}

string Users::get_password() const{
    return password;
}

void Users::display_user() {
    cout << "Name: " << get_name() << endl;
    cout << "NRIC: " << get_NRIC() << endl;
    cout << "Email: " << get_email() << endl;
}

// Other functions
void menu() {
    cout << endl;
    cout << string(35, '=') << endl;
    cout << "Enter corresponding letter" << endl;
    cout << "(Q) to register a Queue" << endl;
    cout << "(A) to take your choosen books home" << endl;
    cout << "(D) to Delete account" << endl;
    cout << "(L) to Logout" << endl;
    cout << string(35, '=') << endl;
}

bool password_verifier(string user_password) {
    bool upper = false, lower = false, digit = false; 
    if (user_password.length() < 8) {
        cout << "Your password is invalid" << endl;
        cout << "Please ensure the password is at least 8 characters long" << endl;
        return false;
    }
    else {
        for (char c : user_password) {
            char check = static_cast<unsigned char>(c);
            if (isupper(check)) {
                upper = true;
            }
            else if (islower(check)) {
                lower = true;
            }
            else if (isdigit(check)) {
                digit = true;
            }
        }
        if (!(upper && lower && digit)) {
            cout << "Your password is invalid" << endl;
            cout << "Please ensure your password contains at least one number, lowercase letter and uppercase letter" << endl;
            return false;
        } 
        else {
            return true;
        }
    }
}

void program() {
    map<string, Users> user_database;
    map<char, QMS_Venue> venues;
    vector<string> all_NRIC;
    map<int, string> all_timeslots;
    User_data dataObj(user_database, all_NRIC);
    User_data libObj(user_database, all_NRIC);

    dataObj.input_database(); //Load from users.txt silently
    libObj.input_librarian(); //Load from librarian.txt silently
    if (!VenueOps::load(venues, all_timeslots)){ // Load Venues containing timeslots into map
        exit(0);
    }

    int choice;
    string log_in;
    bool Operation = true;
    while (Operation) {
        cout << "\n========== Book Giveaway Menu ==========" << endl;
        cout << "1. Login" << endl;
        cout << "2. Register" << endl;
        cout << "3. Exit" << endl;
        cout << "Selection: ";

        if (!(cin >> choice))
        {
            cin.clear();
            cin.ignore(1000, '\n');
            continue;
        }
        switch (choice){
            case 1:
                int roleChoice;
            cout << "\n--- Login Portal ---" << endl;
            cout << "1. Login as Librarian" << endl;
            cout << "2. Login as Visitor" << endl;
            cout << "Selection: ";
            cin >> roleChoice;

            cin.clear();
            cin.ignore(1000, '\n');
            switch (roleChoice){
                case 1:
                    log_in = libObj.login();
                    if (log_in == "T0123123F") // Libaraian login User ID
                    {
                        cout << "Welcome Librarian!" << endl;
                        lib.BMS(dataObj);
                    }
                    else if (log_in != "")
                    {
                        cout << "Reminder: You are trying to access a librarian account" << endl;
                    }
                    break;
                case 2:
                    log_in = dataObj.login(); //NRIC string
                    if (log_in != "")
                    {
                    int option;
                    cout << "\n--- Menu ---" << endl;
                    cout << "1. Enter booking reservation" << endl;
                    cout << "2. Delete account" << endl;
                    cout << "Selection: ";
                        while (true)
                        {
                            cin >> option;
                            if (option == 1 || option == 2) break;
                            cout << "Invalid Input. Please try again: ";
                            cin.clear();
                            cin.ignore(1000, '\n');
                        }
                        if (option == 1) // Redirect to Queue Management
                        {
                            bool has_book = true;
                            int QMS_choice;
                            cout << "Welcome Visitor! Redirecting to QMS..." << endl;
                            while (has_book)
                            {
                                QMS_choice = QMSMenu(log_in, venues, all_timeslots);
                                switch (QMS_choice)
                                {
                                case 2: session.startSession((venues.begin()->first)); break;
                                case 3: has_book = false; Operation = true; break;
                                case 4: has_book = false; Operation = true;
                                    break;                        
                                default:
                                    break;
                                }
                            }
                        }
                        else if (option == 2) // Account Deletion Logic
                        {
                            string current_user_nric = log_in; // Save the NRIC to a temp variable before it gets wiped
                            if(dataObj.delete_user(log_in))
                            {   // Only scrub queues and save files if user confirmed deletion ('Y')
                                leave_venue(current_user_nric, venues);
                                save_curr_timeslots(venues);
                                dataObj.output_database();
                                cout << "Account and bookings cleared." << endl;
                            }
                            else
                            {
                                cout << "Returning to menu..." << endl; // This runs if they typed 'N'
                            }
                        }
                    }
                    break;
                default:
                    cout << "invalid choice. try again: ";
                    cin >> roleChoice;
                    cin.clear();
                    cin.ignore(1000, '\n');
                    break;
            }
            break;
        case 2:  //Register user account
            dataObj.create_user();
            break;
        case 3:
            cout << "Saving data... Goodbye!" << endl;
            dataObj.output_database();
            VenueOps::removeFiles();
            VenueOps::save(venues);
            Operation = false;
            break;
        default:
            cout << "Invalid choice. Please pick 1-3!" << endl;
            break;
        }
    }
}