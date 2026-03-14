#include "Users.h"
#include "BMS.h"
#include "QMS_UPDATED.h"
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
Users::Users(string user_name, string user_NRIC, string user_email, string user_password) {
    name = user_name;
    NRIC = user_NRIC;
    email = user_email;
    password = user_password;
} // Constructor

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

string Users::get_name() {
    return name;
}

string Users::get_NRIC() {
    return NRIC;
}

string Users::get_email() {
    return email;
}

string Users::get_password() {
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
    cout << "(A) to register a Queue" << endl;
    cout << "2" << endl;
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
    vector<string> all_NRIC;
    User_data dataObj(user_database, all_NRIC);

    dataObj.input_database(); //Load from users.txt silently

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
            
            if (roleChoice == 1)
            {
                log_in = dataObj.login();
                if (log_in == "T0333333A") // Libaraian login
                {
                    cout << "Welcom Librarian!" << endl;
                    lib.BMS();
                }
                else if (log_in != "")
                {
                    cout << "Access Denied: You do not have Librarian priviledges." << endl;
                }
            }

            else if (roleChoice == 2) // Visitor login
            {
                log_in = dataObj.login();
                if (log_in != "")
                {
                    cout << "Welcome Visitor! Redirecting to QMS..." << endl;
                    QueueUser(); // Redirect to Queue Management
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
            break;
        }
        else
        {
            cout << "Invalid choice. Please pick 1-3!" << endl;
        }
    }
}
