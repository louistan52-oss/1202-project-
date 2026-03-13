#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

class BookManagementUser {
private:
    std::vector<std::string> scannedBooks; // Stores serials for the current session
    const int MAX_BOOKS = 2;               // Flowchart logic: "Books taken >= 2?"

    // Action: "Reduce book count in database"
    void updateDatabase(std::string serial) {
        std::cout << "[DATABASE] Stock reduced for serial: " << serial << std::endl;
    }

    // Action: "Error message"
    void showErrorMessage(std::string message) {
        std::cout << ">> ERROR: " << message << " <<" << std::endl;
    }

public:
    void startSession() {
        char scanAnother = 'y';
        std::string currentSerial;

        std::cout << "--- System Started ---" << std::endl;

        while (scanAnother == 'y' || scanAnother == 'Y') {
            // Step: "Scan book serial"
            std::cout << "\nPlease scan book serial: ";
            std::cin >> currentSerial;

            // Decision: "Duplicate scan?"
            auto it = std::find(scannedBooks.begin(), scannedBooks.end(), currentSerial);
            if (it != scannedBooks.end()) {
                showErrorMessage("Duplicate scan detected!");
            } 
            // Decision: "Books taken >= 2?"
            else if (scannedBooks.size() >= MAX_BOOKS) {
                showErrorMessage("Maximum book limit (2) reached!");
            } 
            else {
                // Action: "Reduce book count in database"
                updateDatabase(currentSerial);
                scannedBooks.push_back(currentSerial);
            }

            // Decision: "Scan another book?"
            std::cout << "Scan another book? (y/n): ";
            std::cin >> scanAnother;
        }

        // Action: "Receipt of books taken"
        printReceipt();
        
        std::cout << "--- Session Ended ---" << std::endl;
    }

    void printReceipt() {
        std::cout << "\n================================" << std::endl;
        std::cout << "RECEIPT FOR TAKEN BOOKS" << std::endl;
        std::cout << "================================" << std::endl;
        if (scannedBooks.empty()) {
            std::cout << "No books scanned." << std::endl;
        } else {
            for (const auto& serial : scannedBooks) {
                std::cout << "Book Serial: " << serial << std::endl;
            }
        }
        std::cout << "Total Items: " << scannedBooks.size() << std::endl;
        std::cout << "================================\n" << std::endl;
    }
};

int main() {
    BookManagementUser session;
    session.startSession();
    return 0;

}
