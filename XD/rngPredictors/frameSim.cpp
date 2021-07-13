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
uint32_t LCGPercentage(uint32_t& seed){
  float percentResult = 0;
  uint32_t hiSeed = 0;
  LCG(seed);
  hiSeed = seed >> 16;
  percentResult = hiSeed/65536;
  return percentResult;
}





int main(){
  uint32_t seed = 0x2813acb2;
  uint32_t modOp = 12;
  uint32_t v1 = 0;
  uint32_t v2 = 0;

  uint32_t v3 = 0;
  uint32_t v4 = 0;
  uint32_t v5 = 0;
  uint32_t v6 = 0;
  int count = 0;
  int seedPercent = 0;
  /*This function is WEIRD because after it determines new default battle camera
  It uses the result of the rng math do jump somewhere different in the code!!*/
  modOp = 10;

  do{
    count++;
    LCG(seed);
    v1 = seed >> 16;
    v1 = v1 % modOp; //is this ever % 12? even if so, v1 cant be 12 or greater.
    cout << "v1 " << v1 << endl;
   } while(v1 == 3 || v1 >= 10); //What to do if v1 == or >= 11?
    // the function will also loop if v1 == old values, but since these always appear to be 11 and 10, i

  //  if (v1 == 5 || v1 == 6 || v1 == 7 || v1==8){ //at 7, does additional check on r31
  //  } 
  //  if (v1 == 4 || v1 == 0 || v1 == 1 || v1 == 2){
  //    LCG(seed); //Sent into code with float math + physics.
  //  }

  LCG(seed);
  //  if (v1 >= 5 && v1 < 10){
  //    v3 = LCGPercentage(seed);
  //   if (v3 >= 0.5){ //Why they do this, idk - burns a call here too.
  //     LCG(seed);
  //   }
    if (v1 == 8){
      //does same behaviour twice.
      v3 = LCGPercentage(seed);
      // v3 = 2*v3+5;
      v4 = LCGPercentage(seed);
      // v4 = 2*v4+5;
      v5 = LCGPercentage(seed);
      // if (v5 >= 0.5){
      //   v4*= -1; // looks like these values and any additional math on them just get stored in memory.
      // } else {
      //   v3 *= -1;
      // }
    v6 = LCGPercentage(seed);
    //EXTRA CALL?
    }

    //Some physics stuff
    // LCG(seed);
    // return count+1;





    //Some physics stuff
    //float seed
    // LCG(seed);



    //seperate: Rng function float stuff
    /*
    load high bits into bottom of float register.
    subtract to get the bits.
    those 16 high bits / 16 bit max (65536), gets a percentage roll in addition to calling rng.

    */
  //worth noting that the two cam addrs are 11 and 10 respectively.


  //is v1 == 11? if not, is v1 = 0? 
  // if so, load 10 into r0
  
  //If not, is v1 = 3?

  /* Fuction 801a5e00

  is Rlwinm eq true? (supposed to reflect contents put into r0, the target register.)
  This decides whether v1 is % 12 or % 10
      Should always be 10 unless there is already data present in R0 and R25 to handle,
      as seen in an operation to load data into r25's addr if present.

   First rng call
  high 16 digits mod 10.
  v1 = new camera angle.

  Is result == 11 (current camera angle) Or result == 10 (previous camera angle) If not, r28 becomes false. 
  Else: is r27 != 0? -- is 0 usually.
  if ==0, then is r29 != 3?
  if ==3, then r28 becomes true, loop continues. 

  else, loop exits.
  Update angle by storing current angle into past angle., overwriting 10.
  Store v1 into current angle.

  if r25 == 0 (empty), then skip a bunch of stuff.
  Is r29>11? if so, skip a bunch of stuff. if not, 
  v2 = v1*2^2
  this becomes offset for 8040b990. This addr + v2.
  This new addr becomes an INSTRUCTION that the game JUMPS TO. 
  LEAPS TO SPECIFIC INSTRUCTIONS STORED ACROSS MEMORY - is this what a jumptable means?

  in the case of v1 = 6 (meaning v2 = 24) then it goes to 801a64c0
  gets a % rollRNG
  if % from high bits is > 50%, loads 1 into f27
  else it loads -1.

  Loads 1 into r3 then branches.
  This function has a ton of comparisons to r0, reallly wants to skip some calcs. If any pass then it loads
  some value into f1. Else it just loads a 40 over f1.

  Repeats, loads a 65.
  Gets another % roll. 
  After this, lots of math, no comparison in sight so maybe done?



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
  if v1 > 11, skip some physics stuff? 

  2^3 into r0
  Some phsyics stuff into an rng call for funzies.


Reference:
  != 82000088, could also be less than.
  != 42000088?
  == 22000088?
  Function Call 42000088
  result of rlwinm == 1 then 42000088
  result of rlwinm == 0 then 22000088



  SOMETHING RELATED TO CAMERA AFTER ALL!!
  I suspect that on this frame the battle arena loads in so the game sets an initial battle camera angle.
  it doesn't get used as the trainer beginning cutscene plays over it.
  */
   cout<< "RESULT: " << hex << count << " SEED: " << seed  << dec << " V1: " << v1 << " V2: " << v2;



   
    return 0;
}


