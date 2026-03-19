
#include "BMS.h"
#include <iostream>

using namespace std;

void BookManagementUser::updateDatabase(string serial, Books::BookData data) {
        //reduce for database
        ofstream BMS("LibraryBooks.txt");
        if (!BMS) BookManagementUser::showErrorMessage("Error Opening File!");
        for (int i = 0; i < data.book.size(); i++){
            if (serial == data.book[i].getSerial()){ // If the serial matches, we "reduce stock" by not writing it back to the file
                cout << "[DATABASE] Stock reduced for Title: " << data.book[i].getTitle() << endl;
            }
            else {
                //cout << "Book being written back into file: " << data.book[i].getTitle() << endl;
                BMS << data.book[i].getSerial() << "," << data.book[i].getTitle() << "," << data.book[i].getGenre() << "," << data.book[i].getVenue() << endl;
            }
        }
        BMS.close();
    }

    // Utility function to print formatted error messages to the terminal.
void BookManagementUser::showErrorMessage(string message) {
        cout << ">> ERROR: " << message << " <<" << endl;
    }

// Primary session loop for users inside a venue
void BookManagementUser::startSession(char venue) { // Handles book scanning, limit validation, and database synchronization
    Books b;
    char scanAnother = 'Y';
    char decision;
    string currentSerial;
    
    cout << "--- System Started ---" << endl;
    cout << "\nDo you want to claim up to 2 books? (Y/N): ";
    while (true) // Input validation loop for initial decision
    {
        cin >> decision;
        decision = toupper(decision);
        if (decision == 'Y' || decision == 'N') break;

        cout << "Invalid Input. Please try again (Y/N): ";
        cin.clear();
        cin.ignore(1000, '\n');
    }
    if (decision == 'N') scanAnother = 'N';
    else if (decision == 'Y')
    {
        while (scanAnother=='Y') { // Main session loop: continues as long as user wants to scan and hasn't hit limits
            Books::BookData data = b.loadBooks("LibraryBooks.txt"); // Reload book data every loop to ensure real-time stock accuracy
            switch (venue){ // Display books available specifically for this venue
                case 'A':
                    b.BMS_L('A',4,true); break;
                case 'B':
                    b.BMS_L('B',4,true); break;
                case 'C':
                    b.BMS_L('C',4,true); break;
            } 

            cout << "\nPlease scan book serial: ";
            cin >> currentSerial;
            auto it = find(scannedBooks.begin(), scannedBooks.end(), currentSerial);
            if (it != scannedBooks.end()) { // Check for duplicate scans in the current session
                showErrorMessage("Duplicate scan detected!");
            } 
            else
            {
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
            if (scannedBooks.size() < MAX_BOOKS) 
            {   
                do
                {
                    cout << "Scan another book? (Y/N): "; // Secondary validation loop for "Scan Another" prompt
                    cin >> scanAnother;
                    scanAnother=toupper(scanAnother);
                    if (scanAnother== 'Y' || scanAnother== 'N') break;
                    else{   
                        cout << "Invalid Input. Please try again: ";
                        cin.clear();
                        cin.ignore(1000, '\n');  
                    }
                }while(true);  
            }
            else if (scannedBooks.size() >= MAX_BOOKS) { // Hard-cap limit check
                cout << "Maximum book limit (2) reached!" << endl;
                scanAnother = 'N'; // Automatically end loop if limit is reached
            }
        }
        printReceipt();
    }
    cout << "--- Session Ended ---" << endl;
}

void BookManagementUser::printReceipt() { // Prints a summary of claimed books and clears the session memory
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
        scannedBooks.erase(scannedBooks.begin(),scannedBooks.end()); // Clear the vector so the next user starts with a fresh session
    }
