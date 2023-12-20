#include "NPC.h"
/*
    Dear reader, you will notice a lot of weird conversions between Float and Double objects. This is intentional.
    Dolphin/GC often rounds intermediate calculations and I've done my best to preserve them while also condensing the math.

    JIM STUCK COORDS: (8.51607227,-13.769783), ALSO (8.4991436,-13.7876158)
    JIM INTENDED WHEN STUCK: (7.88849592,-23.2070618)
    JIM ANGLE*2 WHEN STUCK: (-2.9532485)
    JIM TIMER: 0

    Will need to settle on an implementation for a multiple npc handler object. Not sure what to call it?
    Pack? Group? Set? Crew? Suite?
    That object would be seeded and would ideally be easier to add into whatever code the programmer is building for their sim.
    Frame-by-frame or waterfall calculation versions?
*/



//~~~~~~~~~~~~ Duration implementation ~~~~~~~~~~~~~~~~~~~~ THIS COULD GO IN PROCESS CORE.

duration::duration(float m_seconds){
        setFrames30(round(m_seconds/(1.0/30))); //This only seems to matter when it comes to 60fps?
        setSeconds(m_seconds); //The 60fps number would depend on whether on an even frame or odd frame, again shouldn't matter right?
}
duration::duration(int inputframes30){
        setSeconds((1.0/30)*inputframes30);
        setFrames30(inputframes30);
}

float duration::getFrames60fromSeconds(){
    return round(m_seconds/(1.0/60)); //need to do more testing to see if this is correct.
}

int duration::getFrames30(){return m_frames30;}
int duration::getFrames60(){return m_frames30*2;}
float duration::getSeconds(){return m_seconds;}

void duration::setFrames30(int input){m_frames30 = input;}
void duration::setSeconds(float input){m_seconds = input;}

//~~~~~~~~~~~~ COMPONENT FUNCTIONS ~~~~~~~~~~~~~~~~~~~~

void NPC::chooseDestination(u32 &seed){
    //Should work but need to test that the float and double conversions work out the same.
    const double importantPiApprox = 3.1415927410125732421875; //40490FDB
    const int factor = 15;
    f_coord destinationPos;
    float f_result = LCG_PullHi16(seed) * importantPiApprox * 2; //Can we swap this for LCG_Percentage?
    destinationPos.x = double(sin(f_result)*factor) + getAnchor().x;
    destinationPos.y = double(cos(f_result)*factor) + getAnchor().y;
    setIntended(destinationPos);
    setAngle(angleLogic(f_result)); //unsure if necessary?
}

f_coord NPC::validatePosition(f_coord inputPos, bool XorY){
    //This is a function owned by the npc. 
    //This function returns a "corrected" coordinate pair as f_coord (if a correction is needed), 
    //If no correction is needed, then return input pair.
    //Programmer/user provides custom function, tailored to the npc and situation you're in.
    //Somewhat presuming that better calculation is done with doubles, not floats. However the rounding may wreck us in the end?
    if (validationFunctionPtr == nullptr){
        return inputPos;
    }
    d_coord result = (*validationFunctionPtr)(inputPos.toDCoord(),XorY);
    //BE REALLY CAREFUL WITH THIS ROUNDING IF COMING OFF OF A D_COORD.
    f_coord resF;
    resF.x = result.x;
    resF.y = result.y;
    return resF;
}

//~~~~~~~~~~~~~~~~INTERVAL STUFF~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
d_coord NPC::computeInterval(){
    float intervalAngle = computeAngle(getDistance());
    setAngle(intervalAngle*2); //This is what shows up in the array, not strictly needed for calculation. Still need base angle next. 

    const float adjustment = 0.9999999403953552;//Constant  -- this is apparently 1 - 1/2^24, an extremely minor adjustment.
    float sinResult = static_cast<float>(sin(intervalAngle)) * adjustment; //Note this is angle, not to be confused with angle*2 which is what set(angle) sets.
    float cosResult = static_cast<float>(cos(intervalAngle));
    const float speedFactor = getSpeedFactor(); //thanks heels.
    //Trigonometry identities (Angle Addition Formulas I think) used:
    //x = 2*sinx*cosx
    //y = cosx^2-sinx^2
    //note that speedfactor is NOT the x here, the x is interval angle so it's already built in.
    d_coord intervals;
    intervals.x = 2*sinResult*cosResult*speedFactor;
    intervals.y = (pow(cosResult,2)-pow(sinResult,2))*speedFactor;    
    return intervals;
    //Now that we have interval and its a fixed number for all steps, can we simply divide distance by interval to get # frames?
}

void NPC::applyStep(int factor){
        f_coord postStepPos = getNextPos();//collision check occurs before either pos is set I think...
        postStepPos.x += getInterval().x * factor;
        postStepPos = validatePosition(postStepPos, 1);
        postStepPos.y += getInterval().y * factor;
        postStepPos = validatePosition(postStepPos, 0);       
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
        //setName("Pre: " + std::to_string(preStep) + "Post: "+std::to_string(postStep));
        return evaluateStop(preStep,postStep); //Walk Time can get incremented here!
    }
bool NPC::evaluateStop(double pre, double post){
        post <= pre ? setWalkTime(getWalkTime().getFrames30()+1) : setState(FINISH);
        return post <= pre; //Evaluates stop condition. Only continue moving if distance is decreasing.
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
    d_coord preAngles = distance;
    double angle = atan2(preAngles.x,preAngles.y)/2;
    return angle;
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

std::string NPC::npcAction_Self(u32 &seed){
        std::string action = ""; //optional -- exists only for debugging.
        int intervalFactor = 2; //Standard?
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
                incrementPosition(intervalFactor); //standard practice?
                incrementPosition(intervalFactor); //if all npcs do this, then move this into beginCycle().
                break;
            case FIRST:
                action = "!!";
                beginCycle(seed);
                incrementPosition(intervalFactor = 1); //special cases -- sometimes a particular npc will ignore?
                incrementPosition(intervalFactor = 5);
                setState(FIRST_WALK); //overrides beginCycle()
                break;
            case FIRST_WALK:
                action = "**"; //If not applicable to all maps, how can the user customize this?
                incrementPosition(intervalFactor = 1);
                setState(SECOND_WALK);
                break;
            case SECOND_WALK:
                action = "**";
                incrementPosition(intervalFactor = 5);
                setState(WALK); // This is unscientific, really should be mirroring the game better.s
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