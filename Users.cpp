#include "Users.h"
#include "User_data.h"
#include "BMS.h"
#include "QMS.h"
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

int removeFile(){
    bool status=1;
    while(status){
    status = remove("vA_shelves.txt");
    status = remove("vB_shelves.txt");
    status = remove("vC_shelves.txt");
    status = remove("vA_stock.txt");
    status = remove("vB_stock.txt");
    status = remove("vC_stock.txt");
    }
    if (status!=0) perror("error deleting file");
    return 0;
}

void program() {
    map<string, Users> user_database;
    map<char, Venue> venues;
    vector<string> all_NRIC;
    map<int, string> all_timeslots;
    User_data dataObj(user_database, all_NRIC);

    dataObj.input_database(); //Load from users.txt silently
    dataObj.create_librarian(); // Implement Libarian data into map
    if (!load_curr_timeslots(venues, all_timeslots)){ // Load Venues containing timeslots into map
        exit(0);
    }

    int choice;
    string log_in;

    while (true) {
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

        if (choice == 1) // Login
        {
            int roleChoice;
            cout << "\n--- Login Portal ---" << endl;
            cout << "1. Login as Librarian" << endl;
            cout << "2. Login as Visitor" << endl;
            cout << "Selection: ";
            cin >> roleChoice;

            cin.clear();
            cin.ignore(1000, '\n');
            
            if (roleChoice == 1)
            {
                log_in = dataObj.login();
                if (log_in == "T0123123F") // Libaraian login password L1brarian
                {
                    cout << "Welcome Librarian!" << endl;
                    lib.BMS();
                }
                else if (log_in != "")
                {
                    cout << "Access Denied: You do not have Librarian priviledges." << endl;
                }
            }

            else if (roleChoice == 2) // Visitor login
            {
                log_in = dataObj.login(); //NRIC string
                if (log_in != "")
                {
                    bool has_book;
                    cout << "Welcome Visitor! Redirecting to QMS..." << endl;
                    do{
                        has_book = QMSMenu(log_in, venues, all_timeslots); // returns whether user has books to take 
                        session.startSession((venues.begin()->first));
                    }while(has_book);
                }
            }
        }
        
        else if (choice == 2) //Register user account
        {
            dataObj.create_user();
        }
        else if (choice == 3)
        {
            cout << "Saving data... Goodbye!" << endl;
            dataObj.output_database();
            removeFile();
            save_curr_timeslots(venues);
            break;
        }
        else
        {
            cout << "Invalid choice. Please pick 1-3!" << endl;
        }
    }
}