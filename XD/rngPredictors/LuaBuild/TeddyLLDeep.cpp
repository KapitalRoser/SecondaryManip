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
float LCGPercentage(uint32_t& seed){
  float percentResult = 0;
  uint32_t hiSeed = 0;
  LCG(seed);
  hiSeed = seed >> 16;
  percentResult = static_cast<float>(hiSeed)/65536;
  return percentResult;
}
vector<int> readNumbersFromFile(string fileName)
{
    uint32_t value;
    int lineRead = 0;
    vector<int> data; //Setup
    ifstream file(fileName);
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
void exhaustGuaranteed(uint32_t& seed,string pattern,vector<int>vPattern){
  int n = 0;
    if (pattern == "LuaAs"){
      //n = 61386; // == 36ee3742
      n= 61497; // 7909ED5B
    } else if (pattern == "LuaSevens"){
      // n = 61443; gets us 3b7a8b55 == 61443 from 0
        n = 61237; // == 70350767 frame, necessary for lua.
    } else if (pattern == "LuaEights"){
      // n = 61511; // == E2550DE9
      n = 61305; // == 780599B
    } // assumes 2f of delay for 7s and 4f for 8s.
    seed = LCGn(seed,n);
}
uint32_t seekTarget(uint32_t& seed,int target,string patternLabel,vector<int>pattern){
    exhaustGuaranteed(seed,patternLabel,pattern);
    for (int i = 0;i<target;i++){
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
        blurDirection = "Counter ClockWise - 0";
    }
    //replace with return statement later.

    cout << left << setw(21) << blurDirection << "  ";
}
void selectBlurStyle(uint32_t& seed){
  //similar process to blurDirection, will only fill this out if there's demand for it.
  LCG(seed);
  LCG(seed);
}
int dummyCam(uint32_t& seed,int camAngle1,int camAngle2){
  bool rareFailure = false;
  uint32_t v1 = 0;
  int count = 0;
  int seedPercent = 0;
  int totalRNGCalls = 0;
  /*The order of the camera angles doesn't actually matter, just that there are two of them.
  Under what condition is a angle of 10,11 possible? 
  When is rareFailure due to dpad sens possible?
  */

  do{
    count++;
    LCG(seed);
    totalRNGCalls++;
    v1 = seed >> 16;
    v1 = v1 % 10; 
   } while(v1 == 3 || v1 >= 10 || v1 == camAngle1 || v1 == camAngle2);
  LCG(seed);
  totalRNGCalls++;
  if (v1 == 5 || v1 == 7){
    if(!rareFailure){
      LCG(seed);
      totalRNGCalls++;
    }
  } else if (v1 == 6 || v1 == 9){
    LCG(seed);
    totalRNGCalls++;
  } else if (v1 == 8){
    seed = LCGn(seed, 4);
    totalRNGCalls += 4;
  }
  
    // cout << "Total RNG from dummycam: " << totalRNGCalls << endl;
    // cout << "CAM: " << count << endl;
    //Some physics stuff
    // LCG(seed);
    return count+1;
}
int debugDummyCam(uint32_t& seed,int camAngle1,int camAngle2){
  
  /*The order of the camera angles doesn't actually matter, just that there are two of them.
  Under what condition is a angle of 10,or 11 considered?
  When is rareFailure due to dpad sens possible?
  */
  uint32_t v1 = 0;
  int count = 0;
  do{
    LCG(seed);
    v1 = (seed >> 16) % 10;
    count++;
  } while (v1 == 3 || v1 >= 10 || v1 == camAngle1 || v1 == camAngle2);
  LCG(seed); //direction choice
  if (v1 >= 5){
    LCG(seed); //opponent/player choice
    if (v1 == 8){
      LCGn(seed,3);
    }
    if (v1 >= 10){
      LCGn(seed,2);
    } 
  }
  return count+1;

//debug
  //int totalRNGCalls = 0;
  // LCG(seed); //clockwise counterclockwise direction choice
  // totalRNGCalls++;
  // if (v1 == 5 || v1 == 7){
  //   //This is dependent on wDpad sens being set to 1. Addr: 804eb8e0
  //   //If 0, or if trainerperside is != 0, then skip.
  //     LCG(seed);
  //     totalRNGCalls++;
  // } else if (v1 == 6 || v1 == 9){
  //   LCG(seed);
  //   totalRNGCalls++;
  // } else if (v1 == 8){
  //   LCGn(seed, 4);
  //   totalRNGCalls += 4;
  // } else if (v1 == 10 || v1 == 11){
  //   LCGn(seed,3);
  //   totalRNGCalls += 3;
  // }
    // cout << "Total RNG from dummycam: " << totalRNGCalls << endl;
    // cout << "CAM: " << count << endl;
    //return count+1;
}
void rollToGeneration(uint32_t&seed,int blurDuration, int camFrame,int cA1,int cA2){ 
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
          // cout << "Seed entering dummycam: " << hex << seed << dec << endl;
          battleCameraCalls = dummyCam(seed,cA1,cA2);
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
  cout << left;


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
    cout << "Seed" 
    //<< hiOrLow << setw(4) 
    << ": " << hex << setw(8) << outSeed
    << " PID: " << setw(8) << PID << dec
    << "  " << setw(2) << hp << " " << setw(2) << atk << " " << setw(2) << def << " " << setw(2)
    << spa << " " << setw(2) << spd << " " << setw(2) << spe << "  "
    << setw(7) << displayNature << "  " << displayGender << endl;
}
void updatePreamble (int vStartF, int vMinF, int target,uint32_t& seed,vector<int>loadedPattern,uint32_t targetValue){

    int targetVisualFrame = seekFrame(vStartF,vMinF,target);
    cout<< "Seek frame: " << targetVisualFrame << endl
        << "GENERATED:"  << endl;
}


int main(){

    //REMEMBER TO SUBTRACT 2 FRAMES FROM MINIMUM FROM BETTER TEXTBOXES.
  //~~~~~~~~~~~~ CONFIG INPUTS ~~~~~~~~~~~~~~~~~~~~
    int target = 0; //151 -- starts at 3.
    string patternLabel =""; //Pattern - step calls built in.
    const int VISUAL_START_FRAME = 39034; //38616 for sevens, 38231 for eights, 38181 for A's.
    const uint32_t INITIAL_SEED = 0x0;
    const int SEARCH_WINDOW = 100;
    int cameraAngleCurrent = 0;
    int cameraAnglePrevious = 5;
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    int aToBlur = 18; //18/41/8 appears to be standard for sevens across all seeds. LUA ONLY.
    int blurDuration = 41;
    int camFrame = blurDuration - 8;
    uint32_t listSeed = 0;
    int VISUAL_MIN_FRAMES = 1527; //60fps, adjust for not -5? -- for sevens, add 2f of lag. 1527 vs 1529
    vector<string>allLeadPatterns{"LuaAs","LuaEights","LuaSevens"};


    
    const string FILE_EXTENSION = ".txt";
    const string FILE_LOCATION = "\\Patterns\\leading";
    int patternPosition = target;
    

    uint32_t seed = INITIAL_SEED;
    uint32_t blurSeed = 0;
    uint32_t sim = 0; //The last frame of overlap
    // vector<uint32_t>LastNormal;

  

  for (int currentPattern = 0; currentPattern < 3; currentPattern++){
    seed = INITIAL_SEED;
    blurSeed = 0;
    // sim = 0;
    patternPosition = target;

    patternLabel = allLeadPatterns.at(currentPattern);
    cout << patternLabel << endl;

    if (patternLabel == "LuaSevens"){ //Any correlation between odd and even frames? May need to revisit this with new savestates.
      VISUAL_MIN_FRAMES += 2;
    } else if (patternLabel == "Eights"){
      VISUAL_MIN_FRAMES += 4;
    }

    //preprocessing -- Important!
    vector<int>noisePattern = readNumbersFromFile("leading" + patternLabel + FILE_EXTENSION);
    uint32_t targetValue = seekTarget(seed,target,patternLabel,noisePattern);
    updatePreamble(VISUAL_START_FRAME,VISUAL_MIN_FRAMES,target,seed,noisePattern,targetValue);



    listSeed = seed;
    //NESTED LIST LOOP
    for (int i = 0; i < SEARCH_WINDOW; i++){
      seed = INITIAL_SEED;
      targetValue = seekTarget(seed,target+i,patternLabel,noisePattern);
      patternPosition = target+i;
      
      // cout << aToBlur << " / " << blurDuration << " / " << blurDuration - camFrame << ". ";

      //generate list item and roll between user input and generation
    for (int j = 0;j<aToBlur;j++){
      seed = LCGn(seed,noisePattern.at(patternPosition+j));
      // cout << "SEED: " << hex << seed << endl;
      if (j == aToBlur-3){
        // cout<< "TAPPED" << endl;
        blurSeed = seed; // wrong.
        //The following are 2 calls each
        blurDirection(seed);
        selectBlurStyle(seed);
      }
      // if (i == aToBlur-1){
      //   // This if statement is only for visual debugging.
      //   sim = seed;
      //   sim = LCGn(sim,2*9600);
      // }
    }
    cout << "Reached: " << hex << setw(8)<< targetValue << dec << " ";
    cout << setw(2) << i << ": ";
    //Find the bear.
    rollToGeneration(seed,blurDuration,camFrame,cameraAngleCurrent,cameraAnglePrevious);
    generateMon(seed);  
    }
  //Highlighter dashlines.
  for (int dash = 0; dash < 100; dash++){
    cout << "-";
  }
  cout << endl;

  }

    return 0;
}



/*
FOR LUA EVERYTHANG IS DIFFERENT


18/41/x is standard...
      */
    
