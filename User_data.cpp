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
        cin.ignore(1000, '\n'); // Clear buffer after NRIC input
        if (user_database.count(NRIC)) {
            cout << "NRIC already in use. Please try again" << endl;
        }
    } while (user_database.count(NRIC));

    cout << "Enter name: ";
    getline(cin, name); // Read spaces for name input

    cout << "Enter email: ";
    getline(cin, email);

    do {
        cout << "Enter a password: ";
        cin >> password;
    } while (!password_verifier(password));

    //username = NRIC;
    Users tempObj{name, NRIC, email, password};
    user_database[NRIC] = tempObj;
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
    ofstream outFile("users.txt", ios::out | ios::trunc); // USE ios::trunc to overwrite. This prevents duplication
    
    // Default minimum widths for iomanip alignment
    size_t maxName = 14;
    size_t maxEmail = 24;

    for (auto const& [key, values] : user_database) // First pass (O(n)) calculates dynamic column widths based on actual data length
    {   // Ensures the UI scales correctly across different screen size / data sets
        if (key == "T0123123F") continue; 
        if (values.get_name().length() > maxName) maxName = values.get_name().length();
        if (values.get_email().length() > maxEmail) maxEmail = values.get_email().length();
    }

    for (auto const& [key, values] : user_database) { // Use 'setw' and 'left' function to align a tabular layout
        if (key == "T0123123F") continue; //Skips librarian account data to be save in user database

        outFile << left << setw(12) << values.get_NRIC() << "| "
                << setw(maxName + 2) << values.get_name() << "| "
                << setw(maxEmail + 2) << values.get_email() << "| "
                << values.get_password() << endl;
    }
    outFile.close();
}

void User_data::input_database() 
{
    string line;
    ifstream inFile("users.txt"); // Hardcode the filename here for a cleaner UX
    if (!inFile) return; // If file doesn't exist, just return;

    user_database.clear(); // Clear memory before loading to prevent RAM-based duplicates

    while (getline(inFile, line)) 
    {
        if (line.empty()) continue;

        // Uses stringstream and getline with a character anchor ('|') 
        stringstream ss(line);
        string nric, name, email, password;

        getline(ss, nric, '|'); //Read data using " | " as the anchor
        getline(ss, name, '|');
        getline(ss, email, '|');
        getline(ss, password); 
        
        //Removes leading / trailing spaces added by the iomanip formatting in output_database
        auto trim = [](string& s) 
        {
            size_t first = s.find_first_not_of(" ");
            if (string::npos == first) return;
            size_t last = s.find_last_not_of(" ");
            s = s.substr(first, (last - first + 1));
        };

        trim(nric);
        trim(name);
        trim(email);
        trim(password);

        if (!nric.empty())
        {
            Users temp(name, nric, email, password);
            user_database[nric] = temp; // Load into the map silently
        }
    }
    inFile.close();
}

string User_data::login() {
    string username, password;
    cout << endl;
    cout << "Enter username (NRIC): ";
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

void User_data::create_librarian() { // Ensure this ID matches to Librarian login
    Users dummy_librarian{"L1brarian", "Librarian", "libby@gmail.com", "L1brarian"};
    user_database.insert({"T0123123F", dummy_librarian});
}
