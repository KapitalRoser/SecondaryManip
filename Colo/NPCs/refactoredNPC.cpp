#include "../../processCore.h"
#include "refactoredNPC.h"

/*
    LIST OF THINGS TO VERIFY:
    1) Make sure new constructor produces the same results as the old constructors. 
    If so, use the new single constructor for everything

    2) Check that chooseDestination produces the same results as InitialXY(),
    especially given the sensitive double and float conversions that happen ingame.

    3) See if the angle matters at all to the end result. If not, then discard or make optional.
    If must keep them then can simplify greatly. Also make sure results are the same as old stuff.

    4)Test ComputeInterval to see if that angle validity check is needed, since currently it gets
    immediately overwritten.

    5)Make sure FIRST state performs the same as calling initializeNPCSelf()

    //RAN 1 TEST, NO DIFFS SO FAR.
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
    {}
     //empty body! nice!
    //Only anchor should be mandatory, everything else is set to default values if not specified,
    //and all initialization is done in the list.
    //Once again, does the optional parameter need to be specified in the cpp or in the .h?
    //Qt makes me think .h but Im not positive.

//~~~~~~~~~~~~ COMPONENT FUNCTIONS ~~~~~~~~~~~~~~~~~~~~


void NPC::InitialXY(u32 &seed){
    const double loosePiApprox = 3.1415927410125732421875; //40490FDB
    const double twoPi = 6.28318530717958623199592693708837032318115234375;
    const int factor = 15;
    f_coord destinationPos = getIntendedPos();
    float f_working = LCG_PullHi16(seed);
    f_working = f_working * loosePiApprox * 2;

    //array angle
    //std::cout << "FWORKING: " << f_working;
    //std::cout << "F WORKING:" << f_working << "\n";
    if (f_working > loosePiApprox || f_working < -loosePiApprox){
        setAngle(f_working - twoPi);
        //std::cout << "ANGLE SET CASE 1:" << f_working - twoPi;
    } else {
        setAngle(f_working); 
        //std::cout << "ANGLE SET CASE 2:" << f_working;
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
    float f_result = LCG_PullHi16(seed) * importantPiApprox * 2;
    destinationPos.x = double(sin(f_result)*factor) + getAnchor().x;
    destinationPos.y = double(cos(f_result)*factor) + getAnchor().y;
    setIntended(destinationPos);
    setAngle(angleLogic(f_result));
}



//~~~~~~~~~~~~~~~~INTERVAL STUFF~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
d_coord NPC::computeInterval(){
        
        float intervalAngle = computeAngle(getDistance());
        // std::cout << "CURRENT ANG"<< getAngle() << " NEW ANG: " << intervalAngle;

        //slightly different to Angle Logic
        // const double loosePiApprox = 3.1415927410125732421875; //40490FDB
        // if (getAngle() > loosePiApprox || getAngle() < -loosePiApprox){
        //     setAngle(intervalAngle);
        // } //Currently, this if block gets overwritten by the next line anyway so what's the point.
        setAngle(intervalAngle*2);

        float sinResult = static_cast<float>(sin(intervalAngle)) * 0.9999999403953552;
        float cosResult = static_cast<float>(cos(intervalAngle));
        const float speedFactor = getSpeedFactor(); //thanks heels.
        //identities used:
        //x = 2*sinx*cosx
        //y = cosx^2-sinx^2
        d_coord intervals;
        intervals.x = 2*sinResult*cosResult*speedFactor; // 2*sinx*cosx is a TRIGONOMETRY IDENTITY, Angle addition formula?
        intervals.y = (pow(cosResult,2)-pow(sinResult,2))*speedFactor;//cos^2*X-sin^2*x, note that speedfactor is NOT the x here, the x is interval angle so it's already built in.
        //should be fine now.
        //std::cout << "INTERVAL ANGLE: " << std::setprecision(17) << intervalAngle*2 << std::endl;
        // << "SINRESULT: " << sinResult << " COSRESULT: " << cosResult << std::endl;
        return intervals;
        //Now that we have interval and its a fixed number for all steps, can we simply divide distance by interval to get # frames?
}

void NPC::applyStep(int factor){
        f_coord postStepPos = getNextPos();
        postStepPos.x += getInterval().x * factor;
        postStepPos.y += getInterval().y * factor;          
        setNextPos(postStepPos);
    }
double NPC::pythagorasDistance (d_coord distance){
    return sqrt(pow(distance.x,2) + pow(distance.y,2)); //A^2 + B^2 = C^2, nice pythagoras!
}
bool NPC::incrementPosition(int factor){
        double preStep = pythagorasDistance(getDistance());
        applyStep(factor);
        double postStep = pythagorasDistance(getDistance());
    
        setCombinedDistance({preStep,postStep}); //saves as d_coord, DOES THIS EVER GET READ??
        //

        //setName("Pre: " + std::to_string(preStep) + "Post: "+std::to_string(postStep));

        return shouldStop(preStep,postStep); //Walk Time gets incremented here!
    }
bool NPC::shouldStop(double pre, double post){
        post <= pre ? setWalkTime(getWalkTime().getFrames30()+1) : setState(FINISH);
        return post <= pre;
}

//~~~~~~~~~~~~~~~~~~~~~~~~ANGLE STUFF~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
float NPC::angleLogic(float angle){
    const double loosePiApprox = 3.1415927410125732421875; //40490FDB
    const double twoPi = 6.28318530717958623199592693708837032318115234375;
    if (angle > loosePiApprox || angle < -loosePiApprox){
        //std::cout << "ANGLE SET CASE 1: " << angle-twoPi << "\n";
        return angle - twoPi;
    } else {
        //std::cout << "Angle SET CASE 2: " << angle << "\n";
        return angle; 
    }
    //Compute Intervals version is different and not sure if it's really needed or not.
}

float NPC::computeAngle(d_coord distance){
    //This is the angle used for interval calculation, but isn't recorded in the array.
    d_coord preAngles = distance;
    double angle = atan2(preAngles.x,preAngles.y)/2;
    return angle; //what if stored result as an f_coord instead? Or do we need the truncation from double to f AFTER the division by 2? Gets casted to F for return anyway???
    //return double(atan2(getDistance().x,getDistance().y)/2); //Does double into float conversion matter?
    //return atan2(distance.x,distance.y)/2; //???
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

void NPC::printNPCData(int currentCycle){
    std::cout << "DEST X POS: " << std::setprecision(17) << getIntendedPos().x << std::endl;
    std::cout << "DEST Y POS: " << std::setprecision(17) << getIntendedPos().y << std::endl;
    std::cout << std::endl;
    std::cout << "It took " << getWalkTime().getFrames60() << " 60fps frames to arrive.\n"; 
    std::cout << "It took " << getWalkTime().getFrames30() << " 30fps frames to arrive.\n\n";
    std::cout << "Returned angle: " << getAngle() << std::endl;
    std::cout << "Cycle: " << currentCycle << ": Timer1 is: " << std::setprecision(17) << getWaitTime().getSeconds() << std::endl;
    std::cout << "At 60fps, this is: " << getWaitTime().getFrames60fromSeconds() << " frames.\n";
    std::cout << "At 30fps, this is: " << getWaitTime().getFrames30() << " frames.\n";
    std::cout << "\n~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ \n\n";
}



//ACTION FUNCTIONS.
void NPC::beginCycle(u32 &seed){
    setState(WALK);
    chooseDestination(seed);
    setIntervals(computeInterval());
    setWaitTime(0); //reset for next cycle
    setWalkTime(0);
}
void NPC::finishCycle(u32 &seed){
    setState(WAIT);
    setWaitTime(waitTimerCalculation(seed));
    setNextPos(getIntendedPos()); //snap to destination to account for overshoot. -- this is behaviour observed in game.
}

void NPC::initializeNPC_Self(u32 &seed){
    npcAction_Self(seed,2);
    // beginCycle(seed); //State is set to "Walk"
    // npcAction_Self(seed,0); //first two steps happen on first frame
    // npcAction_Self(seed,1);
    //Should I be adding a action parameter + return?
}

std::string NPC::npcAction_Self(u32 &seed, int frameNum){
        std::string action = ""; //optional -- exists only for debugging.
        int intervalFactor = 2;
        switch (getState()) //these are seperate states so that the actions happen on unique frames.
            { 
            case WALK:
                action = "**";
                incrementPosition(intervalFactor);
                break;
            case WAIT:
                action = "--";
                decrementWaitTimer();
                break;
            case FINISH:
                action = getName() + "f";
                finishCycle(seed);
                break;
            case BEGIN:
                action = getName() + "b";
                beginCycle(seed);
                incrementPosition(2); //standard practice?
                incrementPosition(intervalFactor); //if all npcs do this, then move this into beginCycle().
                break;
            case FIRST:
                action = "**";
                beginCycle(seed); //if user defined seed, this should be it. Otherwise use current seed
                incrementPosition(intervalFactor = 1); //special cases
                incrementPosition(intervalFactor = 5);
                break;
            default:
                std::cout << "STATE INVALID!";
                break;
            }
            return action;
    }

/*
    Basic workflow is like this:
    CREATE NPC OBJECT: GIVE ANCHOR AND OPTIONAL ARGUMENTS FOR IDENTIFICATION
        Highly recommend specifying a speed as well. Standard assumed otherwise.
    NPC IS GIVEN A SEED: THIS CREATES POSITION AND INTERVAL VALUES (as well as angle as a byproduct)
        Should compare the interval value with the speedfactor to see if walk time in frames can just
        use distance / speedfactor or something. 
    NPC THEN USES ACTION FUNCTIONS TO DESCRIBE IT'S POSITION EACH FRAME.
        Still need a solution for the Jim getting stuck and Randall getting pushed out by the curve.



*/