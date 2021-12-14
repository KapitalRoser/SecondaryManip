#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>

#include "../../processCore.h"
using namespace std;

void printResults(uint32_t seed,int fcount, int i,bool stepFrame){
        cout << dec << setw(3) << i << ": "
        << setw(8) << hex << seed << " : "
         << dec << fcount + (stepFrame*2);
         if (stepFrame){
          cout << " : " << "STEP";
         } 
         cout << endl;
}
bool checkStepPath(vector<int>secondarySteps,uint32_t& seed,int i){
  if (binary_search(secondarySteps.begin(),secondarySteps.end(),i)){
        LCGn(seed,2);
        return true;
  } else {
        return false;
  }
}

int main(){
    const int hiFrame = 116;
    const int loFrame = 76;

    const int xFrame = 114;
    const int yFrame = 154;

    int fcount = 0;
    bool stepFrame = 0;

    //~~~~~~~~~~~~ CONFIG INPUTS ~~~~~~~~~~~~~~~~~~~~
    uint32_t seed = 0x4D43A86F; //INITIAL_SEED
    region gameRegion = NTSCU; 
    coloSecondary chosenSecondary = QUILAVA;
    int framesToTrack = 400;
    bool trackSteps = true;
    bool trackNPCs = true;
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    //starting from same position as loop
    vector<int>quilavaSteps{5,10,15,20,25,30,35,39,44,49,54,67,88,100,111,119,
    126,132,138,143,148,153,158,163,168,173,177,182,187,192,197,202,210,217,
    227,237,252,268,284,292,297,302,307,312,316,321,326,331};
    vector<int>croconawSteps{};
    vector<int>bayleefSteps{};
    vector<int>secondarySteps{};

    switch (chosenSecondary) {
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

    if (trackNPCs){
      seed = LCGn(seed,6);
    }
    
    //Initial NPC movements

    /*In Pause menu mode:
    hiFrame = 38;
    loFrame = 78;
    I starts on 0;
    no npc movements if doing pause mode.
    bad idea since npc walking cycles do not reset from resuming, 
    even if noise pattern is the same*/

    //Pokeball Animation:
    // hiFrame = 32;
    // loFrame = 72;
    //i starts on 0

    //These are in addition to above:
    //exiting from an exit with different music (pokecenter and pregym).
    // seed = LCGn(seed,2);
    // seed = LCGn(seed,loFrame);

  if (gameRegion == NTSCU){ //restructure this for many patterns by having loop on the outside?
    cout << "NoAudioLagPattern - HHLHL" << endl;
    // cout << secondarySteps.at(1);
    for (int i = 1;i<framesToTrack;i++){
      
      if (trackSteps){
        stepFrame = checkStepPath(secondarySteps,seed,i);
      }
      
      //initialNoAudio pre-pattern
      if (i<=3 && i % 3 == 0){
        fcount = loFrame;
      } else {
        fcount = hiFrame;
      }
       //MAIN NOISE PATTERN
      if (i>3){
        if (i % 5 == 1 || i % 5 == 3){
            fcount = loFrame;
        } else {
            fcount = hiFrame;
        }
      }
      LCGn(seed,fcount);
      printResults(seed,fcount,i,stepFrame);
    }  
  };

// //PAL PATTERN:
// } else {
//     cout << "ComplexPalPattern60" << endl;  
//     for (int i = 0;i<30;i++){
//         /*XH
//           HLH YL
//           HHL YH
//           LHL YH
//           HLH YL
//           HLH YH
//           LHL YH
//           LHH YL
//           HLH YL
//           HHL YH
//           LHH XH
//         */
//         seed = LCGn(seed,fcount);
//         printResults(seed,fcount,i);
//     }

// }
    

    return 0;
}