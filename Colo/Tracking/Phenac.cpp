#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;
uint32_t LCG(uint32_t& seed){
  seed = seed * 214013 + 2531011;
  return seed;
}

uint32_t modpow32(uint32_t base, uint32_t exp)
{
  uint32_t result = 1;
  while (exp > 0)
  {
    if (exp & 1)
      result = result * base;
    base = base * base;
    exp >>= 1;
  }
  return result;
}

uint32_t LCGn(uint32_t seed, const uint32_t n)
  {
    uint32_t ex = n - 1;
    uint32_t q = 0x343fd;
    uint32_t factor = 1;
    uint32_t sum = 0;
    while (ex > 0)
    {
      if (!(ex & 1))
      {
        sum = sum + (factor * modpow32(q, ex));
        ex--;
      }
      factor *= (1 + q);
      q *= q;
      ex /= 2;
    }
    seed = (seed * modpow32(0x343fd, n)) + (sum + factor) * 0x269EC3;
    return seed;
  }

void printResults(uint32_t seed,int fcount, int i,bool stepFrame){
        cout << dec << setw(3) << i << ": "
         << hex << seed << " : "
         << dec << fcount + (stepFrame*2);
         if (stepFrame){
          cout << " : " << "STEP";
         } 
         cout << endl;
}

bool checkStepPath(vector<int>secondarySteps,uint32_t& seed,int i){
  if (binary_search(secondarySteps.begin(),secondarySteps.end(),i)){
        LCG(seed);
        LCG(seed);
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

    //config
    uint32_t seed = 0x52B59EFE; //INITIAL_SEED
    int region = 0; //0 ntsc-u, 1 Pal, 2 ntsc-j 
    int secondary = 0;//0 quil,1 croc 2 bayleef
    int framesToTrack = 350;
    bool trackSteps = 1;

    //starting from same position as loop
    vector<int>quilavaSteps{5,10,15,20,25,30,35,39,44,49,54,67,88,100,111,119,
    126,132,138,143,148,153,158,163,168,173,177,182,187,192,197,202,210,217,
    227,237,252,268,284,292,297,302,307,312,316,321,326,331};
    vector<int>croconawSteps{};
    vector<int>bayleefSteps{};
    vector<int>currentSecondary{};

    switch (secondary) {
      case 0 : 
      currentSecondary = quilavaSteps;
      break;
      case 1 : 
      currentSecondary = croconawSteps;
      break;
      case 2 : 
      currentSecondary = bayleefSteps;
      break;
    }
    seed = LCGn(seed,6);
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

  if (region == 0){ //restructure this for many patterns by having loop on the outside?
    cout << "NoAudioLagPattern - HHLHL" << endl;
    // cout << currentSecondary.at(1);
    for (int i = 1;i<framesToTrack;i++){
      
      stepFrame = checkStepPath(currentSecondary,seed,i);
      
      //initialNoAudio
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

        seed = LCGn(seed,fcount);
        printResults(seed,fcount,i,stepFrame);
    }  
  };


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