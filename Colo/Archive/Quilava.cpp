#include <iostream>
#include <iomanip>
#include <string>

#include "../processCore.h"

using namespace std;

u32 rollTransition(u32 seed, u32 n){
   //Pokeball animation calls, done at 72-32-32 pattern for 11 frames.
   //only need to seperate these calls if npcs can interrupt 
   //Currently I don't think they can.
    return LCGn(seed,n);
}
void seekTarget(){

}
int main(){



    
    const int QUIL_GENDER_RATIO = 0x1F;
    uint32_t userInputSeed = 0x5C60397F;
    userInputSeed = rollTransition(userInputSeed,480); //batch calls.
    generateMon(userInputSeed,QUIL_GENDER_RATIO);
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