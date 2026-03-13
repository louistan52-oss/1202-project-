#include "Users.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>

using namespace std;

void menu();
Users user_create();
bool password_verifier(string user_password);

int main() {
    map<string, Users> user_database;
    vector<string> all_NRIC;
    User_data dataObj(user_database, all_NRIC);
    Users userObj;
    dataObj.dummy_data();
    dataObj.retrieve_user();
    dataObj.create_user();
    dataObj.retrieve_user();
    dataObj.output_database();
    return 0;
}
