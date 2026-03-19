#include "BMS.h"
#include "Robot_Transport_System.h"
using namespace std;

SystemController SC;

// Helper function to remove leading / trailing whitespcace, tabs, and newlines
void trim(string& s) { // Clean data parsing from text file
    size_t first = s.find_first_not_of(" \t\r\n");
    if (string::npos == first) return;
    size_t last = s.find_last_not_of(" \t\r\n");
    s = s.substr(first, (last - first + 1));
}

Books::BookData Books::loadBooks(const char* Filename) { // Formatting and dynamically calculates column widths
    Books::BookData data; 
    ifstream BMS(Filename); //Loads book data from a text file
    stringstream ss;
    string line;
    string serial, title, genre, venue;
    // Default starting widths for columns
    data.size[0] = 10; // Min Title Width
    data.size[1] = 10; // Min Genre Width

    if (!BMS) { cout << "Error opening file!\n"; return data; } //if unable to find or open file, returns error

    for (int i = 0;!BMS.eof();i++) {
        ss.clear();
        getline(BMS,line);
        ss.str(line);
        if (line.empty()) continue;                  
        // Reading using TAB delimiter ','
        getline(ss, serial, ',');
        getline(ss, title, ','); //sets second set of read data to title, ends when reader 
        getline(ss, genre, ','); //repeat for genre and venue
        getline(ss, venue);

        trim(serial); trim(title); trim(genre); trim(venue); // Clean data for comparison and display

        data.book.push_back(Books(title, genre, serial, venue)); // Store book object in vector

        // Update max lengths for dynamic alignment
        if ((int)title.length() > data.size[0]) data.size[0] = title.length(); 
        if ((int)genre.length() > data.size[1]) data.size[1] = genre.length(); 
    }
    BMS.close();
    return data;
}


void Books::printHeader(Books::BookData data, bool showVenue) { // Generates standardized table header for book views
    int noW = 5;
    int genreW = data.size[1] + 3;
    int serialW = 15;
    int titleW = data.size[0] + 3;
    int venueW = 8;
    // Calculate total line width based on column visibility
    int totalWidth = noW + genreW + serialW + titleW + (showVenue ? (venueW + 3) : 0) + 7;

    cout << endl << string(totalWidth, '=') << endl;
    cout << "LIBRARIAN VIEW: BOOK DATABASE" << endl;
    cout << string(totalWidth, '=') << endl;

    cout << left << setw(noW) << "No." << " | "
         << setw(genreW) << "Genre" << " | "
         << setw(serialW) << "Serial" << " | "
         << setw(titleW) << "Title";

    if (showVenue) cout << " | " << setw(venueW) << "Venue";
    cout << endl << string(totalWidth, '-') << endl;
}

int Books::BMS_L(int cat, bool sort, bool show) { // Display View: All books
    // Supports sorting by category (Serial, Genre, Title) and book ordering (Ascending / Descending)
    BookData data = loadBooks("LibraryBooks.txt");  
    auto cmp = [&](const Books& a, const Books& b) -> bool { // Lambda for custom sorting based on user selection
        string fieldA, fieldB;
        if (cat == 2) { fieldA = a.genre; fieldB = b.genre; }
        else if (cat == 3) { fieldA = a.title; fieldB = b.title; }
        else { fieldA = a.serial; fieldB = b.serial; }
        return sort ? (fieldA < fieldB) : (fieldA > fieldB);
    };
    std::sort(data.book.begin(), data.book.end(), cmp);

    if (show){
        Books::printHeader(data, true); // Render table header

        int genreW = data.size[1] + 3;
        int titleW = data.size[0] + 3;
    
        for (size_t i = 0; i < data.book.size(); i++) { // Each iteration prints a table row
            cout << left << setw(5) << i + 1 << " | "
                << setw(genreW) << data.book[i].genre << " | "
                << setw(15) << data.book[i].serial << " | "
                << setw(titleW) << data.book[i].title << " | "
                << data.book[i].venue << endl;
        }

        int totalWidth = 5 + genreW + 15 + titleW + 8 + 10; // Dynamic border closure based on column widths
        cout << string(totalWidth, '=') << endl;
        cout << data.book.size() << " Books remaining" << endl;
    }
    return 0;
}

// Display View: Books by specific Venue
int Books::BMS_L(const char& v, int cat, bool sort, bool show) {
    BookData data = loadBooks("LibraryBooks.txt");
    string venue;
    int venue_count=0;
    auto cmp = [&](const Books& a, const Books& b) -> bool { // Lambda for custom sorting based on user selection
        string fieldA, fieldB;
        if (cat == 2) { fieldA = a.genre; fieldB = b.genre; }
        else if (cat == 3) { fieldA = a.title; fieldB = b.title; }
        else { fieldA = a.serial; fieldB = b.serial; }
        return sort ? (fieldA < fieldB) : (fieldA > fieldB);
    };
    std::sort(data.book.begin(), data.book.end(), cmp);

    if (show){
        cout << "\nDisplaying Books for Venue: " << v << endl;
        Books::printHeader(data, false); // Don't show venue col since we filtered for it

        int genreW = data.size[1] + 3;
        int titleW = data.size[0] + 3;
    
        for (size_t i = 0; i < data.book.size(); i++) { //displays books at the venue
            
            if ((venue=v)==data.book[i].getVenue()) {
                cout << left << setw(5) << i + 1 << " | " <<
                setw(genreW) << data.book[i].genre << " | "
                << setw(15) << data.book[i].serial << " | "
                << setw(titleW) << data.book[i].title << endl;
                venue_count++;
            }
            else continue;
        }

        // Calculate width without the venue column (+7 for separators, +3 for base)
        int totalWidth = 5 + genreW + 15 + titleW + 7; 
        cout << string(totalWidth, '=') << endl;
        cout << venue_count << " Books in Venue " << v << " remaining" << endl;
    }
    return 0;
}

void Books::viewAllAccounts() // Display user account database table
{
    ifstream inFile("users.txt");
    string line;
    struct UserRow { string nric, name, email, password; };
    vector<UserRow> rowData;

    int maxName = 13, maxEmail = 23;
    if (!inFile.is_open()) return;

    while (getline(inFile, line)) // Read data from users.txt and calculate required widths
    {
        if(line.empty()) continue;
        stringstream ss(line);
        UserRow row;
        
        getline(ss, row.nric, '|'); //Matching "|" delimiter used in User_data::output_database to ensure cross-module data consistency
        getline(ss, row.name, '|');
        getline(ss, row.email, '|');
        getline(ss, row.password);

        // Trimming for length check
        auto trim = [](string& s)
        {
            size_t first = s.find_first_not_of(" ");
            if (string::npos == first) return;
            size_t last = s.find_last_not_of(" ");
            s = s.substr(first, (last - first + 1));
        };

        trim(row.nric);
        trim(row.name);
        trim(row.email);
        trim(row.password);

        if ((int)row.name.length() > maxName) maxName = row.name.length();
        if ((int)row.email.length() > maxEmail) maxEmail = row.email.length();

        rowData.push_back(row);

    }
    inFile.close();
    // Table creation using IO manipulators
    int nricW = 11; // Define the width variables BEFORE the second pass
    int nameW = maxName + 2;
    int emailW = maxEmail + 2;
    cout << endl;
    cout << string(nricW + nameW + emailW + 25, '=') << endl; // Calulated lendths to draw a seperator lines creates a clean table for reading
    cout << "LIBARIAN VIEW: USER DATABASE          " << endl;
    cout << string(nricW + nameW + emailW + 25, '=') << endl;
    cout << left << setw(11) << "NRIC" << " | " 
         << setw(maxName + 2) << "Name" << " | " 
         << setw(maxEmail + 2) << "Email" << " | "
         << "Password" << endl;
    cout << string(nricW + nameW + emailW + 25, '-') << endl;

    // Using 'const auto&' prevents unnecessary copying of 'UserRow' objects
    for (const auto& row : rowData) {
            cout << left << setw(nricW) << row.nric << " | "
                 << setw(nameW) << row.name << " | "
                 << setw(emailW) << row.email << " | "
                 << row.password << endl;
    }

    cout << string(nricW + nameW + emailW + 25, '=') << endl;
}


int catSelect(){
    int cat;
    cout << "\nChoose specific category:\n1. View All\n2. View By Genre\n3. View by title\n4. View by serial no.\nChoice: ";
    while(!(cin >> cat) || cat < 1 || cat > 5){   
        cout << "Invalid Input. Please try again: ";
        cin.clear();
        cin.ignore();  
    }
    return cat;
}

int sortSelect(){
    int sort;
    char choice;
    cout <<"\nSort in Ascending Order? Y/N: ";
    
    do{
        cin >> choice;
        choice = toupper(choice);
        if (choice=='Y') sort = 1;
        else if (choice=='N') sort = 0;
        else {
            cin.clear();
            cin.ignore();
            cout << "Invalid Input. Please try again: ";
        }
    }while( ((choice!='Y')&&(choice!='N')));
    return sort;
}

char venueSelect(){
    char choice;
    cout <<"\nSelect among these venue(s):\nA\tB\tC\n";
    do{
        cout << "Awaiting user input: ";
        
        cin >> choice;
        if ((choice=='a')||(choice=='b')||(choice=='c')) choice = toupper(choice);
        if ((choice!='A')&&(choice!='B')&&(choice!='C')){
            cout << "Please select a valid venue.\n";
        }
    }while((choice!='A')&&(choice!='B')&&(choice!='C'));
    return choice;
}

int Books::BMS(){
    char choice;
    int cat;
    bool sort;
    do{
        cout << "\n=== BMS - Librarian ===" << endl;
        cout << "1. View all books" << endl;
        cout << "2. Check Specific Venue" << endl;
        cout << "3. Check RTS" << endl; 
        cout << "4. View Registered User Accounts" << endl; 
        cout << "5. Exit BMS" << endl; 
        cout << "Awaiting user input:" << endl ;

        cin >> choice; //first choice, menu option
        cin.clear();   //clear all input
        cin.ignore(1000, '\n');
        switch (choice){
            case '1':   //show all books across database          
                cat = catSelect();
                sort= sortSelect();
                Books::BMS_L(cat,sort); //inputs: category, sorting (high-low)
                break;
            case '2':   //2nd choice: venue.
                choice = venueSelect();
                cat = catSelect();
                sort = sortSelect();
                Books::BMS_L(choice, cat, sort); //input: venue
                break;
            case '3':
                SC.run();
                break;
            case '4': 
                Books::viewAllAccounts();
                break;
            case '5': //Exit
                cout << "Exiting BMS." << endl;
                return 0;
            default:    //Error
                cout << "Invalid option. Please try again: " << endl;
                break;
        }
        
        do{ // Loop to allow multiple operations before logout
            cout << "\nContinue using BMS? (Y/N): ";  //asks if librarian wants to continue using BMS
            cin >> choice; //input choice
            if (choice=='y' || choice == 'n') choice = toupper(choice); //changes input to upper case if y or n. otherwise proceed
            cin.clear(); //clears input
            if (choice == 'Y') break; //ends loop and continue using BMS
            else if (choice == 'N') choice = '5'; //ends loop and forces exit state (choice = 4 to exit while loop of parent function)
            else cout << "Invalid input. Please try again: ";
        }while((choice!='Y')&&(choice!='5'));

    }while(choice!='5');
    return 0;
};
