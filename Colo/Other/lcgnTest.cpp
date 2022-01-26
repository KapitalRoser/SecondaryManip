#include "../../processCore.h"
u32 LCGnn(u32& seed, const u32 n)
  {
    u32 ex = n - 1;
    u32 q = 0x343fd;
    u32 factor = 1;
    u32 sum = 0;
    while (ex > 0)
    {
      if (!(ex & 1))
      {
        sum = sum + (factor * modpow32(q, ex));
        
        ex--;
      }
      factor = factor * (1 + q);
      q *= q;
      ex /= 2;
    }
    seed = (seed * modpow32(0x343fd, n)) + (sum + factor) * 0x269EC3;
    std::cout << modpow32(0x343fd, n) << "\n";
    return seed;
  }


int main(){

    u32 seed = 256;
    LCGnn(seed,10);
    std::cout << seed << "\n" << std::hex << seed;

    return 0;
}