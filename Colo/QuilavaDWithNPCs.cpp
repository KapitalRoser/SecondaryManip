#include "../processCore.h"
#include "../NPC.h"
#include "frame.h"


void printResults(u32 seed,int fcount, int i,bool stepFrame,bool stepCalls){
        std::cout << std::dec << std::setw(3) << i+1 << ": "
        << std::setw(8) << std::hex << seed << " : "
         << std::dec << fcount + (stepFrame + !stepCalls);
         if (stepFrame){
          std::cout << " : " << "Player STEP";
         } 
         std::cout << std::endl;
}

void advanceFrame(u32 &seed,int currentFrame, bool trackSteps, bool trackNPCs,std::vector<int>listOfSteps,region gameRegion,int stepCalls){
  int rollsApplied = 0; 
  //refers to all the logic involved in handling a visual frame, not an rng frame.
  //Still not sure on the exact ordering of these things, wouldn't matter if not for the NPCs...
  if (trackNPCs){ //probably gonna need a lot more params for the NPCs.
    // handleNPCs();
    if (currentFrame == 0){
      LCGn(seed,6); //initial npc movements. 
    rollsApplied += 6;
    }
  }
  bool stepFrame = 0;
  if (trackSteps){
    stepFrame = col_CheckStepPath(listOfSteps,seed,currentFrame,stepCalls);   
  }
  
  rollsApplied += colo_RollBackground(seed,currentFrame,gameRegion);
  //printResults(seed,rollsApplied,currentFrame,stepFrame,stepCalls); //debug bg noise.
}

void rollTransition(u32 &seed,int target,std::vector<int>secondarySteps,region gameRegion){
//Post-A press
    for (int i = target-1; i < target+4; i++) //5 more frames worth of calls happen after a press?
    {
      advanceFrame(seed,i,false,false,secondarySteps,gameRegion,false); //can npc movements happen during this? TBD.
    }

    LCGn(seed,38); //what does this do?
    
    LCGn(seed,448); //72-32-32 X2 + 32 + 72, pretty reliable.
    LCGn(seed,4); //generation related? Shiny related? another reason why maybe I shouldn't have generateMon in the header.

  
   //Pokeball animation calls, done at 72-32-32 pattern for 11 frames.
   //only need to seperate these calls if npcs can interrupt 
   //Currently I don't think they can.
}


int main(){
  //~~~~~~~~~~~~ CONFIG INPUTS ~~~~~~~~~~~~~~~~~~~~
    int target = 0; //+441.
    coloSecondary pokemon = QUILAVA;
    region gameRegion = NTSCU;
    const int VISUAL_START_FRAME = 99755; //99757 for weird Seed
    const uint32_t INITIAL_SEED = 0x7B016A28; //LONG, PRE-6 SEED.
    const int SEARCH_WINDOW = 95; //optional, default 10
    // bool trackSteps = true;
    // bool trackNPCs = true;
    int numCallsPerStep = 2;

    const int QUIL_GENDER_RATIO = 0x1F;
    
    const int MIN_FRAMES_BEFORE_A_PRESS = 440; //440 Assuming standard buffered movement. (down-right -> right).
    int visualTarget = target+VISUAL_START_FRAME;
    target += MIN_FRAMES_BEFORE_A_PRESS;

    // //unknown if these are needed yet:
    // int cameraAngleCurrent = 0;
    // int cameraAnglePrevious = 5;
    uint32_t listSeed = 0;
    uint32_t seed = INITIAL_SEED;



    //PHENAC NPCS LOAD IN THIS ORDER:
    NPC kaib    = NPC({  85, -150} ,STANDARD,"K");
    NPC jim     = NPC({  15,  -10}, STANDARD,"J"); // -- fuck you jim, costs 5s to open door for him.
    NPC grandma = NPC({-140,  -10}, STANDARD,"G");
    NPC boots   = NPC({  90,   90}, STANDARD,"B");
    NPC randall = NPC({ -90,  110}, STANDARD,"R"); 
    NPC heels   = NPC({  30,  300},   SLOWER,"H");
    std::vector<NPC>npcSet = {kaib,jim,grandma,boots,randall,heels}; 
    
    //starting from same position as loop
    std::vector<int>quilavaSteps{5,10,15,20,25,30,35,39,44,49,54,67,88,100,111,119,
    126,132,138,143,148,153,158,163,168,173,177,182,187,192,197,202,210,217,
    227,237,252,268,284,292,297,302,307,312,316,321,326,331};
    std::vector<int>croconawSteps{};
    std::vector<int>bayleefSteps{};
    std::vector<int>secondarySteps{};
    switch (pokemon) {
      case QUILAVA : 
      secondarySteps = quilavaSteps;
      break;
      case CROCONAW : 
      secondarySteps = croconawSteps;
      break;
      case BAYLEEF : 
      secondarySteps = bayleefSteps;
      break;
    }

    //Set different frames if not wanting to track npcs?
    frame instance = {npcSet,secondarySteps,gameRegion};


    
    
    //Simulate up to target.
    for (int i = 0; i < target; i++)
    {
      std::cout << i <<": MADE IT HERE!\n"; //crashing after 34 lines? what's happening?
      //do we need to adjust how npcs are handled? like are they correctly being updated or is something getting re-copied?
      instance.updateFrame(seed,i); //Wow, just one line.
    }
    // Do debug statements here. 
    // Reached! blah blah blah.
    

    std::cout << "Reached: " << std::hex << seed << " - " << std::dec <<visualTarget << std::endl;
    for (unsigned int i = target-1; i < SEARCH_WINDOW+target; i++)
    {
      visualTarget++;
      listSeed = seed;
      rollTransition(listSeed,target,secondarySteps,gameRegion);
      int needToAdjust = col_consultPattern(i+4,gameRegion);
      if (needToAdjust == 76 && gameRegion == NTSCU){
        //cout << "Hit: Low  - ";
        LCGn(listSeed,38); //No idea what this is but it FOLLOWS THE HHLHL pattern! Wacky!
        LCGn(listSeed,2);
      } else {
        //cout << "Hit: High - ";
      }
      std::cout << visualTarget << " - Reached: " << std::hex << std::setw(8) << seed << " - "; 
      generateMon(listSeed,QUIL_GENDER_RATIO);
      
      advanceFrame(seed,i,false,false,secondarySteps,gameRegion,numCallsPerStep);
    }
    

    
    //generateMon(seed,QUIL_GENDER_RATIO);
    return 0;
}

/*
MANIP FLOW



1. BLINK
2. Steps to BG
  2.5 initial NPC movements

3. MAIN FRAME LOOP
    On each frame:
    Roll background
    Update NPCs (dme seed is after this step)
    Apply player step if applicable.
    Repeat.

4. SWIRL ANIMATION

5. YOLOBALL

*/