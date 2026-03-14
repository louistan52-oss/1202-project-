#include "Users.h"
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <iomanip>
using namespace std;

// User_data class
User_data::User_data() {}
User_data::User_data(map<string, Users> user_database, vector<string> all_NRIC) {}

void User_data::create_user() {
    string NRIC, name, email, password, username;
    cout << endl;
    do {
        cout << "Enter NRIC: ";
        cin >> NRIC;
        if (user_database.count(NRIC)) {
            cout << "NRIC already in use. Please try again" << endl;
        }
    }
    while (user_database.count(NRIC));
    cout << "Enter name: ";
    cin >> name;
    cout << "Enter email: ";
    cin >> email;
    do {
        cout << "Enter a password: ";
        cin >> password;
    }
    while (!password_verifier(password));
    username = NRIC;
    Users tempObj{name, NRIC, email, password};
    user_database.insert({username, tempObj});
    all_NRIC.push_back(NRIC);
}

Users User_data::retrieve_user() {
    string username;
    cout << "Enter username: ";
    cin >> username;
    Users tempObj = user_database.at(username);
    tempObj.display_user();
    return tempObj;
}

void User_data::delete_user(string &username) {
    char confirmation;
    cout << "Are you sure you want to delete your account?" << endl;
    cout << "Enter (Y) for yes or (N) for no: ";
    cin >> confirmation;
    if (confirmation == 'Y' || confirmation == 'y') {
        cout << "You have successfully deleted your account" << endl;
        user_database.erase(username);
        username = "";
    }
    else if (confirmation == 'N' || confirmation == 'n') {
        cout << "You have decided to cancel" << endl;
    }
    else {
        cout << "Invalid input" << endl;
    }
}

void User_data::output_database() {
    ofstream outFile;
    outFile.open("users.txt", ios::out | ios::app);
    for (auto key:user_database) {
        auto values = key.second;
        outFile << key.first << "," << values.get_NRIC() << "," << values.get_name() << "," 
        << values.get_email() << "," << values.get_password();
        outFile << endl;
    }
    outFile.close();
}

void User_data::input_database() 
{
    string line;
    ifstream inFile("users.txt"); // Hardcode the filename here for a cleaner UX

    if (!inFile.is_open()) // If file doesn't exist, just return; dummy_data() will handle the start
    {
        return;
    }
    while (getline(inFile, line)) 
    {
        stringstream ss(line);
        string uname, nric, name, email, pass;

        // Pase the CSV format: username, NRIC, name, email, password
        if (getline(ss, uname, ',') && getline(ss, nric,',') && getline(ss, name, ',') 
        && getline(ss, email, ',') && getline(ss, pass, ','))
        {
            Users temp(name, nric, email, pass);
            user_database[uname] = temp; // Load into the map silently
        }
    }
    inFile.close();
}

string User_data::login() {
    string username, password;
    cout << endl;
    cout << "Enter username: ";
    cin >> username;
    if (user_database.count(username)) {
        Users tempObj = user_database.at(username);
        string check = tempObj.get_password(); 
        cout << "Enter password: ";
        cin >> password;
        if (password == check) {
            cout << "Logged in!" << endl;
            return username;
        }
        else {
            cout << "Wrong password" << endl;
            return "";
        }
    }
    else {
        cout << "Username does not exist" << endl;
        return "";
    }
}

void User_data::create_librarian() {
    Users dummy_librarian{"T0123123F", "Librarian", "libby@gmail.com", "L1brarian"};
    user_database.insert({"T0123123F", dummy_librarian});
}
