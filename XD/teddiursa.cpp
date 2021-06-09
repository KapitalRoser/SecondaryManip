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

void blurDirection(uint32_t seed){
  //burns 2 calls.
    uint32_t leftRightHi = 0;
    uint32_t leftRightLo = 0;
    uint32_t leftRight = 0;
    string blurDirection = "";
    LCG(seed);
    leftRightLo = seed >> 16;
    LCG(seed);
    leftRightHi = seed >> 16;
    leftRight = (leftRightHi << 16) | leftRightLo;
    if (leftRight% 2 == 1){
        blurDirection = "CounterClockwise - 1";
    } else {
        blurDirection = "ClockWise - 0";
    }
    //replace with return statement later.
    cout << blurDirection << endl;
}
void rollToGeneration(uint32_t&seed, uint32_t x, bool early){
   

  const uint32_t blurCalls = 9600;
  string version = "Modern";
  int n = 0;

    //THE HOLY EQUATION for post txtbox. 
  if(version == "LUA"){

  n = 9600*41 + 249 + x;

  } else if (version == "Modern"){

    n = blurCalls*48;

    if (early){ //Some kind of relationship between the early blur frame beginning and the hiFrame loFrame problem. Not just a houndour thing.
      n += blurCalls + 1;
    }

    n += 181 + 2 + x; //The 181 may be variable here. 
  }
  seed = LCGn(seed,n);










}


void generateMon(uint32_t inputSeed){
//   uint32_t TID = 0;
  uint32_t PID = 0;
  uint32_t seed = inputSeed;
  uint32_t outSeed = 0;
  string nature = " ";
  string hiddenPowerType = " ";
  int hiddenPowerStrength = 0;
  int genderRatio = 0x7f;
  const string naturesList[25] = {"Hardy","Lonely","Brave","Adamant","Naughty","Bold","Docile","Relaxed",
    "Impish","Lax","Timid","Hasty","Serious","Jolly","Naive","Modest","Mild","Quiet","Bashful",
    "Rash","Calm","Gentle","Sassy","Careful","Quirky"};



//   XD:
  
  const uint32_t clockWise = 36; //F at 30fps blur duration.
  const uint32_t counterClockWise = 30;




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
    bool pidGender = genderRatio > (PID & 0xFF) ? 1 : 0;
    string displayGender;
    if (pidGender){
        displayGender = "Female";
    } else {
        displayGender = "Male";
    }
    cout << "Seed " 
    //<< hiOrLow << setw(4) 
    << ": " << hex << outSeed
    << " PID: " << setw(5) << PID
    << " IVs: " << setw(2) << hp << " " << setw(2) << atk << " " << setw(2) << def << " " << setw(2)
    << spa << " " << setw(2) << spd << " " << setw(2) << spe << " "
    << "Nature: " << displayNature << " Gender: " << displayGender << endl;
}



int main(){
    uint32_t blurSeed = 0x0; //Final normal seed -1 frame. Can be stored or can be determined from an XDLNGR operation if I ever figure out how that works. 
    uint32_t userInputSeed = 0x3C3D8EF5; //Final normal seed.
    bool early = 0;
    blurDirection(blurSeed);
    rollToGeneration(userInputSeed,1,early);
    generateMon(userInputSeed);






    return 0;
}
/*
There is a slight re-interpretation of the calls depending on how I retrieve them from the game. Manually I get a nice even distribution. Automatically, much more chaotic. 

The generation process appears slightly different depending on if the textbox has finished printing or not.
If a is pressed on the first available frame, it is buffered
Then there are 3 frames before the textbox concludes, they all have similar lengths. 


in modern emu, fadeout lasts for 16f instead of 15. 
Difference in initial set of extra calls.
*/

  //This formula applies starting on the first divergent frame (rotation is determined 1 frame earlier). Registered a press is followed by 15 frames of fadeout using the main pattern, then this begins.
  //Figure out clockwise/Counterclockwise determinism OR track it like regular rng calls. 18th frame on modern is where fadeout ends. 
  //ClockWise or CounterClockwise is determined on frame 17 (modern) or frame 16 (past). DB1A2A3C on manually found.
  //X = This is dependent on rng, but how. 

