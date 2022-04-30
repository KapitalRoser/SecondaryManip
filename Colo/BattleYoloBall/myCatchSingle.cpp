#include "../../processCore.h"


u16 getHiSeed(u32 seed){
    return seed >> 16;
}

void displayInfo(u32 seed, int QUIL_CATCH_ODDS, int counter){
    std::cout << std::hex << seed << " : " << std::dec << getHiSeed(seed) << " : " << (getHiSeed(seed) < QUIL_CATCH_ODDS) << " : " << counter << "\n";
}

int main(){
    u32 startingSeed = 0xA67F8881; //Displays the result of a single seed for a catch check.
    u32 seed = LCG(startingSeed);
    const int pokeBall = 47661;
    const int greatBall = 52428; 
    const int QUIL_CATCH_ODDS = greatBall;
    int counter = 0;
    std::cout << "Entered: " << std::hex << startingSeed << std::dec << std::endl;
    while (getHiSeed(seed) < QUIL_CATCH_ODDS){
        counter++;
        LCG(seed);
        displayInfo(seed,QUIL_CATCH_ODDS,counter);
    }
    std::cout << "\nStreak: " << counter << "\n";
    if (counter >= 4){
        std::cout << "SUCCESS!";
    }

    return 0;
}