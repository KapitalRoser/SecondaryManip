#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
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
uint32_t LCGPercentage(uint32_t& seed){
  float percentResult = 0;
  uint32_t hiSeed = 0;
  LCG(seed);
  hiSeed = seed >> 16;
  percentResult = hiSeed/65536;
  return percentResult;
}
vector<int> readNumbersFromFile(string fileName)
{
    uint32_t value;
    int lineRead = 0;
    vector<int> data; //Setup
    ifstream file(fileName);
            cout << "Read some data! \n";
    if (file.fail())
    {
        cout << "File inaccessible";
        exit(EXIT_FAILURE);
    }
    while (!(file.fail()))
    {   /*Keeping the pattern in raw ints of rolls is a bigger filesize than
          encoding everything as individual chars. However this makes it easy to import
          patterns with odd or new values */
        file >> lineRead; 
        data.push_back(lineRead);
    }
    file.close();
    return data;
}
void exhaustGuaranteed(string pattern,uint32_t& seed,vector<int>vPattern){
  //Is it better to have a list of these predetermined values
  //Or do I pass in the vector of values an sum the calls?
  //would make it easier to accept new or modified patterns
  //The code would verify the pattern automatically.
  int n = 0;
    if (pattern == "leadingAs"){
      n = 61386; // == 36ee3742
    } else if (pattern == "leadingSevens"){
      // n = 61237; == 70350767 frame, too early.
      n = 61443;
      //gets us 3b7a8b55 == 61443 from 0
    } else if (pattern == "leadingEights"){
      n = 61511; // == E2550DE9
    }
    seed = LCGn(seed,n);

    //REMEMBER TO TEST THIS AND REPLACE!!!
    //vs this: (slightly off?)
    // const int minFrames = 764; //Works for any pattern, the condition statement may be off though. 
    // for (int i = 0; i<(minFrames-14);i++){
    //   seed = LCGn(seed,vPattern.at(i));
    // }

    //exhaust guaranteed calls according to previous pattern tests. Any deviations work themselves out by the time all footsteps are completed.
}
uint32_t seekTarget(uint32_t& seed,int target,vector<int>pattern){
    for (int i = 0;i<target;i++){ // Seek target
      seed = LCGn(seed,pattern.at(i));
    }
    return seed;
}
int seekFrame(int startF,int minF, int target){
  return startF+minF+target*2; //60 fps to 30fp conversion.
}
void blurDirection(uint32_t& seed){
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
    if (leftRight % 2 == 1){ //literally just even or odd
        blurDirection = "Clockwise - 1"; 
    } else {
        blurDirection = "CounterClockWise - 0";
    }
    //replace with return statement later.
    cout << blurDirection << endl;
}
void selectBlurStyle(uint32_t& seed){
  //similar process to blurDirection, will only fill this out if there's demand for it.
  LCG(seed);
  LCG(seed);
}
int dummyCam(uint32_t& seed){
  uint32_t v1 = 0;
  int count = 0;
  int seedPercent = 0;
  /*This function is WEIRD because after it determines new default battle camera
  It uses the result of the rng math do jump somewhere different in the code!!*/

  do{
    count++;
    LCG(seed);
    v1 = seed >> 16;
    v1 = v1 % 10; //is this ever % 12? even if so, v1 cant be 12 or greater.
    cout << "v1 " << v1 << endl;
   } while(v1 == 3 || v1 >= 10); //What to do if v1 == or >= 11?

  LCG(seed);
    if (v1 == 8){
      seed = LCGn(seed, 4); //LCGPercentages that lead to branches, but dont affect # of calls.
   } else if (v1 >= 5 && v1 < 10){
    if (LCGPercentage(seed) >= 0.5){ //Why they do this, idk - burns a call here too.
      LCG(seed);
    }
   }
  

    //Some physics stuff
    // LCG(seed);
    return count+1;
}
void rollToGeneration(uint32_t&seed,int blurDuration, int camFrame){ 
  const int BLUR_CALLS = 9600;
  int battleCameraCalls = 0;
  // string version = "Modern";
  
    //THE HOLY LOOP 
    for (int i = 0; i<blurDuration; i++){
        seed = LCGn(seed,BLUR_CALLS);

        // if (i == 1){
        //   cout << "PostOverlap: " << hex << seed << dec << endl;
        // }
        // cout << hex << seed << endl;
        if (i == camFrame){ //is this constant?
          cout << "Seed entering dummycam: " << hex << seed << dec << endl;
          battleCameraCalls = dummyCam(seed);
          // cout << "CAM: " << battleCameraCalls << endl; 
        }
    }

  // seed = LCGn(seed,blurBeforeCam*BLUR_CALLS);
  // battleCameraCalls = dummyCam(seed);
  //  cout << "BattleCamCalls " << battleCameraCalls << endl;
  // seed = LCGn(seed,(blurAfterCam+battleCameraCalls)*BLUR_CALLS);
  // n = blurDuration*BLUR_CALLS + dummyCam(seed) + 4; //is 4 const?
  // seed = LCGn(seed,n);
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
    << " PID: " << setw(5) << PID << dec
    << " IVs: " << setw(2) << hp << " " << setw(2) << atk << " " << setw(2) << def << " " << setw(2)
    << spa << " " << setw(2) << spd << " " << setw(2) << spe << " "
    << "Nature: " << displayNature << " Gender: " << displayGender << endl;
}



int main(){
  //~~~~~~~~~~~~ CONFIG INPUTS ~~~~~~~~~~~~~~~~~~~~
    
    int target = 111; //151
    
    int aToBlur = 21; //17,19,20
    int blurDuration = 48; //46,48,49 -- 47?
    int camFrame = blurDuration - 9; //9,8

    const string PATTERN = "leadingEights"; //Pattern - step calls built in.
    const int VISUAL_START_FRAME = 38231;
    const uint32_t INITIAL_SEED = 0x0;
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    const int VISUAL_MIN_FRAMES = 1531; //60fps, adjust for not -5?
    const string FILE_EXTENSION = ".txt";
    int patternPosition = target;
       
    uint32_t seed = INITIAL_SEED;
    uint32_t blurSeed = 0;
    uint32_t sim = 0; //The last frame of overlap
    // vector<uint32_t>LastNormal;

    vector<int>noisePattern = readNumbersFromFile(PATTERN + FILE_EXTENSION);
    int targetVisualFrame = seekFrame(VISUAL_START_FRAME,VISUAL_MIN_FRAMES,target);
    exhaustGuaranteed(PATTERN,seed,noisePattern); //test alt solution and put this into seekTarget()
    uint32_t targetValue = seekTarget(seed,target,noisePattern);


    for (int i = 0;i<aToBlur;i++){
      seed = LCGn(seed,noisePattern.at(patternPosition+i));
      if (i == aToBlur-3){
        blurSeed = seed;
        //The following are 2 calls each
        blurDirection(seed);
        selectBlurStyle(seed);
      }
      if (i == aToBlur-1){
        // This if statement is only for visual debugging.
        sim = seed;
        sim = LCGn(sim,2*9600);
      }
    }



    //Printouts:
    cout<< "Seek frame: " << targetVisualFrame << endl
        << "Target: " << target << ". Reached: " << hex << targetValue << endl
        << "Blur Frame: " << blurSeed << endl
        << "Frames used for calculation: " << sim
        << endl; 

    //Find the bear.
    rollToGeneration(seed,blurDuration,camFrame);
    generateMon(seed);


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

  /*SOB pattern consistent at 19/48/9? 151-155. Also @ 200
    Alt: 19/46/9 156
    20/46/9 157-165+

    20/47/8 -WoW - 235

      SBO
      ACB
      19/49/9
      20/48/9
      20/48/8
      ABC
      19/48/8
      EOC
      ECO

      20/46/8


      ALL COMBOS:
      0  17/46/8
      1  17/46/9
      2  17/48/8
      3  17/48/9
      4  17/49/8
      5  17/49/9
      6  19/46/8
      7  19/46/9 -- seen
      8  19/48/8 -- seen
      9  19/48/9
      10 19/49/8
      11 19/49/9 -- seen
      12 20/46/8 -- seen
      13 20/46/9 -- seen
      14 20/48/8 -- seen
      15 20/48/9 -- seen
      16 20/49/8
      17 20/49/9

      Frame lengths independent of rng seed. Will try to find, otherwise need to record with scripts. 
      */
    
