#include "../processCore.h"
#include "NPCs/NPC.cpp"

enum bgFrames {H = 116, L = 76, A = 114, O = 154}; //High, Low, Alpha, Omega

int col_consultPattern(int i, region gameRegion){
    std::vector<int>NTSCUPattern = {H,H,L,H,L}; //HHLHL
    std::vector<int>PAL60Pattern = {
    A,H,
    H,L,H, O,L,
    H,H,L, O,H,
    L,H,L, O,H,
    H,L,H, O,L,
    H,L,H, O,H,
    L,H,L, O,H,
    L,H,H, O,L,
    H,L,H, O,L,
    H,H,L, O,H,
    L,H,H, A,H //Last A,H necessary?
    };
    std::vector<int>::iterator iter;
    int offset = 3; //const?
    int range = 0;
    switch (gameRegion)
    {
    case NTSCU:
        iter = NTSCUPattern.begin();
        range = NTSCUPattern.size();
        //Does this logic work for all patterns now?
        if (i < offset){
            iter += (i % offset);
        } else {
            iter += (i-offset) % range;
        }
        break;
    case PAL60:
        iter = PAL60Pattern.begin();
        range = PAL60Pattern.size();
        //logic
        break;
    case PAL50:
        //pattern
        //logic
        break;
    case NTSCJ:
        //pattern
        //logic
        break;
    default:
        break;
    }
    return *iter;
}

//slightly depreciated (currently just used in npcs.cpp):
bool col_CheckStepPath(std::vector<int>secondarySteps,u32& seed,int i,int stepCalls){
  if (binary_search(secondarySteps.begin(),secondarySteps.end(),i+1)){
        LCGn(seed,stepCalls);
        return true;
  } else {
        return false;
  }
}
int colo_RollBackground(u32 &seed,int i, region gameRegion){    
      return LCGn(seed,col_consultPattern(i,gameRegion));
}

void printResults(u32 seed,int fcount, int i,bool stepFrame,bool stepCalls){
        std::cout << std::dec << std::setw(3) << i+1 << ": "
        << std::setw(8) << std::hex << seed << " : "
         << std::dec << fcount + (stepFrame + !stepCalls);
         if (stepFrame){
          std::cout << " : " << "Player STEP";
         } 
         std::cout << std::endl;
}

void advanceFrame(u32 &seed,int currentFrame,std::vector<int>listOfSteps,region gameRegion,int stepCalls,std::vector<NPC>npcSet){
  //int rollsApplied = 0; 

  //rollsApplied += 
  colo_RollBackground(seed,currentFrame,gameRegion);
  
  bool stepFrame = 0;
  if (stepCalls > 0){
    stepFrame = col_CheckStepPath(listOfSteps,seed,currentFrame,stepCalls);   
  }

  std::string action = "";
  if (!npcSet.empty()){ 
    for(unsigned int j = 0; j < npcSet.size(); j++){
            action += npcSet[j].npcAction_Self(seed,currentFrame);
        }
  }
  
  //printResults(seed,rollsApplied,currentFrame,stepFrame,stepCalls); //debug bg noise.
}

void rollTransition(u32 &seed,int target,std::vector<int>secondarySteps,region gameRegion,std::vector<NPC>npcSet){
//Post-A press
    for (int i = target-1; i < target+4; i++) //5 more frames worth of calls happen after a press?
    {
      advanceFrame(seed,i,{},gameRegion,0,npcSet); //can npc movements happen during this? TBD.
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
    const int VISUAL_START_FRAME = 99755;
    const uint32_t INITIAL_SEED = 0x7B016A28; 
    const int SEARCH_WINDOW = 95; 

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
    NPC kaib    = NPC({  85, -150},"K");
    NPC jim     = NPC({  15,  -10},"J"); // -- fuck you jim, costs 5s to open door for him.
    NPC grandma = NPC({-140,  -10},"G");
    NPC boots   = NPC({  90,   90},"B");
    NPC randall = NPC({ -90,  110},"R"); // -- fuck you randall, stop doing weird things on the curve.
    NPC heels   = NPC({  30,  300},"H",SLOWER); // -- fuck you heels, for having a slightly slower speed.
    std::vector<NPC>npcSet = {kaib,jim,grandma,boots,randall,heels}; 
    //if not wanting to track NPCs, uncomment this line: 
    //npcSet.clear();
    
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
    //frame instance = {npcSet,secondarySteps,gameRegion};


    //Begin
    for (unsigned int i = 0;i < npcSet.size(); i++){
      npcSet[i].initializeNPC_Self(seed);
    }
    

    //Simulate up to target.
    for (int i = 0; i < target; i++)
    {
      //instance.updateFrame(seed,i); //Wow, just one line.
      advanceFrame(seed,i,secondarySteps,gameRegion,numCallsPerStep,npcSet);
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    std::cout << "Reached: " << std::hex << seed << " - " << std::dec <<visualTarget << std::endl;
    for (unsigned int i = target-1; i < SEARCH_WINDOW+target; i++)
    {
      visualTarget++;
      listSeed = seed;

      rollTransition(listSeed,target,secondarySteps,gameRegion,npcSet);
      
      int needToAdjust = col_consultPattern(i+4,gameRegion);
      if (needToAdjust == 76 && gameRegion == NTSCU){
        //cout << "Hit: Low  - ";
        LCGn(listSeed,38); //No idea what this is but it FOLLOWS THE HHLHL pattern! Wacky!
        LCGn(listSeed,2);
      } else {
        //cout << "Hit: High - ";
      }

      std::cout << visualTarget << " - r: " << std::hex << std::setw(8) << seed << " - "; 
      //replace with better generation function?
      generateMon(listSeed,QUIL_GENDER_RATIO);
    
      //next mon.
      advanceFrame(seed,i,secondarySteps,gameRegion,numCallsPerStep,npcSet);
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