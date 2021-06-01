#include <iostream>
#include <iomanip>
#include <math.h>

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

double pullHi16 (uint32_t& seed, int divisor){
    double X = 0;
    LCG(seed);
    X = seed >> 16;
    X = X/divisor;
    return X;
}


int main(){

    //WAITING INTERVAL LENGTH

    const int divisor = 65536; //4780 and 40F0 ....
    const double piApprox = 3.1415927410125732421875;
    const double AnchorX = 4; //Unique to NPC
    const double AnchorY = 24;
    double firstCallX = 0; //can these ever be floats?
    double secondCallX = 0;

    float fworkingX = 0;
    double dworkingX = 0; //Stupid rounding smh
    float intendedX = 0;
    float intendedY = 0;
    double restoreX = 0;
    double cycleVariance = 0; 
    int factorTime = 3; //Common value
    int baseTimeS = 5;  //Common Value
    float fps30 = 1.0/30;
    float fps60 = 1.0/60;
    
    
    float timer1 = 0;

    //~~~~~~~~~~~~~~ CONFIG ~~~~~~~~~~~~~~~
    int cycleCount = 5;
    uint32_t seed = 0x0; //INITIAL SEED
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    for (int i = 0; i<cycleCount;i++){

    
    
    //Initial Arithmetic
    fworkingX = pullHi16(seed,divisor);
    fworkingX = (fworkingX * 2) * piApprox;
 
    dworkingX = fworkingX;  //F31 copy

    
    dworkingX = sin(dworkingX);
    intendedX = dworkingX * 15; //Memaddr 0x8048e60c
    
    
    dworkingX = fworkingX; //Restore from copy
   

    dworkingX = cos(dworkingX);
    intendedY = dworkingX*15;  //MemAddr 0x
    

    intendedX += AnchorX;
    intendedY += AnchorY;

    //Done!
    cout << "NEW X POS: " << setprecision(17) << intendedX << endl;
    cout << "NEW Y POS: " << setprecision(17) << intendedY << endl;
    



    //TIMER


    //Processing:
    firstCallX = pullHi16(seed,divisor);
    secondCallX = pullHi16(seed,divisor);
    cycleVariance = firstCallX + secondCallX - 1;
    timer1 = cycleVariance * factorTime + baseTimeS;

    //Display
    cout << "Cycle: " << i << ": Timer1 is: " << setprecision(17) << timer1 << endl;
    cout << "At 60fps, this is: " << round(timer1/fps60) << " frames.\n";
    cout << "At 30fps, this is: " << round(timer1/fps30) << " frames.\n";
    }


    /*Speed:

    initial pos 4,24

    ROUGHLY 0.5806408 when in a straight line. 

    somewhere between 0.25 and 0.40 units per frame at 30Fps?




    INSTRUCTION SET BEGINS AT 0x8018854c
    ENDS AT 0x80188590

    Sp is 16 less than Target addr
    Sets up 0x4330 in R0


    stores 0002 in 8048E57C from R3

    load float 4330 into f1 (changes?)

    Moved 809e53d8 from r29 into R3

    Store 4330 from r0 into 0x8048E578

    R4 adds 16 to it to get target addr in r4

    Read float from 0x8048E558, store in f3

    add 809e53d8 + 156 store in r5

    4330.... and the 2 from earlier in 0x8048E578 loaded into f0
    (4330000000000002)

    Load Float from 0x8048E540 into F2 (previous location?)

    4330...2 - 4330 = 2. Store in F4

    Load float from 0x8048E560, store in F1

    Load float from 0x8048E548, store in F0

    F3 * F4 + F2, store in F2
    F1 * F4 + F0, store in F0

    This basically boils down to F1 or F3 * 2 + Previous location

    Store f2 in 0x8048E540 and f0 in 8048E548
    (This will later be memcpy'ed into other areas, and eventually end up in the npc data.)


    Question is, what is F1 and F3? What are these numbers?







    Spend some frames turning, then the rest is walking to point.




    P3-P5 store in P7, this becomes new P5.




    Values are read from 0x804 addr up in the stack/code blocks, to be put onto the heap later.

    New X and y pos values are copied 1 byte at a time in reverse order from another address,
    starting with the lowest bit of Y and working up to highest bit of X.
    



    npc location !== physics position value (!)




    What Paired Singles do:
    While w = 0,
    Take a 8 byte number, like a double, and splits the upper and lower 4 bytes.
    Upper four get put in floating register on the left, and lower 4 on the right.

    w = 1, load left side, right side becomes 1.0 .

    
    */















    // Destination!
    /* Based on reaching X and Y coords.
    Does the pullHi16 function into F1(X)
    Then loads a set 2 into F2
    Loads approximation of Pi into F0 40490FDB == 3.1415927.
    X = X * 2
    X = X * approx of Pi
    copy so value is in f31 and f1.
    x = sin(X).
    load a set 15 (FROM NPC DATA - 4170) into f0 - all npcs have this value.
    f0 = f0 * X (this is just X*15 but stored in f0)
    F31 value restored onto X
    Rounds the value in F0 from double to float. (chops low 32 bits off)
    Store f0's value in memory addr

    f1 gets COS'ed and put back in f1,
    1 - F1 = F0
    Load 15 again into F0
    preps some addresses in the parameter registers 3/4/5

    F0 = F0*X same as above
    Round from double to float.
    Passed into a paired singles function.
    PSVE Add function:

    R3 Load a value (DIFFERENT FOR EACH NPC!!) into F2 (what purpose?)
    R4 Load into f4 8048e60c, one of the F0 values from earlier
    F6 = F2+F4 (Paired single addition)
    F6 Stored in Intended X position spot.
    Repeat, but R3 and R4 (into F5) addrs are offset by + 8
    Store in F7
    Becomes Intended Y position.



    */

    return 0;
}