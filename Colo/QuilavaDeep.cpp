#include <iostream>
#include <iomanip>
#include <string>

#include "../processCore.h"

using namespace std;
void printResults(uint32_t seed,int fcount, int i,bool stepFrame,bool halfSteps){
        std::cout << dec << setw(3) << i+1 << ": "
        << setw(8) << hex << seed << " : "
         << dec << fcount + (stepFrame + !halfSteps);
         if (stepFrame){
          std::cout << " : " << "STEP";
         } 
         std::cout << endl;
}
bool checkStepPath(vector<int>secondarySteps,uint32_t& seed,int i,bool halfSteps){
  if (binary_search(secondarySteps.begin(),secondarySteps.end(),i+1)){
        if (halfSteps){
          LCGn(seed,1);
        } else {
          LCGn(seed,2);
        }
        return true;
  } else {
        return false;
  }
}
int rollBackground(u32 &seed,int i, region gameRegion){    
      //initialNoAudio pre-pattern
      int fcount = 0;
      const int loFrame = 76;
      const int hiFrame = 116;

      const int xframe = 114;
      const int yframe = 154;

      if (gameRegion == USA){
        //PRE-PATTERN
        if (i<=2 && i % 3 == 2){
          fcount = loFrame;
        } else {
          fcount = hiFrame;
        }
        //MAIN NOISE PATTERN
        if (i>3){
          if (i % 5 == 0 || i % 5 == 2){
              fcount = loFrame;
          } else {
              fcount = hiFrame;
          }
        }
      }
      LCGn(seed,fcount); //application of rules.
      return fcount;
}
void advanceFrame(u32 &seed,int currentFrame, bool trackSteps, bool trackNPCs,vector<int>listOfSteps, 
region gameRegion,bool halfSteps){
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
    stepFrame = checkStepPath(listOfSteps,seed,currentFrame,halfSteps);   
  }
  
  rollsApplied += rollBackground(seed,currentFrame,gameRegion);
  //printResults(seed,rollsApplied,currentFrame,stepFrame,halfSteps);
}
void rollTransition(u32 &seed,int target,vector<int>secondarySteps,region gameRegion){
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
// void updatePreamble (int vStartF, int vMinF, int target,uint32_t& seed,uint32_t targetValue){

//     int targetVisualFrame = seekFrame(vStartF,vMinF,target);
//     cout<< "Seek frame: " << targetVisualFrame << endl
//         << "GENERATED:"  << endl;
// }

int main(){
  //~~~~~~~~~~~~ CONFIG INPUTS ~~~~~~~~~~~~~~~~~~~~
    int target = 0; //+441.
    coloSecondary pokemon = QUILAVA;
    region gameRegion = USA;
    const int VISUAL_START_FRAME = 99757; //99757 for weird Seed
    const uint32_t INITIAL_SEED = 0x7B016A28; //LONG, PRE-6 SEED.
    const int SEARCH_WINDOW = 49; //optional, default 10
    bool trackSteps = true;
    bool trackNPCs = true;
    bool weirdHalfStep = true; //why tho? rare?
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    const int QUIL_GENDER_RATIO = 0x1F;
    
    const int MIN_FRAMES_BEFORE_A_PRESS = 440; //Assuming standard buffered movement. (down-right -> right).
    int visualTarget = target+VISUAL_START_FRAME;
    target += MIN_FRAMES_BEFORE_A_PRESS;

    // //unknown if these are needed yet:
    // int cameraAngleCurrent = 0;
    // int cameraAnglePrevious = 5;
    uint32_t listSeed = 0;
    uint32_t seed = INITIAL_SEED;

    //starting from same position as loop
    vector<int>quilavaSteps{5,10,15,20,25,30,35,39,44,49,54,67,88,100,111,119,
    126,132,138,143,148,153,158,163,168,173,177,182,187,192,197,202,210,217,
    227,237,252,268,284,292,297,302,307,312,316,321,326,331};
    vector<int>croconawSteps{};
    vector<int>bayleefSteps{};
    vector<int>secondarySteps{};

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

    //Simulate up to target.
    for (int i = 0; i < target; i++)
    {
      advanceFrame(seed,i,trackSteps,trackNPCs,secondarySteps,gameRegion,weirdHalfStep);
    }
    //Do debug statements here. 
    //Reached! blah blah blah.
    cout << "Reached: " << hex << seed << " - " << dec <<visualTarget << std::endl;
    for (unsigned int i = target-1; i < SEARCH_WINDOW+target; i++)
    {
      listSeed = seed;
      rollTransition(listSeed,target,secondarySteps,gameRegion);
      if (i == target+1 || i == target + 3){
        LCGn(listSeed,38); //No idea what this is but it FOLLOWS THE HHLHL pattern! Wacky!
        LCGn(listSeed,2);
      }
      generateMon(listSeed,QUIL_GENDER_RATIO);
      
      advanceFrame(seed,i,trackSteps,trackNPCs,secondarySteps,gameRegion,weirdHalfStep);
    }
    

    
    //generateMon(seed,QUIL_GENDER_RATIO);
    return 0;
}



// Why was this included in my generation code? Was this from early xd tests???
  //Roll load from continue
//   seed = LCGn(seed, 24);
//   cout << "seed at continue: " << hex << seed << " ";

//     seed = LCGn(seed,Frame44);

//   if (extraFrame){
//     seed = LCGn(seed, 9600);
//     if (!mainMenuSeed){
//       LCG(seed);
//     }
//     hiOrLow = "HIGH";
//   } else {
//     hiOrLow = "LOW";
//   }
    // seed = LCGn(seed,6);
    // TID = LCG(seed) >> 16; //YAY FIGURED IT OUT
    // seed = LCGn(seed,3);

// void generateMon(uint32_t inputSeed){
// //   uint32_t TID = 0;
//   uint32_t PID = 0;
//   uint32_t seed = inputSeed;
//   uint32_t outSeed = 0;
//   string nature = " ";
//   string hiddenPowerType = " ";
//   int hiddenPowerStrength = 0;
//   int genderRatio = 0x7f; //same ratio for both quil and teddy, nice.
//   const string naturesList[25] = {"Hardy","Lonely","Brave","Adamant","Naughty","Bold","Docile","Relaxed",
//     "Impish","Lax","Timid","Hasty","Serious","Jolly","Naive","Modest","Mild","Quiet","Bashful",
//     "Rash","Calm","Gentle","Sassy","Careful","Quirky"};
//   cout << left;

//     //Some tid/sid stuff?
//     seed = LCGn(seed,4);

//     outSeed = seed;
//     //Would be High bits/Secret ID, used for Shiny.

//     // LCG(seed); These do not appear on shadow generation.
//     // LCG(seed); //originally from Dummy ID, leftover.

//     //IVS
//     LCG(seed);
//     int hp = (seed >> 16) & 31;
//     int atk = (seed >> 21) & 31;
//     int def = (seed >> 26) & 31; 
 
//     LCG(seed); 
//     int spe = (seed >> 16) & 31;
//     int spa = (seed >> 21) & 31;
//     int spd = (seed >> 26) & 31; 

    
//     LCG(seed); //Ability call
//     //PID STUFF:
//     uint32_t hId = LCG(seed) >> 16;
//     uint32_t lId = LCG(seed) >> 16;
//     PID = (hId << 16) | (lId);
//     string displayNature = naturesList[PID % 25];
//     bool pidGender = genderRatio > (PID & 0xFF) ? 1 : 0;
//     string displayGender;
//     if (pidGender){
//         displayGender = "Female";
//     } else {
//         displayGender = "Male";
//     }
//     cout << "Seed" 
//     //<< hiOrLow << setw(4) 
//     << ": " << hex << setw(8) << outSeed
//     << " PID: " << setw(8) << PID << dec
//     << "  " << setw(2) << hp << " " << setw(2) << atk << " " << setw(2) << def << " " << setw(2)
//     << spa << " " << setw(2) << spd << " " << setw(2) << spe << "  "
//     << setw(7) << displayNature << "  " << displayGender << endl;
// }


//Backup quil generation
// void findQuil(uint32_t inputSeed){
// //   uint32_t TID = 0;
//   uint32_t PID = 0;
//   uint32_t seed = inputSeed;
//   uint32_t outSeed = 0;
//   string nature = " ";
//   string hiddenPowerType = " ";
//   int hiddenPowerStrength = 0;
  
//   //Nature Array:
//     const string naturesList[25] = {"Hardy","Lonely","Brave","Adamant","Naughty","Bold","Docile","Relaxed",
//     "Impish","Lax","Timid","Hasty","Serious","Jolly","Naive","Modest","Mild","Quiet","Bashful",
//     "Rash","Calm","Gentle","Sassy","Careful","Quirky"};
  
//     //Some tid/sid stuff?
//     seed = LCGn(seed,4);



//     outSeed = seed;

//     //Would be High bits/Secret ID, used for Shiny.

//     // LCG(seed); These do not appear on shadow generation.
//     // LCG(seed); //Dummy ID, leftover, no connection to other parts of program except for the impact on rng.

//     //IVS
//     LCG(seed);
//     int hp = (seed >> 16) & 31;
//     int atk = (seed >> 21) & 31;
//     int def = (seed >> 26) & 31; 
 
//     LCG(seed); 
//     int spe = (seed >> 16) & 31;
//     int spa = (seed >> 21) & 31;
//     int spd = (seed >> 26) & 31; 

    
//     LCG(seed); //Ability call
//     //PID STUFF:
//     uint32_t hId = LCG(seed) >> 16;
//     uint32_t lId = LCG(seed) >> 16;
//     PID = (hId << 16) | (lId);
//     string displayNature = naturesList[PID % 25];

//     cout
//     << "Seed " 
//     //<< hiOrLow << setw(4) 
//     << ": " << hex << outSeed
//     << " PID: " << setw(5) << PID
//     << " IVs: " << setw(2) << hp << " " << setw(2) << atk << " " << setw(2) << def << " " << setw(2)
//     << spa << " " << setw(2) << spd << " " << setw(2) << spe << " "
//     << "Nature: " << displayNature << endl;
// }