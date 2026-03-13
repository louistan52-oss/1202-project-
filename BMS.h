#pragma once
#include <iostream> //basic
#include <string> //string-related func
#include <iomanip> //manipulators
#include <sstream> //istringstream, ostringstream
#include <cstring> //convert/read string
#include <cctype> //convert type
#include <fstream> //file handling
#include <ctype.h> //char-related functions
#include <algorithm>
#include <vector> //dyanamic memory

class Books{
    private:
        std::string title, genre, serial, venue;  //declare needed book datas - book title, genre, serial number, venue]
    public:
        Books(): title(""), genre(""), serial(""), venue(""){}; //default constructor
        Books(std::string t, std::string g, std::string s, std::string v) : title(t), genre(g), serial(s), venue(v){};
        int BMS_L(int cat=0, bool sort=0);
        int BMS_L(const char& v = 0, int cat=0, bool sort=0);
        int BMS_L(int RTS=0, const char& v=0);
        int BMS();
        std::string getSerial() const { return serial; }
        struct BookData{
            std::vector<Books> book; //declared vector for book class -- allows program to dynamically allocate memory to the class during runtime
            int size[3] = {0,0,0}; //3 sizes to be used for iomanip -- text output manipulation
        };
};


