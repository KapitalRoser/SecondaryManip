#include "../../processCore.h"

double pullHi16 (uint32_t &seed, int divisor){
    double X = 0;
    LCG(seed);
    X = seed >> 16;
    X = X/divisor;
    return X;
}
float stupidFloatRounding(float inputF12,float inputF10,float inputF9){   
      /*For whatever reason, multiplying the floats together in this way is as close as I can get to the actual number.
      Seems to still be off on the last (17th) digit. The Hex value lines up so it doesn't appear to affect the 
      results in any way, so I'm leaving it at this. Trying to condense or rewrite any other way causes the 
      rounding to be off by more, up to the final hex digit being different. I suspect the issue lies in F12 being 19 digits,
      but that is a task for someone smarter than me.*/
      float factor1 = 0;
      float factor2 = 0; 
      float subtrahend = 0;

      factor1 = inputF12 * inputF10;
      factor2 = inputF9 * inputF10;

      subtrahend = factor2* inputF9;

      return -(inputF12*factor1 - subtrahend);
}
void InitialXY(float &intendedX, float &intendedY, u32 &seed, int divisor, double loosePiApprox, double anchorX, double anchorY){
    float fworkingX = 0;
    double dworkingX = 0; //Stupid rounding smh, makes a difference.
    //Initial Arithmetic
    fworkingX = pullHi16(seed,divisor);
    fworkingX = (fworkingX * 2) * loosePiApprox;
 
    dworkingX = fworkingX;  //F31 copy

    dworkingX = sin(dworkingX);
    intendedX = dworkingX * 15; //Memaddr 0x8048e60c
    
    dworkingX = fworkingX; //Restore from copy
   
    dworkingX = cos(dworkingX);
    intendedY = dworkingX*15;  //MemAddr 0x
    
    intendedX += anchorX; //anchors are const.
    intendedY += anchorY;

    //Done!
    std::cout << "NEW X POS: " << std::setprecision(17) << intendedX << std::endl;
    std::cout << "NEW Y POS: " << std::setprecision(17) << intendedY << std::endl;
}
void walkTimerCalculation(u32 &seed, int factorTime, int baseTimeS,float timer1,int divisor, int currentCycle){
    double firstCallX = 0;
    double secondCallX = 0;
    double cycleVariance = 0;
    const float fps30 = 1.0/30;
    const float fps60 = 1.0/60;
    //Processing:
    firstCallX = pullHi16(seed,divisor);
    secondCallX = pullHi16(seed,divisor);
    cycleVariance = firstCallX + secondCallX - 1;
    timer1 = cycleVariance * factorTime + baseTimeS;

    //Display
    std::cout << "Cycle: " << currentCycle << ": Timer1 is: " << std::setprecision(17) << timer1 << std::endl;
    std::cout << "At 60fps, this is: " << round(timer1/fps60) << " frames.\n";
    std::cout << "At 30fps, this is: " << round(timer1/fps30) << " frames.\n";
}
double computeAngle(float &intendedX, float &intendedY,double anchorX, double anchorY,bool adjustmentNeeded,double altPiApprox){
    //hopefully we don't need to track turn frames...
    const double absurdlySpecificNumber = 0.000000000000000122464679914735320717376402945839660462569212467758006379625612680683843791484832763671875;
    
    double preAngle1 = anchorX - intendedX; //No proof on this, just a hunch
    double preAngle2 = anchorY - intendedY; //need abs?
    double workingAngle = atan(preAngle1/preAngle2);

    //Some kind of angle adjustment - possibly running into wall fix?
    if (adjustmentNeeded){
    workingAngle -= absurdlySpecificNumber;
    workingAngle = altPiApprox - workingAngle; 
    }
    //alt pi (double) is better than float pi here. seriously.

    float wAngleAsFloat = static_cast<float>(workingAngle);
    double returnAngle =  wAngleAsFloat; //3B6E C2CD
    //Loaded from 0x809E5418 WHICH IS A TURNING ANGLE STORED IN THE NPC DATA!!
    returnAngle = returnAngle/2; //3AEE C2CD
    return returnAngle;
}
void computeInterval(double angleInput, double &intervalValueX, double &intervalValueY){
 //Interval Computation
    float sinResult = static_cast<float>(sin(angleInput)); //"TanF" NOT EVEN A TAN FUNCTION LMAO - dolphin why you lie to me.
    const float adjustment = 0.9999999403953552; //Constant 3F7FFFFF
    float cosResult = static_cast<float>(cos(angleInput)); //FE4

    sinResult = sinResult * adjustment; //C2C3
    
    const float factorC = 0.29032257199287415; //0x3E94A529 CONSTANT YAAAAAAAAAY
    float factorA = sinResult; //C2C3 --- Variable. addr: 0x8048e568
    float factorB = cosResult; //FE4 --- Variable. addr:  0x8048E570

    //Reference function: zz_800dfeec_, lot of math here boils down to just this:
    intervalValueX = 2*factorA*factorB*factorC; // I'll be damned
    intervalValueY = stupidFloatRounding(factorA,factorC,factorB);

    //Debug:
    std::cout << "Computed interval X: " << std::setprecision(17)<<intervalValueX << std::endl;
    std::cout << "Computed interval Y: " << intervalValueY << std::endl;
}
void takeStep (float &nextXPos, float &nextYPos,double intervalValueX, double intervalValueY, bool initialStep){
      int factor = 1;
      if (!initialStep){
          factor = 2;
      }
      nextXPos = intervalValueX * factor + nextXPos;
      nextYPos = intervalValueY * factor + nextYPos;
     
      //debug:
      std::cout << std::endl << "Next X POS: " << std::setprecision(17) << nextXPos << std::endl;
      std::cout << "Next Y POS: " << nextYPos << std::endl;
}
bool checkArrived (double angleInput,float nextXPos, float nextYPos){
    if (angleInput < 1.5 && angleInput >= 0){
    return true; //South-East
    }




    return false;
}

int main(){

    //My math is still flawed for other movements.

    //CONSTANTS: (could make these global)
    const int divisor = 65536; //4780 and 40F0 ....
    //sort out these Pi approxes man
    const double loosePiApprox = 3.1415927410125732421875; //40490FDB
    const double altPiApprox = 3.141592653589793115997963468544185161590576171875; //There also exists an exact double float of this that is almost as long as the absurdly long number in computeAngle()
    const float floatPi = 3.1415927410125732;
    
      //~~~~~~~~~~~~~~ CONFIG ~~~~~~~~~~~~~~~
    int cycleCount = 1;
    uint32_t seed = 0x20FF71DA; //INITIAL SEED 
    const double anchorX = 4; //Starting Location 
    const double anchorY = 24;
    bool adjustmentNeeded = 0;
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
    float intendedX = 0;
    float intendedY = 0;
    int factorTime = 3; //Common value
    int baseTimeS = 5;  //Common Value
    float timer1 = 0;
    float nextXPos = anchorX;
    float nextYPos = anchorY;
    double intervalValueX = 0; //Why are these set values?
    double intervalValueY = 0;
    double angleInput = 0;

    //phenac grandma pink = -140, -10

    //Rng -> intended X and Y positions -> walking angle -> distance interval for steps -> time to reach destination.

    for (int i = 0; i<cycleCount;i++){

    InitialXY(intendedX,intendedY,seed,divisor,loosePiApprox,anchorX,anchorY); //Rolls RNG to find a destination, affects intendedX and intendedY
    
    angleInput = computeAngle(intendedX,intendedY,anchorX,anchorY,adjustmentNeeded,altPiApprox); //Walking angle
    
    computeInterval(angleInput,intervalValueX,intervalValueY);

    //will always be at least one step right? surely the minimum distance is >= one step?
    takeStep(nextXPos,nextYPos,intervalValueX,intervalValueY,true);  //first step

   //Calculate the actual steps.
    int framesWalked = 0;

    //the 4 situations
    if (angleInput < 1.5 && angleInput >= 0){

        while(nextXPos <= intendedX && nextYPos <= intendedY);

    } else if (angleInput >= 1.5){

    } else if (angleInput > -1.5 && angleInput < 0){

    } else if (angleInput < -1.5){
        
    }


    bool arrived = false;
    //insert additional check here?


    while(!arrived){
      takeStep(nextXPos,nextYPos,intervalValueX,intervalValueY,false);
      framesWalked++;

      //movement occurs once per 30fps frame
    }
    
    std::cout << std::endl;
    std::cout << "It took " << framesWalked*2 << " 60fps frames to arrive.\n"; 
    std::cout << "It took " << framesWalked << " 30fps frames to arrive.\n\n";
    std::cout << "Returned angle: " << angleInput*2 << std::endl;
    //TIMER
    walkTimerCalculation(seed,factorTime,baseTimeS,timer1,divisor,i);
    }






    return 0;
}
    /*


Keep an eye out for 4081BFAD intended x pos
and 421BFFE6 intended y pos























~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
load Double BFDA13E000000000 F2 AND F0???? (BED09F00)
80184ed4: bl	->0x800CE2D8

800cb21c: lfs	f1, 0x0020 (sp)
Loads 
state2: 405AC41A from 0x8048E600 (interesting mem location)
state1: 3D5FD680 instead. diff of current new pos?


800cb220: lfs	f0, 0x0028 (sp)
Loads 
state2: BED09F00 from 0x8048E608
state1: 416FFF98

what are these values?

Value carried forward for a whiiile.

800cb224:fdiv	f0,f1,f0
Divide: 
405AC41A / BED09F00 = C1063975 (negative number)

Absoluted into : 41063975 = f1

atan(f1) = 3FB9E025

store/update address that stores this number.


Load 250D3132 from 8047C578 into f0 Double
Load 40490FDB from 8047C528 into f2 double - loads Pi

2 fsubs: (blue func)

f1 - f0 = f0
3FB9E025 - 250D3132 = 3FB9E025  subtracts such a small amount that it almost doesn't even register at all. a difference of just 2 bits!
f2 - f0 = f1
3FB9E025 - 40490FDB = 3FD83F90 When seen as a double, kinda makes a small difference at the tiniest decimal places but as floats it gets wiped anyway.
This is f1 - pi basically.

BLR to earlier:
```````````````````````````````````````````


After blr and value is in the register, it follows this path to being put in the addr. 



80184ed8: frsp	f1,f1
Round double to single precision (static cast float) from dbl.
F0 gets zeroed so prob not impactful?




AFTER THIS, IT IS JUST COPIED AROUND UNTIL PUT INTO THE TARGET ADDR. This is the code if needed:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
z_80184edc starts:

f2 loads 40C90FDB, nothing changes
40C90FDB = 2xPi ?
Fsub
f1 - 0 = f0
3FD83F90 - 0 = 3FD83F90 
basically just a copy into f0;

Fadd
f2 + f0 = f1
40C90FDB + 3FD83F90 = 40FF1FBF

FMOD func.

Check if Pi and angle are valid numbers:
comparison to check if both 3FD83F90 and 40490FDB are valid numbers TRUE
comparison to check if both 3FD83F90 and C0490FDB are valid numbers TRUE 
40490FDB is positive Pi
C0490FDB is negative Pi
f1 gets put into f31 and later returned so that it can be stored in memory. No further changes.









******

80185074: fmadds	f1,f3,f1,f31
40C90FDB * 0 + 3FD83F90 = 3FD83F90
2*Pi
basically just copies f31 into f1

stfs	f1, 0x0040 (r28)
stores result in 0x809e5418



F31 comes from earlier F1


zz_800a3cb0_

Recurring Values for beginning direction: 
3f6dd859a0000000 - 3B6EC2CD
3fd294a520000000 - 3E94A529 - F10, inputC
3f5dd85880000000 - 3AEEC2C4



800a3cb0: mflr	r0
800a3cb4: stw	r0, 0x0004 (sp)
800a3cb8: stwu	sp, -0x0038 (sp)
Update stackpointer

SP = 0x8048e4e8

800a3cbc: stfd	f31, 0x0030 (sp)
Store double

800a3cc0: stfd	f30, 0x0028 (sp)
Store double

800a3cc4: fmr	f30, f1
3E94A529 !!! -- Factor C? -- THIS THING IS EVERYWHERE, constant no matter what angle.

800a3cc8: stw	r31, 0x0024 (sp)
3B6EC2CD into 0x8048E50C
???


800a3ccc: addi	r31, r3, 0
800a3cd0: addi	r3, r4, 0
800a3cd4: addi	r4, sp, 20
just stores addr in r3 into r31
replaces with r4, who in turn is given the sp with an offset of +20



800a3cd8: bl	->0x800A3ADC ----DO I EVEN CARE ABOUT THIS?
Ton of paired singles math.


800a3cdc: lfs	f0, -0x73D0 (rtoc)
 Result: 0.5  addr: 0x8047C2D0

At this pt f30 = f31
800a3ce0: fmuls	f30,f0,f30
result: 3AEEC2CD

800a3ce4: fmr	f1, f30
moved 3AEEC2CD into F1


800a3ce8: bl	->0x800CE6AC tanf() math function. Params?

First, F1 is run thru sin() function.






800a3cec: fmr	f31, f1
800a3cf0: fmr	f1, f30
Store result of first tan in f31, use old value from F30 for tan #2

800a3cf4: bl	->0x800CE6D0


F31 comes from first TanF, F1 comes from 2nd TanF



800a3d04: lfs	f0, 0x0018 (sp)
Result: 3F7FFFFF addr: 8048E500
loads constant!

800a3d08: fmuls	f0,f31,f0
produces: 3AEEC2C3

800a3d0c: stfs	f0, 0x0004 (r31)
Places input A (c2c3) in 0x8048e568


800a3d1c: stfs	f1, 0x000C (r31)
Places input B (FE4) in 0x8048E570


















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
