#pragma once

using namespace std;

int BMS_L(){
    stringstream ss;
    vector<Books> book;
    ifstream BMS("LibraryBooks.txt");
    string title, genre, serial, venue, line;
    int size[3]={0,0,0};
    if (!BMS) {cout << "error opening file!\n"; return 1;}
    for (int i=0; !BMS.eof(); i++){
        ss.clear();
        getline(BMS, line);
        ss.str(line);
        ss >> serial;
        ss.ignore();
        getline(ss, title, '\t');
        getline(ss, genre, '\t');
        getline(ss, venue, '\n');
        if (title.length()+5>size[0]) size[0]=title.length()+5;
        if (genre.length()+5>size[1]) size[1]=genre.length()+5;
        book.push_back(Books(title,genre,serial,venue));
    }
    size[2]=to_string(book.size()).length()+5;
    cout << left << setw(size[2]) << "No." << setw(size[1]) << "Genre" << setw(15) << "Serial"
    << setw(size[0]) << "Title" << setw(5) << "Venue" << endl;
    for (int i=0; i<book.size();i++){
        cout << left << setw(size[2]) << i+1 << setw(size[1]) << book[i].genre << setw(15) 
        << book[i].serial << setw(size[0]) <<  book[i].title
        << setw(5) << book[i].venue << endl;
    }
    if (BMS.is_open()) BMS.close();
    return 0;
};

int BMS_L(char v){
    vector<Books> book;
    ifstream BMS("LibraryBooks.txt");
    stringstream ss;
    string title, genre, serial, venue, line;
    int size[3]={0,0,0};
    if (!BMS) {cout << "error opening file!\n"; return 1;}
    for (int i=0; !BMS.eof(); i++){
        ss.clear();
        getline(BMS, line);
        ss.str(line);
        ss >> serial;
        ss.ignore();
        getline(ss, title, '\t');
        getline(ss, genre, '\t');
        getline(ss, venue, '\n');
        if (title.length()+5>size[0]) size[0]=title.length()+5;
        if (genre.length()+5>size[1]) size[1]=genre.length()+5; 
        if (venue[0]==v){
            book.push_back(Books(title,genre,serial,venue));
        }
    }
    size[2]=to_string(book.size()).length()+5;
    cout << left << setw(size[2]) << "No." << setw(size[1]) << "Genre" << setw(15) << "Serial"
    << setw(size[0]) << "Title" << setw(5) << "Venue" << endl;
    cout << "Venue " << v << " Database" << endl; 
    for (int i=0; i<book.size();i++){
        cout << left << setw(size[2]) << i+1 << setw(size[1]) << book[i].genre << setw(15) 
        << book[i].serial << setw(size[0]) << book[i].title << endl;
    }
    if (BMS.is_open()) BMS.close();
    return 0;
};

int BMS_L(int RTS){
    //call RTS status
    cout << "simulating rts" << endl;
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
                BMS_L(); 
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
                BMS_L(choice);
                break;
            case '3':   //RTS
                BMS_L(1); 
                break;
            case '4':   //Exit
                cout << "exiting program." << endl;
                return 0;
            default:    //Error
                cout << "Invalid option. Please try again: " << endl;
                break;
        }
        
        do{
            cout << "Continue using BMS? (Y/N): ";
            cin >> choice;
            if (choice=='y' || choice == 'n') choice = toupper(choice);
            cin.clear();
            if (choice == 'Y') break;
            else if (choice == 'N') choice = '4';
            else cout << "Invalid input. Please try again: ";
        }while((choice!='Y')&&(choice!='4'));

    }while(choice!='4');
    return 0;
};

/*
void BMS_View(string title[], string genre[], string serial[], string venue[], int max){
    cout << "  " << "Venue" << "Genre" << "Title" << "Serial\n";  
    for (int i = 0; i<max; i++){
        if (title[i]==" ") break;
        cout << i+1 << " " << venue[i] << " " << genre[i] << " " << title[i] << " " << serial[i] << endl;
    }
}

void BMS_View(string title[], string genre[], string serial[], string venue[], int max,int option){
    cout << "  " << "Genre" << "Title" << "Serial\n"; 
    switch(option){
        case 1: //venue A
            for (int i = 0; i<max; i++){
                if (venue[i] == "A")  cout << genre[i] << " " << title[i] << " " << serial[i] << endl;
            }
            break;
        case 2: //venue B
            for (int i = 0; i<max; i++){
                if (venue[i] == "B")  cout << genre[i] << " " << title[i] << " " << serial[i] << endl;
                }
            break;
        case 3: //venue C
            for (int i = 0; i<max; i++){
                if (venue[i] == "C")  cout << genre[i] << " " << title[i] << " " << serial[i] << endl;
                }
            break;
    }
}

int BMS_Close(){
    fstream BMS("LibraryBooks.txt");
    if (BMS.is_open()) BMS.close();
    return 0;
}

int BMS_Reader(int option){
    stringstream ss;
    const int max=20;
    int maxref=max;
    string serial[max], genre[max], title[max], venue[max], line;
    ifstream BMS("LibraryBooks.txt");
    if (!BMS) {cout << "error opening file!\n"; return 1;}
    for (int i=0; i<max; i++){
        getline(BMS, line);
        if (BMS.eof()) {
            maxref=i;
            break;
        }
        ss.clear();
        ss.str(line);
        ss >> serial[i];
        ss.ignore();
        getline(ss, title[i], '\t');
        getline(ss, genre[i], '\t');
        getline(ss, venue[i], '\n');
    }
    switch (option){
    case 1: //view all books
        BMS_View(title, genre, serial, venue, maxref);
        break;
    case 2:
        cout << "which venue? \n1. Venue A\n2. Venue B\n3. Venue C\n Choice: ";
        cin >> option;
        BMS_View(title, genre, serial, venue, maxref, option);
        break;
    default:
        cout << "invalid reader\n";
        break;
    }
    BMS.close();
    return 0;
}

int Continue(int option){
    cout << "Continue Viewing? \n1. yes\n2. no\nyour choice: ";
    while(true){
        cin.clear();
        cin.ignore(256,'\n');
        cin >> option;
        if (option == 1 || option == 2) return option;
        else {
            cout << "invalid option. try again: ";
            continue;
        }
    }
}

void BMS_Librarian(){
    int option=0;
    do{
        cout << "database options:\n1. View Remaining Books\n2. View Book Types\n3. RTS Status\n4. Exit Menu\nChoose your option: ";
        cin >> option;
        switch (option)
        {
        case 1:
            cout << "Viewing Remaining Books\n";
            BMS_Reader(option);
            break;
        case 2:
            cout << "Viewing Book Types\n";
            BMS_Reader(option);
            break;
        case 3:
            cout << "Checking Robotic Transport System Status\n";
            break;
        case 4:
            cout << "exited menu\n";
            BMS_Close();
            return;
        default:
            cout << "invalid choice\n";
            break;
        }
        BMS_Close();
        option = Continue(option);
    } while (option!=2);
    cout <<"stopped viewing\n";
}

void BMS_User(){
    int serial, books_taken=0;
    
    do{
        cout << "please scan your book's serial code: ";
        cin >> serial;
        if (serial==123) cout << "you have scanned the same book twice! please scan again: ";
        else {
            if (++books_taken>=2) {
                cout << "you have borrowed the maximum number of books.\n";
                break;
            }
            //reduce book count by 1 in category and database
            serial = Continue(serial);
            if (serial == 2) break;
        }
    } while (books_taken < 2);
    cout << "thank you for taking these books!\n";
}

void BMS_Access(){
    int access;
    cout << "are you a: \n1. librarian\n2. user\n";
    cin >> access;
    switch  (access){
        case 1:
            cout << "is librarian\n";
            BMS_Librarian();
            break;
        case 2:
            cout << "is user.\n";
            BMS_User();
            break;
        default:
            cout << "invalid input\n";
            break;
    }
}
    */