#include "Users.h"
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <iomanip>
using namespace std;

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
    cout << "1" << endl;
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
    dataObj.dummy_data();
    char lor;
    string log_in;
    
    while (true) {
        bool logout = 0;
        cout << endl;
        cout << "Enter (L) to login and (R) to register or (X) to exit program;" << endl;
        cin >> lor;
        if (lor == 'L' || lor == 'l') {
            log_in = dataObj.login();
            while (log_in != "") {
                char option;
                menu();
                cin >> option;
                cout << endl;
                switch (option) {
                    case 'D':
                    case 'd':
                        dataObj.delete_user(log_in);
                        break;

                    case 'L':
                    case 'l':
                        char confirmation;
                        cout << "Are you sure you would like to log out?" << endl;
                        cout << "Enter (Y) for yes or (N) for no: ";
                        cin >> confirmation;
                        if (confirmation == 'Y' || confirmation == 'y') {
                            cout << "Your have decided to log out" << endl;
                            log_in = "";
                        }
                        else if (confirmation == 'N' || confirmation == 'n') {
                            cout << "You have decided not to log out" << endl;
                        }
                        else {
                            cout << "Invalid input" << endl;
                        }
                        break;
                    default: 
                        cout << "Invalid input!" << endl;
                        break;
                }
            };
        }
        else if (lor == 'R' || lor == 'r') {
            dataObj.create_user();
        }
        else if (lor == 'X' || lor == 'x') {
            cout << "You have decided to exit the program. Bye!" << endl;
            dataObj.output_database();
            break;
        }
        else {
            cout << "Wrong user input. Please try again" << endl;
        }

    }
}
