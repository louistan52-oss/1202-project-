#pragma once
#include <iostream> //basic
#include <string> //string-related func
#include <iomanip> //manipulators
#include <sstream> //istringstream, ostringstream
#include <cstring> //convert/read string
#include <cctype> //convert type
#include <fstream> //file handling
#include <ctype.h> //char-related functions
#include <vector> //dyanamic memory

using namespace std;

class Books{
    private:
        bool access; //access level for BMS - user or librian
    public:
        string title, genre, serial, venue;  //declare needed book datas - book title, genre, serial number, venue

        Books(): title(""), genre(""), serial(""), venue(""){}; //default constructor
        Books(string t, string g, string s, string v) : title(t), genre(g), serial(s), venue(v){};
};

int BMS_L(int cat=0, int sort=0);
int BMS_L(const char& v = 0);
int BMS_L(int RTS=0, const char& v=0);
int BMS();