#include "Robot_Transport_System.h"
#include <ctime>

int main() {
    srand(static_cast<unsigned int>(time(0))); // Seed for random start battery.
    SystemController sc;
    sc.run();
    return 0;
}