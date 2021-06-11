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
int main(){
    uint32_t seed = 0xc231369f;
  uint32_t v1 = 0;
  int count = 0;
  //burns X calls
  do{
    count++;
    LCG(seed);
    v1 = seed >> 16;
    v1 = v1 % 10;
   } while(v1 == 3);

    //Some physics stuff
    LCG(seed);


  //is v1 == 11? if not, is v1 = 0? If not, is v1 = 3?

  /* Fuction 801a5e00
  is Rlwinm eq true? (supposed to reflect contents put into r0, the target register.)
  This decides whether v1 is % 12 or % 10
   First rng call
  high 16 digits mod 10.
  Is result == 12 Or result == 11? If not, r28 becomes 0. 
  Else: is r27 != 0? -- is 0
  if ==0, then is r29 != 3?
  if ==3, then r28 becomes 1


  if r29 == 10 then skip to if r29 == 3


  If(r27 == 0 && r29 == 3){
    r28 = 1; will loop
    }
    else break; (r28 = 0)


  if rlwinm r28 becomes 0, break out of loop.


  R0 becomes value of r28

  once out of the loop (via putting a 0 from rwlinmn)
  store r0 (value of 11) and r29 into the dummy cam bytes. 

  if r25 == 0, branch and skip a bunch of stuff.

  2^3 into r0
  Some phsyics stuff into an rng call for funzies.


Reference:
  != 82000088, could also be less than.
  == 22000088?
  Function Call 42000088
  result of rlwinm == 1 then 42000088
  result of rlwinm == 0 then 22000088



  SOMETHING RELATED TO CAMERA AFTER ALL!!
  I suspect that on this frame the battle arena loads in so the game sets an initial battle camera angle.
  it doesn't get used as the trainer beginning cutscene plays over it.
  */
 
  //Take last digit and that is how many rolls remain?
   cout<< "RESULT: " << hex << count << " SEED: " << seed;
    return 0;
}