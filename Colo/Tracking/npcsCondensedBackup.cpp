#include "../../processCore.h"

double pullHi16 (uint32_t &seed, int divisor){
    double X = 0;
    LCG(seed);
    X = seed >> 16;
    X = X/divisor;
    return X;
}
float stupidFloatRounding(float inputF12,float inputF10,float inputF9){   
      float factor1 = inputF12 * inputF10;
      float factor2 = inputF9 * inputF10;
      float subtrahend = factor2 * inputF9;
      return -(inputF12*factor1 - subtrahend);
}
void InitialXY(float &intendedX, float &intendedY, u32 &seed, int divisor, double loosePiApprox, double anchorX, double anchorY){
    float f_working = 0;
    double d_working = 0;
    //sensitive conversion between float -> double and back.
    f_working = pullHi16(seed,divisor);
    f_working = f_working * loosePiApprox * 2;

    d_working = f_working;
    d_working = sin(d_working) * 15;
    intendedX = d_working;

    d_working = f_working;
    d_working = cos(d_working)* 15;
    intendedY = d_working;

    intendedX += anchorX;
    intendedY += anchorY;

    std::cout << "DEST X POS: " << std::setprecision(17) << intendedX << std::endl;
    std::cout << "DEST Y POS: " << std::setprecision(17) << intendedY << std::endl;
}
float walkTimerCalculation(u32 &seed,float timer1,int divisor, int currentCycle){
    const int factorTime = 3;
    const int baseTimeS = 5;
    double firstCallX = pullHi16(seed,divisor);
    double secondCallX = pullHi16(seed,divisor);
    double cycleVariance = firstCallX + secondCallX - 1;
    return cycleVariance * factorTime + baseTimeS;
}
float computeAngle(float &intendedX, float &intendedY,double nextX, double nextY){
    double preAngle1 = intendedX - nextX; //replace with distance?
    double preAngle2 = intendedY - nextY;
    std::cout << "PreAngle1: " << preAngle1 << ". PreAngle2: " << preAngle1 << "\n";
     return atan2(preAngle1,preAngle2)/2;
}
void computeInterval(double angleInput, double &intervalValueX, double &intervalValueY){
    float sinResult = static_cast<float>(sin(angleInput)) * 0.9999999403953552;
    float cosResult = static_cast<float>(cos(angleInput));
    const float factorC = 0.29032257199287415;
    intervalValueX = 2*sinResult*cosResult*factorC;
    intervalValueY = stupidFloatRounding(sinResult,factorC,cosResult);
}
void takeStep (float &nextXPos, float &nextYPos,double intervalValueX, double intervalValueY,int factor){
      nextXPos = intervalValueX * factor + nextXPos;
      nextYPos = intervalValueY * factor + nextYPos;
}
double combineDistance (float distanceX, float distanceY){
    double sumSq = (distanceX*distanceX) + (distanceY*distanceY);
    double fractional = 1/sqrt(sumSq);
    double fractionalSq = fractional*fractional;
    double result = -(fractionalSq*sumSq-3) * (fractional/2) * sumSq;
    return result;
}
void updateDistance(double &distX, double &distY, float intendedX,float intendedY, float nextX, float nextY){
    distX = intendedX - nextX;
    distY = intendedY - nextY;
};

int main(){
    //CONSTANTS:
    const int divisor = 65536;
    const double loosePiApprox = 3.1415927410125732421875; //40490FDB
    //~~~~~~~~~~~~~~ CONFIG ~~~~~~~~~~~~~~~
    int cycleCount = 3;
    uint32_t seed = 0x20ff71DA; //INITIAL SEED  -- KR Seed: 20ff71DA -- matches emu behaviour exactly.
    const double anchorX = 4; //Starting Location 
    const double anchorY = 24;
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    float angleInput = 0;
    float intendedX = 0;
    float intendedY = 0;
    float nextXPos = anchorX;
    float nextYPos = anchorY;
    double intervalValueX = 0;
    double intervalValueY = 0;
    double distanceX = 0;
    double distanceY = 0;
    double preStepCombinedDist = 0;
    double postStepCombinedDist = 0;
    float timer1 = 0;
    int Stepfactor = 2;
    int framesWalked = 0;
    for (int i = 0; i<cycleCount;i++){
    //Setup
    InitialXY(intendedX,intendedY,seed,divisor,loosePiApprox,anchorX,anchorY); //Rolls RNG to find a destination, affects intendedX and intendedY
    angleInput = computeAngle(intendedX,intendedY,nextXPos,nextYPos);
    computeInterval(angleInput,intervalValueX,intervalValueY);
    updateDistance(distanceX,distanceY,intendedX,intendedY,nextXPos,nextYPos);

    //initial iteration
    preStepCombinedDist = combineDistance(distanceX,distanceY);
    if (i == 0){
        takeStep(nextXPos,nextYPos,intervalValueX,intervalValueY,Stepfactor/2);
    } else {
        takeStep(nextXPos,nextYPos,intervalValueX,intervalValueY,Stepfactor);
    }
    updateDistance(distanceX,distanceY,intendedX,intendedY,nextXPos,nextYPos);
    postStepCombinedDist = combineDistance(distanceX,distanceY);
    
    //main loop.
    do
    {
    preStepCombinedDist = combineDistance(distanceX,distanceY);
    takeStep(nextXPos,nextYPos,intervalValueX,intervalValueY,2);  //first step
    updateDistance(distanceX,distanceY,intendedX,intendedY,nextXPos,nextYPos);
    postStepCombinedDist = combineDistance(distanceX,distanceY);
    framesWalked++;
    } while (postStepCombinedDist <= preStepCombinedDist);

    //Must re-adjust for overshoot.
    nextXPos = intendedX;
    nextYPos = intendedY;
    timer1 = walkTimerCalculation(seed,timer1,divisor,i);
    //DONE! ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
    //print section
    std::cout << std::endl;
    std::cout << "It took " << framesWalked*2 << " 60fps frames to arrive.\n"; 
    std::cout << "It took " << framesWalked << " 30fps frames to arrive.\n\n";
    std::cout << "Returned angle: " << angleInput*2 << std::endl;
    //Display
    std::cout << "Cycle: " << i << ": Timer1 is: " << std::setprecision(17) << timer1 << std::endl;
    std::cout << "At 60fps, this is: " << round(timer1/(1.0/60)) << " frames.\n";
    std::cout << "At 30fps, this is: " << round(timer1/(1.0/30)) << " frames.\n";
    std::cout << "\n~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ \n\n";  
    }
    return 0;
}