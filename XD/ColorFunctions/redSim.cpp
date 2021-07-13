#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>
#include <vector>
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

int redFunction (uint32_t &seed, uint32_t redVariable){
  //Contains some quat and vec functions, likely something to do with camera movement.
  int count = 0;
  float redFloat = 0;

  //BITMAGIC BLOCK
  uint32_t rv_Initial = redVariable & 3;
  uint32_t rv_Large = redVariable & 128;
  uint32_t rv_Medium = redVariable & 64;
  uint32_t rv_Small = redVariable & 32;
  uint32_t rv_Micro = redVariable & 16;
  // functionally achieves a Mod to a power of 2, but it's a bit weird so I'm leaving it in it's bitmagic form.


  //redVariable initialization from r5's addr.

  redVariable += 265;

  //Can potentially simplify further.
  //Can get called but redvariable (r0) could be something super high like 40.
  if (rv_Initial != 2){
    if (rv_Initial < 2) {
      if(rv_Initial != 0){
        if (rv_Medium == 0){
          LCGPercentage(seed);
          count++;
        }
        if (rv_Large == 0){
          LCGPercentage(seed);
          count++;
        }
        if (rv_Small == 0){ 
          LCGPercentage(seed);
          count++;
        }
        return count;
      }
      if (0x804eda38 < redFloat){ // || 0x804eda38 < fvar2 after some calcs. ) && dvar < 0x804eda38, which is gained from some memory addr. 
        if (rv_Large == 0){
          LCGPercentage(seed);
          count++;
        }
        if (rv_Small == 0){
        LCGPercentage(seed);
        count++;
        }
      }
      if (rv_Micro == 0){
        LCGPercentage(seed);
        count++;
      }
      return count;
    }
    if (rv_Initial < 4){
      if (rv_Large == 0){
        LCGPercentage(seed);
        count++;
      }
      if (rv_Small == 0){
        LCGPercentage(seed);
        count++;
      }
    }
    return count;
  }
  return count;
}



int main(){
    uint32_t seed = 0;
    uint32_t rv_Input = 256;
    int rngCalls = redFunction(seed,rv_Input);
    cout << "An input of: " << rv_Input << " produces: " << rngCalls << " RNG calls.";





    return 0;
}