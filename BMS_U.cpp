#include "BMS.h"
#include <iostream>

using namespace std;

void BookManagementUser::updateDatabase(string serial, Books::BookData data) {
        //reduce for database
        ofstream BMS("LibraryBooks.txt");
        if (!BMS) BookManagementUser::showErrorMessage("Error Opening File!");
        for (int i = 0; i < data.book.size(); i++){
            if (serial == data.book[i].getSerial()){
                cout << "[DATABASE] Stock reduced for Title: " << data.book[i].getTitle() << endl;
            }
            else {
                //cout << "Book being written back into file: " << data.book[i].getTitle() << endl;
                BMS << data.book[i].getSerial() << "," << data.book[i].getTitle() << "," << data.book[i].getGenre() << "," << data.book[i].getVenue();
            }
            if ((i+1)>=data.book.size()) BMS.close();
            else BMS << endl;
        }
    }

    // Action: "Error message"
void BookManagementUser::showErrorMessage(string message) {
        cout << ">> ERROR: " << message << " <<" << endl;
    }

void BookManagementUser::startSession() {
    Books b;
    char scanAnother = 'Y', venue;
    string currentSerial;
    cout << "--- System Started ---" << endl;
    cout << "Please confirm the location you are going to:\nA\tB\tC\nYour Venue: ";
    do{
        cin >> venue;
        venue = toupper(venue);
        if ((venue=='A')||(venue=='B')||(venue=='C')) break;
        else{
            cout << "Invalid input. Please try again.\n";
            cin.clear();
            cin.ignore();
        }
    }while(!(cin >> venue)||(venue!='A')||(venue!='B')||(venue!='C'));
    while (scanAnother=='Y') {
        Books::BookData data = b.loadBooks("LibraryBooks.txt", &venue);
        switch (venue){
            case 'A':
                b.BMS_L('A',4, true);
                break;
            case 'B':
                b.BMS_L('B',4, true);
                break;
            case 'C':
                b.BMS_L('C',4, true);
                break;
        }
        if (scannedBooks.size() >= MAX_BOOKS) {
            showErrorMessage("Maximum book limit (2) reached!");
            printReceipt();
            return;
        }
        cout << "\nPlease scan book serial: ";
        cin >> currentSerial;

        auto it = find(scannedBooks.begin(), scannedBooks.end(), currentSerial);
        if (it != scannedBooks.end()) {
            showErrorMessage("Duplicate scan detected!");
        } 
        else{
            for (int i = 0; i < data.book.size(); i++){
                if (currentSerial!=data.book[i].getSerial()) {
                    //cout << "checking " << i << " of " << data.book.size() << " books...\n";
                    if ((i+1) >= data.book.size()) showErrorMessage("Invalid Serial Data!");
                }
                else {
                    //cout << "Book Found. Updating Database...\n";
                    updateDatabase(currentSerial, data);
                    scannedBooks.push_back(currentSerial);
                    break;
                }
            }
        }
        
        do{
            cout << "Scan another book? (y/n): ";
            cin >> scanAnother;
            scanAnother=toupper(scanAnother);
            if (scanAnother=='Y') break;
            else if (scanAnother=='N') break;
            else{   
                cout << "Invalid Input. Please try again: ";
                cin.clear();
                cin.ignore();  
            }
        }while(scanAnother!='Y' || scanAnother!='N');
    }

    printReceipt();
    cout << "--- Session Ended ---" << endl;
}

void BookManagementUser::printReceipt() {
        cout << "\n================================" << endl;
        cout << "RECEIPT FOR TAKEN BOOKS" << endl;
        cout << "================================" << endl;
        if (scannedBooks.empty()) {
            cout << "No books scanned." << endl;
        } else {
            for (const auto& serial : scannedBooks) {
                cout << "Book Serial: " << serial << endl;
            }
        }
        cout << "Total Items: " << scannedBooks.size() << endl;
        cout << "================================\n" << endl;
    }
