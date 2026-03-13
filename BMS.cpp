#pragma once //to prevent reading the same file more than once during compiling
using namespace std;

int BMS_L(int cat, int sort){ //input category, sort
    stringstream ss;    //stringstream to allow cin/cout of string related variables
    vector<Books> book; //declared vector for book class -- allows program to dynamically allocate memory to the class during runtime
    ifstream BMS("LibraryBooks.txt"); //opens text file
    string title, genre, serial, venue, line; //declaration of variables
    int size[3]={0,0,0};    //3 sizes to be used for iomanip -- text output manipulation
    if (!BMS) {cout << "error opening file!\n"; return 1;}  //if unable to find or open file, returns error
    for (int i=0;!BMS.eof(); i++){
        ss.clear(); 
        getline(BMS,line); //while BMS is read into line
        ss.str(line); //sets line for string manipulation
        ss >> serial;//first set of data is fed into serial                    
        ss.ignore(); //skips any extra text, like space
        getline(ss, title, '\t');   //sets second set of read data to title, ends when reader encounters \t
        getline(ss, genre, '\t');   //repeat for genre and venue
        getline(ss, venue, '\n');
        if (title.length()+5>size[0]) size[0]=title.length()+5; //iomanip: checks the longest length and sets it to be the size of the setw later
        if (genre.length()+5>size[1]) size[1]=genre.length()+5; //repeat for genre. only these two needed as they have varying lengths
        book.push_back(Books(title,genre,serial,venue));        //vector function: since book is a declared class of Books, will need to use the Books constructor to add to book. pushback will add to the back of the current <vector>Books book
    }
    //remind me to add sorting function
    size[2]=to_string(book.size()).length()+5;  //iomanip: reads longest length of book size for serial number. i.e. legnth of number 10 is 2, so 2+5 spaces away is the next printed text
    cout << left << setw(size[2]) << "No." << setw(size[1]) << "Genre" << setw(15) << "Serial"
    << setw(size[0]) << "Title" << setw(5) << endl; //prints category of output text according to max length of each string. fixed values for others
    for (int i=0; i<book.size();i++){   //as long as i is less than book size, continue to print out values in book.
        cout << left << setw(size[2]) << i+1 << setw(size[1]) << book[i].genre << setw(15) 
        << book[i].serial << setw(size[0]) <<  book[i].title
        << setw(5) << book[i].venue << endl;
    }
    BMS.close(); //forces BMS to close regardless of status
    return 0;
};

int BMS_L(const char& v){ //input: venue
    vector<Books> book;       //declared vector for book class -- allows program to dynamically allocate memory to the class during runtime
    ifstream BMS("LibraryBooks.txt");   //opens text file
    stringstream ss; //stringstream to allow cin/cout of string related variables
    string title, genre, serial, venue, line; //declares variables
    int size[3]={0,0,0};    //size for iomanip
    if (!BMS) {cout << "error opening file!\n"; return 1;}  //if cannot open file, returns error
    for (int i=0; !BMS.eof(); i++){ //while BMS is not end of file, reads from text and save as string
        ss.clear();
        getline(BMS, line);
        ss.str(line);
        ss >> serial;
        ss.ignore();
        getline(ss, title, '\t');
        getline(ss, genre, '\t');
        getline(ss, venue, '\n');
        if (venue[0]==v){   //only adds to the class if venue matches the libarian input from earlier
            book.push_back(Books(title,genre,serial,venue));
            if (title.length()+5>size[0]) size[0]=title.length()+5; //changes max setw based on longest text length
            if (genre.length()+5>size[1]) size[1]=genre.length()+5; 
        }
    }
    cout << "Venue " << v << " Database" << endl; 
    size[2]=to_string(book.size()).length()+5;
    cout << left << setw(size[2]) << "No." << setw(size[1]) << "Genre" << setw(15) << "Serial"
    << setw(size[0]) << "Title" << setw(5) << "Venue" << endl;
    for (int i=0; i<book.size();i++){   //displays books at the venue
        cout << left << setw(size[2]) << i+1 << setw(size[1]) << book[i].genre << setw(15) 
        << book[i].serial << setw(size[0]) << book[i].title << endl;
    }
    //will need to differentiate this function from the above one. remind me.
    if (BMS.is_open()) BMS.close();
    return 0;
};

int BMS_L(int RTS, const char& v){
    //call RTS status
    cout << "simulating rts: robot no." << RTS << "of venue:" << v << endl;
    return 0;
};

int BMS(){
    char choice;
    do{
        cout << "BMS - Librarian\n1. View all books\n2. Check Specific Venue"
        << "\n3. Check RTS\n4. Exit BMS\nAwaiting user input: ";
        cin >> choice; //first choice, menu option
        cin.clear();   //clear all input
        cin.ignore();
        switch (choice){
            case '1':   //show all books across database
                BMS_L(1,1); //inputs: category, sorting (high-low)
                break;
            case '2':   //2nd choice: venue.
                cout <<"Select among these venue(s):\nA\tB\tC\n";
                do{
                    cout << "Awaiting user input: ";
                    
                    cin >> choice;
                    if ((choice=='a')||(choice=='b')||(choice=='c')) choice = toupper(choice);
                    if ((choice!='A')&&(choice!='B')&&(choice!='C')){
                        cout << "Please select a valid venue.\n";
                    }
                }while((choice!='A')&&(choice!='B')&&(choice!='C'));
                BMS_L(choice); //input: venue
                break;
            case '3':   //RTS
                BMS_L(1, choice); //input: robot number, venue
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