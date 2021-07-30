#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <numeric>
//XD TOOL NOT COLO
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
uint32_t LCGn(uint32_t& seed, const uint32_t n)
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

int alignTrio (vector<int>changeFrames, int currentFrame, int oldAlign, bool toDecrement){
  int alignment = oldAlign;
  if (binary_search(changeFrames.begin(),changeFrames.end(),currentFrame)){ //If current frame is special, adjust alignment.
    if (toDecrement){
      alignment--;
      alignment = alignment % 3;     
    } else {
      alignment++;
    }
    //Over/underflow correction:
    if (alignment >=3){ //has to be this way. Would need euclidean mod implementation to handle negative values.
      alignment = 0;
    } else if (alignment < 0){
      alignment = 2;
    }
  }
  return alignment;
}


int main(){

    // int currentGap = 0;
    // int alignment = 1; //Eights, default
    // string pattern = "Sevens";
    // if (pattern == "Sevens"){
    //   alignment = 0; //?
    // } else if (pattern == "As"){
    //   alignment = 2; //?
    // }
    // //THIS BLOCK ISN'T APPLICABLE, BUT IS RELEVANT TO THE MYST OBJs.
    // int currentGap = 0;
    // int alignment = 1; //Eights, default
    // //FOR FOUNTAIN, ALIGNMENT'S BASE IS UNKNOWN, SINCE BOTH RNG SEED AND OVERALL PATTERN ARE ACCOUNTED FOR.
    // vector<int>changeFrames{4};//initial offset
    // for (int i = 0; i<(2000-4)/125;i++){ //should run 15 times.
    //   if (i % 3 == alignment){
    //     currentGap = 127;
    //   } else {
    //     currentGap = 124;
    //   }
    //   if (i == 0){
    //     changeFrames.at(i) = changeFrames.at(i) + currentGap;
    //   } else {
    //     changeFrames.push_back(changeFrames.at(i-1)+currentGap);
    //   }
      
    //   cout << changeFrames.at(i) <<" " << currentGap << endl;
    // }
  //  vector<int>northFountCF {258,508,757,1008,1258,1507,1758}; //doesn't change? changes? 249/251 pattern?
  //   int previous = northFountCF.at(0);
  //   for (unsigned int i = 0; i < northFountCF.size()-1;i++){
  //       previous -= northFountCF.at(i+1);
  //       cout << abs(previous) << endl;
  //       previous = northFountCF.at(i+1);
  //   }

  //FOR FOUNTAIN, ALIGNMENT'S CAUSE IS UNKNOWN, SINCE BOTH RNG SEED AND OVERALL PATTERN ARE ACCOUNTED FOR.
  int initialOffset = 0; //Does this vary with pattern?
  int currentGap = 0;
  int size = 2003;
  string fountain = "Mid";
  vector<int> possibleIntervals;
  int alignment = 1;
  if (fountain == "Mid"){ //127 - 124 - 124
    possibleIntervals = {127,124,124};
    initialOffset = 4-6; 
    
  } else if (fountain == "North"){
    possibleIntervals = {250,249,251};
    initialOffset = 7-7;//default 7 @ 10, 1 pattern -- pushes boundaries a bit but works.
  }
  //in this case, alignment is really just an anchor from which to begin the pattern cycle, rather than the literal position of a off value in the pattern
  //like I would for an aab.
  int averageValue = 0;
  int denom = possibleIntervals.size();
  if (denom != 0){
    averageValue = accumulate(possibleIntervals.begin(),possibleIntervals.end(),0)/denom;
  }
  
  vector<int>changeFrames{initialOffset};//initial offset  

  for (int i = alignment; i<(size-initialOffset)/averageValue + alignment;i++){ //condition is # of intervals will fit within the filesize.
    currentGap = possibleIntervals.at(i%3);
    if (i == alignment){
      changeFrames.at(0) = changeFrames.at(0) + currentGap;
      cout << changeFrames.at(0) << " - " << currentGap << endl;
    } else {
      changeFrames.push_back(changeFrames.back()+currentGap);
      cout << changeFrames.back() << " - " << currentGap << endl;
    }
  }
  //WOW SOME CHANGE FRAMES ARE PERFECTLY STABLE, IN THAT NO MATTER WHAT ALIGNMENT OF THE PATTERN, THEIR CHANGE POINT IS THE SAME.
  //This is because the average across the patterns is a whole number.
  //EVERY 3 FRAMES: 379, 754, 1129, 1504, 1879, meaning only two frames of doubt per pattern change.
  //757,1507 .. .. 
  

// for (unsigned int i = 0; i < changeFrames.size();i++){
//   cout << changeFrames.at(i) << endl;
// }

//    124,124,127,124,124,127 -- standard?
// {131,255,379,506,630,754,881,1005,1129,1256,1380,1504,1631,1755,1879}
// vector<int>northFountCF {258,508,757,1008,1258,1507,1758};

    return 0;
}