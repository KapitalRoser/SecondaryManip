#include "../processCore.h"


void simpleDummyCam(u32& seed,int camAngle1,int camAngle2,bool isFirst){
 u32 v1 = 0;
 int modulus = isFirst ? 10 : 12;
  do{
    LCG(seed);
    v1 = (seed >> 16) % modulus;
  } while ((isFirst && (v1 >= 10 || v1 == 3)) || v1 == camAngle1 || v1 == camAngle2);
  LCG(seed); //direction for the ones that care
  if (v1 >= 5){
    LCG(seed);
    if (v1 == 8){
      LCGn(seed,3);
    }
    if (v1 >= 10){
      LCGn(seed,2);
    } 
  }
}


void fullCamera(u32& seed,int camAngle1,int camAngle2,bool isFirst){
    u32 v1 = 0;
    int modulus = isFirst ? 10 : 12;
    do{
        LCG(seed);
        v1 = (seed >> 16) % modulus;
    } while ((isFirst && (v1 >= 10 || v1 == 3)) || v1 == camAngle1 || v1 == camAngle2);
    

    float percentResult;
    switch (v1)
    {
    case 0:
    percentResult = LCGPercentage(seed);
    if (percentResult >= 0.5){
            //direction == right
        } else {
            //direction == left.
        }
        break;
    case 1:
    percentResult = LCGPercentage(seed);
    if (percentResult >= 0.5){
            //direction == right
        } else {
            //direction == left.
        }
        break;
    case 2:
    percentResult = LCGPercentage(seed);
    if (percentResult >= 0.5){
            //direction == right
        } else {
            //direction == left.
        }
        break;
    case 3:
    percentResult = LCGPercentage(seed);
    if (percentResult >= 0.5){
            //direction == right
        } else {
            //direction == left.
        }
        break;
    case 4:
    
        v1 = 1;
        float f3x, f3y, f3z;
        //The game allocates these values in a very weird way. 
        float f2 = 100; //becomes starting and ending zoom (i think)
        float f1 = 27.5; //becomes starting and ending height at 0x808335E4 in memory, 801a6324 in code.
        float f0 = 65; //becomes starting and ending radius.  

        //flips coin for direction.
        //**technically I think this is flipping which position is the start and end position.
        //For all practical purposes this is what it means.
        percentResult = LCGPercentage(seed);
        if (percentResult >= 0.5){
            //direction == right
        } else {
            //direction == left.
        }
        break;

    case 5:
        percentResult = LCGPercentage(seed);
        if (percentResult < 0.5){
            //fvar1 = -1 -- Opponent
        } else {
            //fvar1 = 1 -- Player
        }
        //direction is always towards the pokemon/trainer.
        LCGPercentage(seed); //This is percentage in the code. Impacts speed of the movement, 
        //but tbf it doesn't make a big difference to the naked eye.
        //Unless you care about the specific frames for something (like a full tracking), then this can be a regular LCG(seed);
        // if one does care, then will need to take data because the code is wacky.
        //Direction is Always towards the target.
        break;
    case 6:
        percentResult = LCGPercentage(seed);
        if (percentResult < 0.5){
            //fvar1 = -1 -- Opponent
        } else {
            //fvar1 = 1 -- Player
        }
        //direction is always inside to out.
        LCG(seed); //another speed check
        break;
    case 7:
        percentResult = LCGPercentage(seed);
        if (percentResult < 0.5){
            //fvar1 = -1 -- Opponent
        } else {
            //fvar1 = 1 -- Player
        }
        //direction is always inside to out. again
        LCG(seed); //another speed check
        break;
    case 8:
        //no distance is moved on X which is derived from opponent/player check, Z values are fixed.
        LCG(seed); //Gets value of ending Y
        LCG(seed); //Gets value of starting Y
        percentResult = LCGPercentage(seed);
        if (percentResult < 0.5){ //Direction check.
            //Starting will be positive, ending will be negative
            //this means camera will move from nearside to farside
        } else {
            //Starting will be negative, ending will be positive
            //Camera will move from farside to nearside.
            //I think. Might depend on which side is targeted?
        }
        percentResult = LCGPercentage(seed);
        if (percentResult < 0.5){
            //fvar1 = -1 -- Opponent
        } else {
            //fvar1 = 1 -- Player
        }
        LCG(seed); //another speed check
        break;
    case 9:
        percentResult = LCGPercentage(seed);
        if (percentResult < 0.5){
            //fvar1 = -1 -- Opponent
        } else {
            //fvar1 = 1 -- Player
        }
        //Y pos is set from the first RNG call. 
        //starting and ending have their signs set so that the camera moves from nearside to farside always.
        LCG(seed); //another speed check
        break;
    case 10:
        
        break;
    default:
        break;
    } 
    /*
    Cases 10 and 11 break from the standard structure of cam movements. 
    Positive and negatives are the same as overworld movement. Positive Y is closer to camera, positive x is right-> direction. 
    
       NEG   
    NEG | POS
       POS

    Weird xyz allocation:
    F0: 
        first --- Ending Y
        second --- Starting X
    F2: first --- Ending X
        second --- Starting Y
    F1: first --- Ending Z
        second --- Starting Z
    Does all end xzy, then all start xzy.
    starts at: 801a6314



    

    
    */
    
    
    
    
    
    
    
    
    
    
    
    
    LCG(seed);
    if (v1 >= 5){
        LCG(seed);
        if (v1 == 8){
            LCGn(seed,3);
        }
        if (v1 >= 10){
            LCGn(seed,2);
        } 
    }
}





int main () {
    






    return 0;
}