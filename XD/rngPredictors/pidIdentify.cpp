#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;


struct frameData{
  int visFrame; //expecting single value per frame.
  int reached;
  vector<int> outSeed; //expecting 20 of each per frame.
  vector<int> PID;
  vector<int> aToBlur;
  vector<int> blurDuration;
  vector<int> camOffset;
};

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
vector<int> decimalReadNumbersFromFile(string fileName)
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
vector<uint32_t> hexReadNumbersFromFile(string fileName)
{
    uint32_t value;
    string lineRead = "";
    stringstream hexConvert;
    vector<uint32_t> data; //Setup
    ifstream file(fileName);
            cout << "Read some data! \n";
    if (file.fail())
    {
        cout << "File inaccessible";
        exit(EXIT_FAILURE);
    }
    while (!(file.fail()))
    {                 
        getline(file,lineRead); 
        hexConvert << hex << lineRead; //cuz i just HAD to have my docs in hexa and not int...;
        hexConvert >> value;
        hexConvert.clear();
        hexConvert.str("");
        data.push_back(value);
    }
    file.close();
    return data;
}
void exhaustGuaranteed(uint32_t& seed,string pattern,vector<int>vPattern){
  int n = 0;
    if (pattern == "As"){
      n = 61386; // == 36ee3742
      // n= 61497; // 7909ED5B - lua
    } else if (pattern == "Sevens"){
      n = 61443; // gets us 3b7a8b55 == 61443 from 0
        // n = 61237; // == 70350767 lua.
    } else if (pattern == "Eights"){
      n = 61511; // == E2550DE9
      // n = 61305; // == 780599B
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
    leftRightLo = seed >> 16; //does leftrightLo & 1 accomplish the result? Ghidra says it should.
    LCG(seed);
    leftRightHi = seed >> 16;
    leftRight = (leftRightHi << 16) | leftRightLo;
    
    if (leftRight % 2 == 1){ //literally just even or odd
        blurDirection = "Clockwise - 1"; 
    } else {
        blurDirection = "Counter ClockWise - 0";
    }
    //replace with return statement later.

    // cout << left << setw(21) << blurDirection << "  ";
}
void selectBlurStyle(uint32_t& seed){
  //similar process to blurDirection, will only fill this out if there's demand for it.
  LCG(seed);
  LCG(seed);
  //Only 3 or 4 types of splotches?
}
int dummyCam(uint32_t& seed,int camAngle1,int camAngle2){
  uint32_t v1 = 0;
  int count = 0;
  int seedPercent = 0;
  int totalRNGCalls = 0;
  /*The order of the camera angles doesn't actually matter, just that there are two of them.
  Under what condition is a angle of 10,11,or 12 possible? 
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
    //This is dependent on wDpad sens being set to 1. Addr: 804eb8e0
    //If 0, or if trainerperside is != 0, then skip.
      LCG(seed);
      totalRNGCalls++;
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
void generateMon(uint32_t inputSeed,frameData &curF){
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
  // cout << left;


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
    bool pidGender = genderRatio > (PID & 0xFF) ? 1 : 0; //Hell if I know what these bitwise operations mean, I put my trust in the code that came before me.
    string displayGender;
    if (pidGender){
        displayGender = "Female";
    } else {
        displayGender = "Male";
    }
    // cout << "Seed" 
    // //<< hiOrLow << setw(4) 
    // << ": " << hex << setw(8) << outSeed
    // << " PID: " << setw(8) << PID << dec
    // << "  " << setw(2) << hp << " " << setw(2) << atk << " " << setw(2) << def << " " << setw(2)
    // << spa << " " << setw(2) << spd << " " << setw(2) << spe << "  "
    // << setw(7) << displayNature << "  " << displayGender << endl;


    //This should get called 20 times to output the important stuff to the greater frame data struct.
    curF.outSeed.push_back(outSeed);
    curF.PID.push_back(PID);
}
int updatePreamble (int vStartF, int vMinF, int target,uint32_t& seed,vector<int>loadedPattern,uint32_t targetValue){
    int targetVisualFrame = seekFrame(vStartF,vMinF,target);
//     cout<< "Seek frame: " << targetVisualFrame << endl
//         << "Target: " << target << ". Reached: " << hex << targetValue << endl 
//         << "GENERATED:" << dec << endl;
return targetVisualFrame;
 }
void trackToInput(vector<frameData>&outFrame, int target){
  
    //REMEMBER TO SUBTRACT 2 FRAMES FROM MINIMUM FROM BETTER TEXTBOXES.
  //~~~~~~~~~~~~ CONFIG INPUTS ~~~~~~~~~~~~~~~~~~~~
    // int target = 0; //151
    string patternLabel =""; //Pattern - step calls built in.
    const int VISUAL_START_FRAME = 38181; //38616 for sevens, 38231 for eights, 38181 for A's.
    const uint32_t INITIAL_SEED = 0x0;
    int cameraAngleCurrent = 10;
    int cameraAnglePrevious = 11; 
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    int aToBlur = 0; //,19,20 -- 18? 17? No evidence that these exist.
    int blurDuration = 0; //46,47,48,49, 50????
    int camFrame = blurDuration - 0; //9,8
    uint32_t listSeed = 0;
    int VISUAL_MIN_FRAMES = 1527; //60fps, adjust for not -5? -- for sevens, add 2f of lag. 1527 vs 1529
    vector<string>allLeadPatterns{"As","Eights","Sevens"};


    
    const string FILE_EXTENSION = ".txt";
    const string FILE_LOCATION = "Patterns\\leading";
    int patternPosition = target;
    

    uint32_t seed = INITIAL_SEED;
    uint32_t blurSeed = 0;
    uint32_t sim = 0; //The last frame of overlap
    // vector<uint32_t>LastNormal;


    

  

  for (int currentPattern = 0; currentPattern < 1; currentPattern++){ //this only runs 1 per frame, expanding it would get weird.
    frameData curF; //Current Frame. redefined per frame.
    seed = INITIAL_SEED;
    blurSeed = 0;
    // sim = 0;
    patternPosition = target;

    patternLabel = allLeadPatterns.at(currentPattern);
    // cout << patternLabel << endl;

    if (patternLabel == "Sevens"){ //Any correlation between odd and even frames? May need to revisit this with new savestates.
      VISUAL_MIN_FRAMES += 2;
    } else if (patternLabel == "Eights"){
      VISUAL_MIN_FRAMES += 4;
    }

    //preprocessing -- Important! -- point value of seed does not have an impact on blurduration or atoblur.
    vector<int>noisePattern = decimalReadNumbersFromFile(FILE_LOCATION + patternLabel + FILE_EXTENSION);
    uint32_t targetValue = seekTarget(seed,target,patternLabel,noisePattern);
    curF.reached = targetValue;
    curF.visFrame = updatePreamble(VISUAL_START_FRAME,VISUAL_MIN_FRAMES,target,seed,noisePattern,targetValue);



    listSeed = seed;
    //NESTED LIST LOOP
    for (int i = 0; i <= 19; i++){
      seed = listSeed;
      //Decode block.
      if (i % 10 < 2){
        blurDuration = 46;
      } else if (i % 10 < 4){
        blurDuration = 47;
      } else if (i % 10 < 6){
        blurDuration = 48;
      } else if (i % 10 < 8){
        blurDuration = 49;
      } else {
        blurDuration = 50;
      }
      if (i % 2){
        camFrame = blurDuration - 9;
      } else {
        camFrame = blurDuration - 8;
      }
      if (i >= 10){
        aToBlur = 20;
      } else {
        aToBlur = 19;
      }
      //OUTPUT PATTERN. if matching these vectors gets weird then i'll restructure with pattern IDs.
      curF.aToBlur.push_back(aToBlur);
      curF.blurDuration.push_back(blurDuration);
      curF.camOffset.push_back(blurDuration - camFrame);

      // cout << aToBlur << " / " << blurDuration << " / " << blurDuration - camFrame << ". ";

      //generate list item and roll between user input and generation
    for (int i = 0;i<aToBlur;i++){
      seed = LCGn(seed,noisePattern.at(patternPosition+i));
      if (i == aToBlur-3){
        blurSeed = seed;
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

    // cout << setw(2) << i << ": ";

    //Find the bear.
    rollToGeneration(seed,blurDuration,camFrame,cameraAngleCurrent,cameraAnglePrevious);
    generateMon(seed,curF);
    
    //debug
    }
  // for (int dash = 0; dash < 100; dash++){
  //   cout << "-";
  // }
  // cout << endl;
  // }
  outFrame.push_back(curF);
}
}
int matchPIDs(vector<int>candidatePIDS,int sourcePID, bool &dblMatch){
  for (unsigned int i = 0; i < candidatePIDS.size()-1; i++){ //Loop all candidates
    if (candidatePIDS.at(i) == sourcePID){ //evaluate.
      if (candidatePIDS.at(i+1) == sourcePID){
        dblMatch = true; //means an 8 or 9 camframe makes no impact on pid.
        // cout << "dbl match happened\n";
      } else {
        dblMatch = false;
      }
      return i; //YAY! Correct PID found!
    }
  }
  return -1; //error, no pid found on frame. May be missing in original recording or is a weird edge case not seen before.
 }

int main(){
    const string FILE_EXTENSION = ".txt";
    string FILE_NAME = "";
    cout << "Enter filename: ";
    getline(cin, FILE_NAME);
    vector<uint32_t>sourcePIDS = hexReadNumbersFromFile(FILE_NAME + FILE_EXTENSION);
    vector<int>sourceCamFrames = decimalReadNumbersFromFile("outCamFrameOnly.txt");
    cout << sourcePIDS.size() << endl;
    ofstream outputData ("out" + FILE_NAME + FILE_EXTENSION);

    vector<frameData> singleFrame;
    int correctPID = 0;
    bool dblMatch = false;


for (int i = 0; i<1025;i++){ //PER FRAME BASIS
//THIS SHOULD BE LIMITED BY sourcePIDS.size()-1 HOWEVER current leading As pattern is too small lmao.
//file out: Frame# Visual - Reached - outSeed - PID - Pattern.

//Alt: Pattern
//alt2: pid - pattern. 
trackToInput(singleFrame,i); //Builds a frame's worth of data for output -- comparison.
correctPID = matchPIDs(singleFrame.at(i).PID,sourcePIDS.at(i),dblMatch);
if (correctPID == -1){  //error case
  outputData << "ERROR - MISSING?\n";
  continue;
}

//print block
outputData
<< singleFrame.at(i).visFrame 
<< hex
 << ": - Reached: "<< setw(8) 
<< singleFrame.at(i).reached
<< " - Seed: "    << setw(8) << singleFrame.at(i).outSeed.at(correctPID)
<< " - PID:  "    << setw(8) << singleFrame.at(i).PID.at(correctPID) 
<< dec 
<< " - Pattern: "
<< singleFrame.at(i).aToBlur.at(correctPID)
<< "/" 
<< singleFrame.at(i).blurDuration.at(correctPID) 
<< "/";
if (dblMatch){
  outputData << sourceCamFrames.at(i);
  // << " *";
} else {
  outputData << singleFrame.at(i).camOffset.at(correctPID);
}
outputData << endl;
} //end of loop
// vector<uint32_t>dupes;
// for (unsigned int i = 0; i < sourcePIDS.size() -1 ;i++){
//   if (sourcePIDS.at(i) == sourcePIDS.at(i+1)){
//     dupes.push_back(sourcePIDS.at(i));
//   } 
// }
// outputData << " Tripes found: " << dupes.size() << endl;
// outputData << "List: \n";
// for (unsigned int i = 0; i < dupes.size() - 1; i++){
//   outputData << hex << dupes.at(i) << endl;
// }

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

    20/47/8 -WoW - 235 - 239 pattern 18
    20/48/8 240 - 244 ptn 20 new 24.
    245 - 253 ptn 12, new 14.
    254 - new old ptn 14
    255 - new ptn 9
     256++ ptn 15/neww 17
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


      ALL COMBOS: (old) wow, still no evidence for 17 actually existing.
      0  17/46/8
      1  17/46/9
      2  17/47/8 
      3  17/47/9
      4  17/48/8
      5  17/48/9
      6  17/49/8
      7  17/49/9
      8  19/46/8
      9  19/46/9 -- seen
      10 19/47/8
      11 19/47/9 -- seen
      12 19/48/8 -- seen
      13 19/48/9 -- seen
      14 19/49/8 -- seen 254
      15 19/49/9 -- seen
      16 20/46/8 -- seen
      17 20/46/9 -- seen
      18 20/47/8 -- seen
      19 20/47/9 -- seen
      20 20/48/8 -- seen
      21 20/48/9 -- seen
      22 20/49/8
      23 20/49/9


     ALL COMBOS: (new)
      0  17/46/8
      1  17/46/9
      2  17/47/8 
      3  17/47/9
      4  17/48/8
      5  17/48/9
      6  17/49/8
      7  17/49/9
      8  17/50/8
      9  17/50/9
      10 19/46/8 -- maybe seen
      11 19/46/9 -- seen
      12 19/47/8 -- Seen
      13 19/47/9 -- seen -152 -- eights 56.
      14 19/48/8 -- seen == old 12. frame 253
      15 19/48/9 -- seen
      16 19/49/8 -- seen frame 254
      17 19/49/9 -- seen frame 256. -- 257? -- 256 on eights too.
      18 19/50/8
      19 19/50/9 -- seen frame 255 sevens
      20 20/46/8 -- seen
      21 20/46/9 -- seen
      22 20/47/8 -- seen == old ptn 18
      23 20/47/9 -- seen
      24 20/48/8 -- seen == old ptn 20 153 154
      25 20/48/9 -- seen
      26 20/49/8
      27 20/49/9 -- seen 45/46 As.
      28 20/50/8
      29 20/50/9

      ALL COMBOS NEWNEW -- no 17.
      0  19/46/8 -- maybe seen
      1  19/46/9 -- seen
      2  19/47/8 -- Seen
      3  19/47/9 -- seen -152 -- eights 56.
      4  19/48/8 -- seen == old 12. frame 253
      5  19/48/9 -- seen
      6  19/49/8 -- seen frame 254
      7  19/49/9 -- seen frame 256. -- 257? -- 256 on eights too.
      8  19/50/8
      9  19/50/9 -- seen frame 255 sevens
      10 20/46/8 -- seen
      11 20/46/9 -- seen
      12 20/47/8 -- seen == old ptn 18
      13 20/47/9 -- seen
      14 20/48/8 -- seen == old ptn 20 153 154
      15 20/48/9 -- seen
      16 20/49/8
      17 20/49/9 -- seen 45/46 As.
      18 20/50/8
      19 20/50/9

  
  try 111 next.





      21??????????? 50????????
 
      Frame lengths independent of rng seed. Will try to find, otherwise need to record with scripts. 
      */
    
