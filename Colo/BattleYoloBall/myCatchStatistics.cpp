#include "../../processCore.h"


u16 getHiSeed(u32 seed){
    return seed >> 16;
}


int main(){
    u32 startingSeed = 0x0;
    const int BREAKOUT_ATTEMPTS = 10; //the real break out, not the 3 shakes you see in-game. 
    //THE LONGEST STREAK OF SEEDS WHICH PASS THE CONDITION AT THIS THRESHOLD IS 94
    
    u32 seed = startingSeed;
    bool found = false;
    const int pokeBall = 47661; //64 longest streak
    const int greatBall = 52428; //94 longest streak
    const int QUIL_CATCH_ODDS = greatBall;


    int counter = 0; //nice name lol
    u32 infBlock = 0;
    int longest = 0;
    std::vector<int> set;
    while (infBlock < 100000000){ //prevent infinite loop??
    
    //std::cout  << infBlock <<  " : " << "SEED: " << std::hex << seed << " : " << std::dec << getHiSeed(seed) << " : " << (getHiSeed(seed) < QUIL_CATCH_ODDS)  << "\n";  
    
        if (getHiSeed(seed) < QUIL_CATCH_ODDS){
            //Consider it a ball shake
            counter++;
        } else {
            if (counter > longest){
                longest = counter;

            }
            set.push_back(counter);
            
           counter = 0; //consider it a breakout, start counter over
        }
    LCG(seed);
    infBlock++;  
    }

    //found?
    // if (counter >= BREAKOUT_ATTEMPTS){
    //     found = true;
    //     LCGn_BACK(seed,BREAKOUT_ATTEMPTS);
    //     std::cout << "found at: " << std::hex << seed << ", which is " << std::dec << findGap(startingSeed,seed,true) <<  "\n"; 
    // }
    std::cout << "res: " << longest << "\nMedian: " << median(set) << "\nMean: ";
    int mean = 0;
    for (unsigned int i = 0; i < set.size(); i++)
    {
            mean += set[i];
    }
    mean = mean /set.size();
    std::cout << mean;


    //SUPER INEFFICIENT WAY.


    return 0;
}