#include "../../processCore.h"
#include "duration.h"
#include "refactoredNPC.h"

/*
    LIST OF THINGS TO VERIFY:
    1) Make sure new constructor produces the same results as the old constructors. 
    If so, use the new single constructor for everything

    2) Check that chooseDestination produces the same results as InitialXY(),
    especially given the sensitive double and float conversions that happen ingame.

    3) See if the angle matters at all to the end result. If not, then discard or make optional.
    If must keep them then can simplify greatly. Also make sure results are the same as old stuff.

    4)
*/




NPC::NPC(d_coord anchor){
    setAnchor(anchor);
    f_coord temp;
    temp.x = anchor.x;
    temp.y = anchor.y;
    setNextPos(temp);
    setSpeed(walkingSpeed[STANDARD]);
}
    NPC::NPC(d_coord anchor, commonSpeed speed){
    setAnchor(anchor);
    f_coord temp;
    temp.x = anchor.x;
    temp.y = anchor.y;
    setNextPos(temp);
    setSpeed(walkingSpeed[speed]);
}
NPC::NPC(d_coord anchor, commonSpeed speed, int ID){
    setAnchor(anchor);
    f_coord temp;
    temp.x = anchor.x;
    temp.y = anchor.y;
    setNextPos(temp);
    setID(ID);
    setSpeed(walkingSpeed[speed]);
}
NPC::NPC(d_coord anchor, commonSpeed speed, std::string name){
    setAnchor(anchor);
    f_coord temp;
    temp.x = anchor.x;
    temp.y = anchor.y;
    setNextPos(temp);
    setName(name);
    setSpeed(walkingSpeed[speed]);
}

NPC::NPC(d_coord anchor, commonSpeed speed = STANDARD, int ID = 0, std::string name = "NAME_NOT_SET") :
    m_anchor(anchor),
    m_nextPos({float(anchor.x),float(anchor.y)}), //returns valid f_coord?
    m_ID(ID),
    m_name(name),
    m_speedFactor(walkingSpeed[speed])
    {} //empty body! nice!
    //Only anchor should be mandatory, everything else is set to default values if not specified,
    //and all initialization is done in the list.
    //Once again, does the optional parameter need to be specified in the cpp or in the .h?
    //Qt makes me think .h but Im not positive.

void NPC::InitialXY(u32 &seed){
    const double loosePiApprox = 3.1415927410125732421875; //40490FDB
    const double twoPi = 6.28318530717958623199592693708837032318115234375;
    const int factor = 15;
    f_coord destinationPos = getIntendedPos();
    float f_working = LCG_PullHi16(seed);
    f_working = f_working * loosePiApprox * 2;

    //array angle
    //std::cout << "FWORKING: " << f_working;
    if (f_working > loosePiApprox || f_working < -loosePiApprox){
        setAngle(f_working - twoPi);
    } else {
        setAngle(f_working); 
    }

    double d_working = f_working; //***sensitive conversion between float -> double and back.
    d_working = sin(d_working) * factor;
    destinationPos.x = d_working;

    d_working = f_working; //restore
    d_working = cos(d_working)* factor;
    destinationPos.y = d_working;

    destinationPos.x += getAnchor().x;
    destinationPos.y += getAnchor().y;
    setIntended(destinationPos); //for walking calculations
}

void NPC::chooseDestination(u32 &seed){
    //Should work but need to test that the float and double conversions work out the same.
    const double importantPiApprox = 3.1415927410125732421875; //40490FDB
    const int factor = 15;
    f_coord destinationPos;
    float f_result = LCG_PullHi16(seed);
    destinationPos.x = double(sin(f_result*importantPiApprox)*factor) + getAnchor().x;
    destinationPos.y = double(cos(f_result*importantPiApprox)*factor) + getAnchor().y;
    //OPTIONAL: -- does the angle and such have any impact on the overall result?
    angleLogic(f_result);
}

d_coord NPC::computeInterval(){
        const double loosePiApprox = 3.1415927410125732421875; //40490FDB
        //Could declare a ** operator as a power shorthand.
        d_coord intervals;
        float intervalAngle = computeAngle();
        // std::cout << "CURRENT ANG"<< getAngle() << " NEW ANG: " << intervalAngle;
        //slightly different to Angle Logic
        if (getAngle() > loosePiApprox || getAngle() < -loosePiApprox){
            setAngle(intervalAngle);
        } //Currently, this if block gets overwritten by the next line anyway so what's the point.
        setAngle(intervalAngle*2);
        float sinResult = static_cast<float>(sin(intervalAngle)) * 0.9999999403953552;
        float cosResult = static_cast<float>(cos(intervalAngle));
        const float speedFactor = getSpeedFactor(); //thanks heels.
        intervals.x = 2*sinResult*cosResult*speedFactor;
        intervals.y = -(pow(sinResult,2)*speedFactor - pow(cosResult,2)*speedFactor);
        //std::cout << "INTERVAL ANGLE: " << std::setprecision(17) << intervalAngle*2 << std::endl;
        // << "SINRESULT: " << sinResult << " COSRESULT: " << cosResult << std::endl;
        return intervals;
}

void NPC::angleLogic(float angle){
    const double loosePiApprox = 3.1415927410125732421875; //40490FDB
    const double twoPi = 6.28318530717958623199592693708837032318115234375;
    if (angle > loosePiApprox || angle < -loosePiApprox){
        setAngle(angle - twoPi);
    } else {
        setAngle(angle); 
    }
    //Compute Intervals version is different and not sure if it's really needed or not.
}

float NPC::computeAngle(){
    //This is the angle used for interval calculation, but isn't recorded in the array.
    d_coord preAngles = getDistance();
    double angle = atan2(preAngles.x,preAngles.y)/2;
    return angle;
    //return double(atan2(getDistance().x,getDistance().y)/2); //Does double into float conversion matter?
}

float NPC::waitTimerCalculation(u32 &seed){
    const int factorTime = 3;//these don't seem to vary by npc but who knows.
    const int baseTimeS = 5; 
    double firstCall = LCG_PullHi16(seed);
    double secondCall = LCG_PullHi16(seed);
    double cycleVariance = firstCall + secondCall - 1;
    return cycleVariance * factorTime + baseTimeS; //time in seconds.
    //one liner, assuming it works:
    //return (LCG_PullHi16(seed) + LCG_PullHi16(seed) - 1)*3 + 5;
}
void NPC::decrementWaitTimer(){
    const float decrement = 0.0333333350718021392822265625; //float?
    setWaitTime(getWaitTime().getSeconds() - decrement);
    if (getWaitTime().getSeconds() <= 0){
        setState(BEGIN);
    }
    //alternatively decrement by a certain number of ms.
}