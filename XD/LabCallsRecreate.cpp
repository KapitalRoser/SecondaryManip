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




int main(){
    //~~~~~~~~~~~~ CONFIG ~~~~~~~~~~~~~~~~~~~~
    const string FILE_NAME = "13NTSCOld";
    const string FILE_EXTENSION = ".txt";
    const uint32_t INITIAL_SEED = 0x246D8FB1;
    const int steps = 13;
    int target = 999;
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    uint32_t seed = INITIAL_SEED;
    
     /*Frames at 30fps to shoot for, will eventually come from finding a good teddy.

     //every 1500f theres a 100 call frame. 
     //Every 500F there is an 87 call frame. 
     //Every 1500F there is a 99 call frame, but alternates with the 100 call frames.
     //These only occur looong after the initial section, which does not follow such regularity. 

     //No discernable difference between frames entered the loading zone, or by starflashing??????
     //RNG seed does affect some section times and needs to be reversed.

      */

      /*
        4
        23x3
        4
        46
        57x3
        68
        78
        2
        57
        68
        78
        2

        1
        58
        57
        11
        78
        sets of 1 or 2 calls: sum: 22
        61
        143
        122

        35
        68


        764 Frames expected. 
        From first full black frame, rng-5 happens 8 frames later on old emu.
        on new emu, 6 frames later. 


      */









    vector<int>noisePattern = readNumbersFromFile(FILE_NAME + FILE_EXTENSION);
    for (int i = 0;i<target-1;i++){
       seed = LCGn(seed,noisePattern.at(i));
        // cout << hex <<seed << " : " << dec << noisePattern.at(i) << endl;
    }
    
    //add on stepframes.
    // LCGn(seed,2*steps);


    cout << "Target: " << target << ". Reached: " << hex << seed;






    return 0;
}