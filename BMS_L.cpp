#include "BMS.h"
#include "Robot_Transport_System.h"
using namespace std;

SystemController SC;

Books::BookData Books::loadBooks(const char* Filename, const char* venueFilter){
    Books::BookData data;
    stringstream ss;//stringstream to allow cin/cout of string related variables
    ifstream BMS(Filename); //opens text file
    string title, genre, serial, venue, line;  //declaration of variables

    if (!BMS) { cout << "error opening file!\n"; return data; } //if unable to find or open file, returns error

    for (int i=0;!BMS.eof(); i++){
        ss.clear(); 
        getline(BMS,line); //while BMS is read into line
        ss.str(line); //sets line for string manipulation
        if (line.empty()) continue;
        ss >> serial;//first set of data is fed into serial                    
        ss.ignore(); //skips any extra text, like space
        getline(ss, title, '\t');   //sets second set of read data to title, ends when reader encounters \t
        getline(ss, genre, '\t');   //repeat for genre and venue
        getline(ss, venue, '\n');

        // if venueFilter is set, skip non-matching venues
        if (venueFilter && venue[0] != *venueFilter) continue;

        data.book.push_back(Books(title, genre, serial, venue)); //vector function: since book is a declared class of Books, will need to use the Books constructor to add to book. pushback will add to the back of the current <vector>Books book
        if (title.length()+5>data.size[0]) data.size[0]=title.length()+5; //iomanip: checks the longest length and sets it to be the size of the setw later
        if (genre.length()+5>data.size[1]) data.size[1]=genre.length()+5; //repeat for genre. only these two needed as they have varying lengths
    }
    data.size[2] = to_string(data.book.size()).length()+5;
    BMS.close();
    return data;
};

int Books::BMS_L(int cat, bool sort){ //input category, sort

   BookData data = loadBooks("LibraryBooks.txt");
    
    auto cmp = [&](const Books& a, const Books& b) -> bool {
        string fieldA, fieldB;
        if (cat == 2) { fieldA = a.genre;  fieldB = b.genre; }
        if (cat == 3) { fieldA = a.title;  fieldB = b.title; } 
        if (cat == 4 || cat == 1) { fieldA = a.serial; fieldB = b.serial;}        
        return sort ? (fieldA < fieldB) : (fieldA > fieldB);
    };
    std::sort(data.book.begin(), data.book.end(), cmp);

    data.size[2]=to_string(data.book.size()).length()+5;  //iomanip: reads longest length of book size for serial number. i.e. legnth of number 10 is 2, so 2+5 spaces away is the next printed text
    
    Books::printHeader(data);

    for (int i=0; i<data.book.size();i++){   //as long as i is less than book size, continue to print out values in book.
        cout << left << setw(data.size[2]) << i+1 << setw(data.size[1]) << data.book[i].genre << setw(15) 
        << data.book[i].serial << setw(data.size[0]) <<  data.book[i].title
        << setw(5) << data.book[i].venue << endl;
    }
    cout << data.book.size() << " Books remaining" << endl;
    return 0;
};

void Books::printHeader(Books::BookData data){
    if ((data.book[0].getTitle() == "\0")){
        cout << left << setw(data.size[2]) << "No." << setw(data.size[1]) << "Genre" << setw(15) << "Serial"
        << setw(data.size[0]) << "Title" << setw(5) << endl; //prints category of output text according to max length of each string. fixed values for others
    }
    else{
        cout << left << setw(data.size[2]) << "No." << setw(data.size[1]) << "Genre" << setw(15) << "Serial"
        << setw(data.size[0]) << "Title" << setw(5) << "Venue" << endl; //prints category of output text according to max length of each string. fixed values for others
    }
};

int Books::BMS_L(const char& v, int cat, bool sort){ //input: venue
    BookData data = loadBooks("LibraryBooks.txt", &v);

    auto cmp = [&](const Books& a, const Books& b) -> bool {
        string fieldA, fieldB;
        if (cat == 2) { fieldA = a.genre;  fieldB = b.genre; }
        if (cat == 3) { fieldA = a.title;  fieldB = b.title; } 
        if (cat == 4 || cat == 1) { fieldA = a.serial; fieldB = b.serial;}        
        return sort ? (fieldA < fieldB) : (fieldA > fieldB);
    };
    std::sort(data.book.begin(), data.book.end(), cmp);

    cout << "Venue " << v << " Database" << endl; 
    data.size[2]=to_string(data.book.size()).length()+5;
    cout << left << setw(data.size[2]) << "No." << setw(data.size[1]) << "Genre" << setw(15) << "Serial"
    << setw(data.size[0]) << "Title"<< endl;
    for (int i=0; i<data.book.size();i++){   //displays books at the venue
        cout << left << setw(data.size[2]) << i+1 << setw(data.size[1]) << data.book[i].genre << setw(15) 
        << data.book[i].serial << setw(data.size[0]) << data.book[i].title << endl;
    }
    cout << data.book.size() << " Books in Venue " << v << endl;
    return 0;
};

int Books::BMS_L(int RTS, const char& v){
    //call RTS status
    cout << "simulating rts: robot no." << RTS << "of venue:" << v << endl;
    return 0;
};

int catSelect(){
    int cat;
    cout << "Choose specific category:\n1. View All\n2. View By Genre\n3. View by title\n4. View by serial no.\nChoice:";
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
    cout <<"Sort in Ascending Order? Y/N: ";
    
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
    cout <<"Select among these venue(s):\nA\tB\tC\n";
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
        cout << "BMS - Librarian\n1. View all books\n2. Check Specific Venue"
        << "\n3. Check RTS\n4. Exit BMS\nAwaiting user input: ";
        cin >> choice; //first choice, menu option
        cin.clear();   //clear all input
        cin.ignore();
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
            case '4':   //Exit
                cout << "exiting program." << endl;
                return 0;
            default:    //Error
                cout << "Invalid option. Please try again: " << endl;
                break;
        }
        
        do{
            cout << "Continue using BMS? (Y/N): ";  //asks if librarian wants to continue using BMS
            cin >> choice; //input choice
            if (choice=='y' || choice == 'n') choice = toupper(choice); //changes input to upper case if y or n. otherwise proceed
            cin.clear(); //clears input
            if (choice == 'Y') break; //ends loop and continue using BMS
            else if (choice == 'N') choice = '4'; //ends loop and forces exit state (choice = 4 to exit while loop of parent function)
            else cout << "Invalid input. Please try again: ";
        }while((choice!='Y')&&(choice!='4'));

    }while(choice!='4');
    return 0;
};