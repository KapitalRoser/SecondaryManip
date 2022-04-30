#include "../../processCore.h"


u16 getHiSeed(u32 seed){
    return seed >> 16;
}


//WE FOUND IT

//ON Frame 39, AFTER THE BACKGROUND AND BLINK HAS BEEN CALLED 
//Only 4 successes needed - 10 is a value set to widen the frame window to hit.
//WS_BALL_THROW in XD, so at 0x80213e94 I've labeled it COLO_BALL_THROW
//7 static calls occur just after A press happens. Like 4 frames later I think.
//Great ball is pretty strong.


int main(){
    u32 startingSeed = 0xA67F8881; //This version takes a seed and SCANS for the next window that meeds the breakout Attempts const. 
    const int BREAKOUT_ATTEMPTS = 10; //This must be at least 4, but can be as wide as you like. Higher number means less choices but easier to hit. 

    u32 seed = startingSeed;
    bool found = false;
    const int pokeBall = 47661; //64 longest
    const int greatBall = 52428; //96 longest avg 4 in a row. CORRECT LETS FUCKIN GO
    const int QUIL_CATCH_ODDS = pokeBall;


    int counter = 0; //nice name lol
    u32 infBlock = 0;
    while (counter < BREAKOUT_ATTEMPTS && infBlock < UINT32_MAX){ //prevent infinite loop??
    
    std::cout  << infBlock <<  " : " << "SEED: " << std::hex << seed << " : " << std::dec << getHiSeed(seed) 
    << " : " << (getHiSeed(seed) < QUIL_CATCH_ODDS); 
    
        if (getHiSeed(seed) < QUIL_CATCH_ODDS){
            //Consider it a ball shake
            counter++;
        } else {
           counter = 0; //consider it a breakout, start counter over
        }
    std::cout<< " : " << counter << "\n"; 
    LCG(seed);
    infBlock++;  
    }

    //found?
    if (counter >= BREAKOUT_ATTEMPTS){
        found = true;
        LCGn_BACK(seed,BREAKOUT_ATTEMPTS);
        std::cout << "found at: " << std::hex << seed << ", which is " << std::dec << findGap(startingSeed,seed,true) <<  "\n"; 
    }





    //SUPER INEFFICIENT WAY.


    return 0;
}