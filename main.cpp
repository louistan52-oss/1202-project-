#include "BMS.h"
//#include "BMS_L.cpp"
//#include "BMS_U.cpp"
/*
when actually compiling
*/

Books lib;
BookManagementUser session;
using namespace std;

int main(){
    char stuff='0';
    
    while(stuff!='N'){
        cout << "are you user? Y/N: ";
        cin >> stuff;
        stuff = toupper(stuff);
        if (stuff == 'Y'){
            session.startSession();
        }
        else if (stuff == 'N') {
            lib.BMS();
        }
        else {
            cout << "invalid input. try again: ";
        }
        cin.clear();
        cin.ignore();
        cout << "continue? Y/N: ";
        cin >> stuff;
        stuff = toupper(stuff);
        if (stuff == 'Y'){
            cout << "continuing\n";
        }
        else if (stuff == 'N') {
            cout << "ending system\n";
        }
        else {
            cout << "invalid input. try again: ";
        }
        cin.clear();
        cin.ignore();
    }
    return 0;
}