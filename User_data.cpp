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
    cout << "Enter NRIC: ";
    cin >> NRIC;
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

void User_data::dummy_data(){
    Users dummy1{"Chris", "T0321927A", "chris@gmail.com", "Abcd1234"};
    Users dummy2{"Yeoman", "T0317369G", "Yeoman@outlook.com", "YYeoman9"};
    Users dummy3{"Anthony", "T1234567B", "anton@icloud.com", "@T0nytkha"};
    Users dummy4{"Louis", "T0987654Z", "louis@yahoo.com", "1122AAbb"};
    user_database.insert({"T0321927A", dummy1});
    user_database.insert({"T0317369G", dummy2});
    user_database.insert({"T1234567B", dummy3});
    user_database.insert({"T0987654Z", dummy4});
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