#ifndef USERS
#define USERS

#include <iostream> // Input and output streams
#include <string> // String manipulation
#include <vector> // Vectors
#include <map> // Maps
#include <iomanip>
using namespace std;

class Users
{
    private:
        string name, NRIC, email, password; // Object variables
    public:
        Users(); // Default Constructor
        Users(string user_name, string user_NRIC, string user_email, string user_password); // Constructor

        void set_name(string user_name); //set functions
        void set_NRIC(string user_name);
        void set_email(string user_name);
        void set_password(string user_name);
        void display_user();
        //void input_database();

        string get_name() const; //get functions
        string get_NRIC() const;
        string get_email() const;
        string get_password() const;

        bool password_verifier(string user_password);
/*
TO DO:

- User verification (does account exist?)
- do the password verifier (one caps?, at least 8 char?, at least one number?)
- dupe nric (should be char[9])
- 


*/
};
#endif

#ifndef USER_DATA
#define USER_DATA

class User_data 
{
    private:
        map<string, Users> user_database;
        vector<string> all_NRIC;

    public:
        User_data(); // Default constructor
        User_data(map<string, Users> user_database, vector<string> all_NRIC); // Constructor
        void create_user(); // Create user object and input into database
        Users retrieve_user(); // Retrieve user object from database
        bool delete_user(string &username); // Delete user object from database
        void output_database(); // Write map onto text file
        void input_database(); // Read text file data and convert to map
        void create_librarian(); // Creates a fake librarian with details
        string login();
};
#endif

// Other functions
bool password_verifier(string user_password);
void menu();
void program();
