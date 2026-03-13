#include "Users.h" // Users header file with method declarations
#include <string> // String manipulations
#include <iostream> // I/O stream
#include <vector> // Vectors
#include <map> // Maps
#include <fstream> // I/O Filestream
#include <sstream> 
#include <iomanip> // I/O Manipulations
using namespace std;

// Users class
Users::Users() {} // Default Constructor
Users::Users(string user_name, string user_NRIC, string user_email, string user_password) { //Constructor
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

string Users::get_name() { // Get data member "name" of object
    return name;
}

string Users::get_NRIC() { // Get data member "NRIC" of object
    return NRIC;
}

string Users::get_email() { // Get data member "email" of object
    return email;
}

string Users::get_password() { // Get datamember "password" of object
    return password;
}

void Users::display_user() { // Display data members (name, NRIC, email) of object
    cout << "Name: " << get_name() << endl;
    cout << "NRIC: " << get_NRIC() << endl;
    cout << "Email: " << get_email() << endl;
}


// User_data class
User_data::User_data() {} // Default constructor
User_data::User_data(map<string, Users> user_database, vector<string> all_NRIC) {} // Constructor

void User_data::create_user() { // Create user and add to map to store object
    string NRIC, name, email, password, username;
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
    cout << "Enter username: ";
    cin >> username;
    Users tempObj{name, NRIC, email, password};
    user_database.insert({username, tempObj});
    all_NRIC.push_back(NRIC);
    //return tempObj; 
}

Users User_data::retrieve_user() { // Retrieve data of object when given username
    string username;
    cout << "Enter username: ";
    cin >> username;
    Users tempObj = user_database.at(username);
    tempObj.display_user();
    return tempObj;
}

void User_data::update_user() { // Update data of object when given username
    int change_val;
    string change;
    cout << "Enter the corresponding number to change the value" << endl;
    cout << "(1) Name" << endl;
    cout << "(2) NRIC" << endl;
    cout << "(3) Email" << endl;
    cout << "(4) Password" << endl;    
    cout << "(5) Userame" << endl;
    cout << "(0) To exit" << endl;
    cin >> change_val;
    cout << endl;
    switch (change_val) {
        case 1: 
            
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 0:
        default:
    }
}

void User_data::output_database() { // Output file data
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
    Users dummy2{"Yeoman", "T0317369G", "Yeoman@outlook.com", "YYYeoman"};
    Users dummy3{"Anthony", "T1234567B", "anton@icloud.com", "@T0nytkha"};
    Users dummy4{"Louis", "T0987654Z", "louis@yahoo.com", "1122AAbb"};
    user_database.insert({"chris", dummy1});
    user_database.insert({"yeoos", dummy2});
    user_database.insert({"anton", dummy3});
    user_database.insert({"louis", dummy4});
}


// Other functions
void menu() {
    cout << string(35, '=') << endl;
    cout << "Press 1 to create user details" << endl;
    cout << "Press 2 to retrieve user details" << endl;
    cout << "Press 3 to create user details" << endl;
    cout << "Press 4 to create user details" << endl;
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
