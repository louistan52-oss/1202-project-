#include "BMS.h"
#include "Users.h"
#include "Robot_Transport_System.h"

using namespace std;

SystemController sc;

int main(){
    srand(static_cast<unsigned int>(time(0)));
    //cout << "Initializing Robot Transport System... Please type Q when done:\n";
    //sc.run();
    USERS::program();
    return 0;
}
