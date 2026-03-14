#ifndef BOOKS
#define BOOKS
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
        std::string getTitle() const { return title; }
        std::string getSerial() const { return serial; }
        std::string getGenre() const { return genre; }
        std::string getVenue() const { return venue; }
        struct BookData{
            std::vector<Books> book; //declared vector for book class -- allows program to dynamically allocate memory to the class during runtime
            int size[3] = {0,0,0}; //3 sizes to be used for iomanip -- text output manipulation
        };
        Books::BookData loadBooks(const char* Filename="LibraryBooks.txt", const char* venueFilter=nullptr);
        void printHeader(BookData data);
};
#endif

#ifndef BMS_U
#define BMS_U
class BookManagementUser {
private:
    std::vector<std::string> scannedBooks; // Stores serials for the current session
    Books::BookData data;

    // Action: "Reduce book count in database"
    void updateDatabase(std::string serial, Books::BookData data);

    // Action: "Error message"
    void showErrorMessage(std::string message);

public:
    static constexpr int MAX_BOOKS = 2;               // Flowchart logic: "Books taken >= 2?"
    void startSession();
    void printReceipt();
    std::vector<std::string> const getSerial(){ return scannedBooks; }
};

#endif
