#include <iostream>
#include <iomanip>
#include <string>
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

uint32_t rollToGeneration(uint32_t seed, uint32_t n){
   //Pokeball animation calls, done at 72-32-32 pattern for 11 frames.
    return seed = LCGn(seed,n);
}
void findQuil(uint32_t inputSeed){
//   uint32_t TID = 0;
  uint32_t PID = 0;
  uint32_t seed = inputSeed;
  uint32_t outSeed = 0;
  string nature = " ";
  string hiddenPowerType = " ";
  int hiddenPowerStrength = 0;
  

//   XD:
//   const uint32_t blurCalls = 9600;
//   const uint32_t Frame44 = 432000;
//   string hiOrLow = " ";
  //Nature Array:
    const string naturesList[25] = {"Hardy","Lonely","Brave","Adamant","Naughty","Bold","Docile","Relaxed",
    "Impish","Lax","Timid","Hasty","Serious","Jolly","Naive","Modest","Mild","Quiet","Bashful",
    "Rash","Calm","Gentle","Sassy","Careful","Quirky"};
  
    
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

   

    //Some tid/sid stuff?
    seed = LCGn(seed,4);



    outSeed = seed;

    //Would be High bits/Secret ID, used for Shiny.

    // LCG(seed); These do not appear on shadow generation.
    // LCG(seed); //Dummy ID, leftover, no connection to other parts of program except for the impact on rng.

    //IVS
    LCG(seed);
    int hp = (seed >> 16) & 31;
    int atk = (seed >> 21) & 31;
    int def = (seed >> 26) & 31; 
 
    LCG(seed); 
    int spe = (seed >> 16) & 31;
    int spa = (seed >> 21) & 31;
    int spd = (seed >> 26) & 31; 

    
    LCG(seed); //Ability call
    //PID STUFF:
    uint32_t hId = LCG(seed) >> 16;
    uint32_t lId = LCG(seed) >> 16;
    PID = (hId << 16) | (lId);
    string displayNature = naturesList[PID % 25];


    cout
    << "Seed " 
    //<< hiOrLow << setw(4) 
    << ": " << hex << outSeed
    << " PID: " << setw(5) << PID
    << " IVs: " << setw(2) << hp << " " << setw(2) << atk << " " << setw(2) << def << " " << setw(2)
    << spa << " " << setw(2) << spd << " " << setw(2) << spe << " "
    << "Nature: " << displayNature << endl;
}



int main(){
    uint32_t userInputSeed = 0xE1FE660D;
    userInputSeed = rollToGeneration(userInputSeed,544);
    findQuil(userInputSeed);

    return 0;
}

