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
void exhaustGuaranteed(string pattern,uint32_t& seed){
  //Is it better to have a list of these predetermined values
  //Or do I pass in the vector of values an sum the calls?
  //would make it easier to accept new or modified patterns
  //The code would verify the pattern automatically.
  int n = 0;
    if (pattern == "leadingAs"){
      n = 61386;
    } else if (pattern == "leadingSevens"){
      n = 0;
    } else if (pattern == "leadingEights"){
      n = 0;
    }
    seed = LCGn(seed,n);

    //REMEMBER TO TEST THIS AND REPLACE!!!
    //vs this:
    // const int minFrames = 764; //Works for any pattern, the condition statement may be off though. 
    // for (int i = 0; i<(minFrames);i++){
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
        blurDirection = "Clockwise - 1";
    } else {
        blurDirection = "CounterClockWise - 0";
    }
    //replace with return statement later.
    cout << blurDirection << endl;
}
int dummyCam(uint32_t& seed){
  uint32_t v1 = 0;
  int count = 0;
  //burns X calls

  //Sim calls up to this point

  do{
    count++;
    LCG(seed);
    v1 = seed >> 16;
    v1 = v1 % 10; //is this ever % 12?
   } while(v1 == 3);

    //Some physics stuff
    LCG(seed);
    return count+1;
}
void rollToGeneration(uint32_t&seed){ 
  const int blurCalls = 9600;
  string version = "Modern";
  int n = 0;
  int blurBeforeCam = 0;
  int blurAfterCam = 0;
  int blurDuration = 46;
  int battleCameraCalls = 0;

    //THE HOLY EQUATION for post txtbox. 
  // if(version == "LUA"){
  //   blurDuration = 41; //Isn't represented until 3 frames in, as the first two frames overlap the background noise pattern.
    
  // } else if (version == "Modern"){
  //   blurDuration = 49;
  //   blurBeforeCam = 41;

  //   if (framesShortened > 0){ //Some kind of relationship between the short blur frame beginning and the hiFrame loFrame problem. Not just a houndour thing.
  //     blurDuration -= framesShortened;
  //     blurAfterCam = blurDuration-blurBeforeCam;
  //   } else {
  //     blurAfterCam = 8;
  //   }
  // }
  // cout << blurBeforeCam + blurAfterCam << endl;
  // blurDuration--;


  //bit wonky, because dummy cam calls are determined by the point value part-way through the blur.
  //could just suck it up and loop this. - gonna do that.


    for (int i = 0; i<blurDuration; i++){
        seed = LCGn(seed,blurCalls);
        // cout << hex << seed << endl;
        if (i == 39){ //is this constant?
          // cout << "Seed entering dummycam: " << hex << seed << dec << endl;
          battleCameraCalls = dummyCam(seed);
          // cout << "CAM: " << battleCameraCalls << endl; 
        }
    }
    seed = LCGn(seed,4); // These occur between the lag frame before the outSeed.



  
  // seed = LCGn(seed,blurBeforeCam*blurCalls);
  // battleCameraCalls = dummyCam(seed);
  //  cout << "BattleCamCalls " << battleCameraCalls << endl;
  // seed = LCGn(seed,(blurAfterCam+battleCameraCalls)*blurCalls);
  // n = blurDuration*blurCalls + dummyCam(seed) + 4; //is 4 const?
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
    
    uint32_t userInputSeed = 0x920457b9; //CALC SEED
    int aToBlur = 20;
    const string PATTERN = "leadingAs"; //Pattern - step calls built in.
    int target = 278; //151
    const int VISUAL_START_FRAME = 38181;
    const uint32_t INITIAL_SEED = 0x0;
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    const int VISUAL_MIN_FRAMES = 1527; //60fps, adjust for not -5?
    const string FILE_EXTENSION = ".txt";
    int patternPosition = target;
    


    //17, 19, 20
    
    uint32_t seed = INITIAL_SEED;
    vector<uint32_t>toEndOfOverlap; //+2f from normal (?)
    uint32_t blurSeed = 0;
    // vector<uint32_t>LastNormal;

    vector<int>noisePattern = readNumbersFromFile(PATTERN + FILE_EXTENSION);
    int targetVisualFrame = seekFrame(VISUAL_START_FRAME,VISUAL_MIN_FRAMES,target);
    exhaustGuaranteed(PATTERN,seed); //test alt solution and put this into seekTarget()
    
    uint32_t targetValue = seekTarget(seed,target,noisePattern);

    for (int i = 0;i<=aToBlur;i++){
      seed = LCGn(seed,noisePattern.at(patternPosition+i));
      if (i == aToBlur-3){
        blurSeed = seed;
      }
      // if (i == 16 || i == 17){ //I theorize that this is the same as the blur frame.
      //   LastNormal.push_back(seed);
      // }
      if (i == aToBlur){
        toEndOfOverlap.push_back(seed);
      }
    }

    //BlurFrames == last normal? does last normal even matter at all?
    //Printouts:
    cout<< "Seek frame: " << targetVisualFrame << endl
        << "Target: " << target << ". Reached: " << hex << targetValue << endl
        << "Blur Frame: " << blurSeed << endl
        << "Frames used for calculation: " << toEndOfOverlap.at(0)
       << endl; 






















    // int framesShort = 5;
    blurDirection(blurSeed);
    rollToGeneration(userInputSeed);
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

