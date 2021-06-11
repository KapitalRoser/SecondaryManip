#include <iostream>
#include <iomanip>
#include <math.h>
#include <sstream>
#include <vector>
#include <fstream>

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
uint32_t exhaustGuaranteed(string pattern,uint32_t& seed){
    if (pattern == "leadingAs"){
      seed = LCGn(seed,61386);
    } else if (pattern == "leadingSevens"){
      seed = LCGn(seed,0);
    } else if (pattern == "leadingEights"){
      seed = LCGn(seed,0);
    }
    //exhaust guaranteed calls according to previous pattern tests. Any deviations work themselves out by the time all footsteps are completed.
}

int main(){
    //~~~~~~~~~~~~ CONFIG ~~~~~~~~~~~~~~~~~~~~
    const string PATTERN = "leadingAs"; //Pattern
    const string FILE_EXTENSION = ".txt";
    const uint32_t INITIAL_SEED = 0x0;
    // const int steps = 13;
    // bool earlyFrame = 0;
    int target =161;
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    const int VISUAL_START_FRAME = 38181;
    const int VISUAL_MIN_FRAMES = 1527; //60fps
    int targetVisualFrame = VISUAL_START_FRAME + VISUAL_MIN_FRAMES + target*2;
    int patternPosition = target;
    uint32_t targetValue = 0;
    uint32_t seed = INITIAL_SEED;
    vector<int>noisePattern = readNumbersFromFile(PATTERN + FILE_EXTENSION);
    vector<uint32_t>LastNormal;
    vector<uint32_t>blurFrames; //-1f from  normal
    vector<uint32_t>toEndOfOverlap; //+2f from normal (?)

    exhaustGuaranteed(PATTERN,seed);
    
    for (int i = 0;i<target;i++){ // Seek target
      seed = LCGn(seed,noisePattern.at(i));
    }
    targetValue = seed;

    for (int i = 0;i<=19;i++){
      seed = LCGn(seed,noisePattern.at(patternPosition+i));
      if (i == 15 || i == 16){
        blurFrames.push_back(seed);
      }
      if (i == 16 || i == 17){
        LastNormal.push_back(seed);
      }
      if (i == 18 || i == 19){
        toEndOfOverlap.push_back(seed);
      }
    }

    //BlurFrames == last normal? does last normal even matter at all?
    //Printouts:
    cout<< "Seek frame: " << targetVisualFrame << endl
        << "Target: " << target << ". Reached: " << hex << targetValue << endl
        << "Blur Frames: " << blurFrames.at(0) << ", " << blurFrames.at(1) << endl
        << "Last Normal Frames: " << LastNormal.at(0) << ", " << LastNormal.at(1) << endl
        << "Frames used for calculation: " << toEndOfOverlap.at(0) << ", " << toEndOfOverlap.at(1) << endl; 

    //Time for hiFrameLoFrame bullshit.
    //A is pressed on target frame, fadeoutCalcs:
    
    

  

    // for(int i = 0; i<19;i++){
    //   seed = LCGn(seed,noisePattern.at(patternPosition+i));
    //   if (i == 16 || i == 17){
    //     lastNormal;
    //   }
    //   if (i >=15){
    //     cout << seed << ",";
    //   }
    //   // cout << seed << endl;
    //   }
    //   cout << endl;
    //   cout << "Last normal frame: " << seed;
    //   seed = LCGn(seed,noisePattern.at(patternPosition+17));
    //   cout << " or " << seed;





    return 0;
}


 
     /*Frames at 30fps to shoot for, will eventually come from finding a good teddy.

     //every 1500f theres a 100 call frame. 
     //Every 500F there is an 87 call frame. 
     //Every 1500F there is a 99 call frame, but alternates with the 100 call frames.
     //These only occur looong after the initial section, which does not follow such regularity. 

     //No discernable difference between frames entered the loading zone, or by starflashing??????
     //RNG seed does affect some section times and needs to be reversed.
     //Only so many types of patterns exist. Lua slot 8 is on a leading 7s pattern, whereas modern slot 8 is on a leading A's pattern. Across versions though, these specific patterns are the same.
     //Current unknown: What causes you to end up on one pattern vs another?
     //Extent of the impact of specific rng seed on the pattern - appears to influence the start and end of pattern segments. 

     //For now, document leading A's and 7's pattern as a confidence boost.
     //For what it's worth, an A's pattern is on even frames for newdolphin.

    X frames of overlap may exist*/