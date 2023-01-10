#ifndef NPC_H
#define NPC_H
/*
THIS IS A LOCAL COPY OF THE HEADER.
*/
#include "../../processCore.h"
enum state {WALK,WAIT,BEGIN,FINISH};
const float common_speeds[3] = {0.29032257199287415,0.28125}; //add to this as more speeds are recorded.
enum commonSpeed{STANDARD,SLOWER};
class d_coord {
    public:
    double x,y;
};
class f_coord {
    public:
    float x,y;
    //blame powerpc data type rounding.
};
class duration {
    public:
    //This is super important to get right, as the numbers produced here will directly impact rng manip in the future!
    duration(float m_seconds){
        setFrames30(round(m_seconds/(1.0/30))); //This only seems to matter when it comes to 60fps?
        setSeconds(m_seconds); //The 60fps number would depend on whether on an even frame or odd frame, again shouldn't matter right?
    }
    duration(int inputframes30){
        setSeconds((1.0/30)*inputframes30);
        setFrames30(inputframes30);
    }
    //Shouldn't ever call duration with a 60fps number, right? if so then add a /2 in the parameter.
    int getFrames30(){return m_frames30;}
    int getFrames60(){return m_frames30*2;}
    float getFrames60fromSeconds(){
        return round(m_seconds/(1.0/60)); //need to do more testing to see if this is correct.
    }
    float getSeconds(){return m_seconds;}
    void setFrames30(int input){m_frames30 = input;}
    void setSeconds(float input){m_seconds = input;}
    private:
    float m_seconds = 0;
    int m_frames30 = 0;
};
class NPC {
    private:
    d_coord m_anchor = {0,0};
    d_coord m_intervalValues = {0,0};
    f_coord m_nextPos = {0.0};
    f_coord m_intendedPos = {0,0};
    
    float m_angle = 0;
    float m_speedFactor = 0;
    duration m_waitTime = duration{0};
    duration m_walkTime = duration{0};
    d_coord m_CombinedDistances = {0,0};
    int m_state = BEGIN;
    std::string m_name = "";
    int m_ID = 0;

    public:
    NPC(d_coord anchor){
        setAnchor(anchor);
        f_coord temp;
        temp.x = anchor.x;
        temp.y = anchor.y;
        setNextPos(temp);
        setSpeed(common_speeds[STANDARD]);
    }
     NPC(d_coord anchor, int speedIdx){
        setAnchor(anchor);
        f_coord temp;
        temp.x = anchor.x;
        temp.y = anchor.y;
        setNextPos(temp);
        setSpeed(common_speeds[speedIdx]);
    }
    NPC(d_coord anchor,int speedIdx, int ID){
        setAnchor(anchor);
        f_coord temp;
        temp.x = anchor.x;
        temp.y = anchor.y;
        setNextPos(temp);
        setID(ID);
        setSpeed(common_speeds[speedIdx]);
    }
    NPC(d_coord anchor, int speedIdx, std::string name){
        setAnchor(anchor);
        f_coord temp;
        temp.x = anchor.x;
        temp.y = anchor.y;
        setNextPos(temp);
        setName(name);
        setSpeed(common_speeds[speedIdx]);
    }
    void InitialXY(u32 &seed){
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
    
    float computeAngle(){
        //This is the angle used for interval calculation, but isn't recorded in the array.
        d_coord preAngles = getDistance();
        double angle = atan2(preAngles.x,preAngles.y)/2;
        return angle;
    }
    d_coord computeInterval(){
        const double loosePiApprox = 3.1415927410125732421875; //40490FDB
        //Could declare a ** operator as a power shorthand.
        d_coord intervals;
        float intervalAngle = computeAngle();
        // std::cout << "CURRENT ANG"<< getAngle() << " NEW ANG: " << intervalAngle;
        if (getAngle() > loosePiApprox || getAngle() < -loosePiApprox){
            setAngle(intervalAngle);
        }
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
    void applyStep(int factor){
        f_coord postStepPos = getNextPos();
        postStepPos.x += getInterval().x * factor;
        postStepPos.y += getInterval().y * factor;          
        setNextPos(postStepPos);
    }
    double combineDistance (d_coord distance){
        return sqrt(distance.x*distance.x + distance.y*distance.y);
    }
    bool incrementPosition(int factor){
        double preStep = combineDistance(getDistance());
        applyStep(factor);
        double postStep = combineDistance(getDistance());
    
        setCombinedDistance({preStep,postStep});
        //setName("Pre: " + std::to_string(preStep) + "Post: "+std::to_string(postStep));
        if (postStep <= preStep){
            setWalkTime(getWalkTime().getFrames30()+1);
            return true;
        } else {
            setState(FINISH);
            return false;      
        }
    }
    float waitTimerCalculation(u32 &seed){
        const int factorTime = 3;//these don't seem to vary by npc but who knows.
        const int baseTimeS = 5; 
        double firstCall = LCG_PullHi16(seed);
        double secondCall = LCG_PullHi16(seed);
        double cycleVariance = firstCall + secondCall - 1;
        return cycleVariance * factorTime + baseTimeS; //time in seconds.
    }
    void decrementWaitTimer(){
        float decrement = 0.0333333350718021392822265625; //float?
        setWaitTime(getWaitTime().getSeconds() - decrement);
        // if (getWaitTime().getFrames30() <= 0){
        //     setState(BEGIN);
        // }
        if (getWaitTime().getSeconds() <= 0){
            setState(BEGIN);
        }
        //alternatively decrement by a certain number of ms.
    }
    void beginCycle(u32 &seed){
        setState(WALK);
        InitialXY(seed);
        setIntervals(computeInterval());
        setWaitTime(0);
        setWalkTime(0);
    }
    void finishCycle(u32 &seed){
        setState(WAIT);
        setWaitTime(waitTimerCalculation(seed));
        setNextPos(getIntendedPos()); //snap to account for overshoot.
    }
    //putting it all together
    void initializeNPC_Self(u32 &seed){
        beginCycle(seed);
        npcAction_Self(seed,0); //first two steps happen on first frame
        npcAction_Self(seed,1);
        //Should I be adding a action parameter + return?
    }
    std::string npcAction_Self(u32 &seed, int i){
        std::string action = ""; //optional -- exists only for debugging.
        int intervalFactor = 2;
        if (i == 0){
            intervalFactor = 1;
        }
        else if (i == 1){
            intervalFactor = 5;
        }
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
                incrementPosition(intervalFactor);
                break;
            default:
                std::cout << "STATE INVALID!";
                break;
            }
            return action;
    }

    void printNPCData(int currentCycle){
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

    void setAnchor(d_coord input) {m_anchor = input;} //uses double instead of float or int because idk maybe there's an npc with a very specific anchor location. I doubt it but why take the risk lol.
    void setAngle(float input){m_angle = input;}
    void setCombinedDistance(d_coord input){m_CombinedDistances = input;}
    void setIntended(f_coord input){m_intendedPos = input;}
    void setIntervals(d_coord input){m_intervalValues = input;}
    void setNextPos(f_coord input){m_nextPos = input;}
    void setState (int input){m_state = input;}
    void setWaitTime(duration input){m_waitTime = input;}
    void setWalkTime(duration input){m_walkTime = input;}
    void setName(std::string input){m_name = input;}
    void setID(int input){m_ID = input;}
    void setSpeed(float input){m_speedFactor = input;}
    
    int getID(){return m_ID;}
    int getState(){return m_state;}
    float getAngle(){return m_angle;}
    float getSpeedFactor(){return m_speedFactor;}
    std::string getName(){return m_name;}
    d_coord getAnchor(){return m_anchor;}
    d_coord getCombinedDistance(){return m_CombinedDistances;}
    d_coord getDistance(){
        return {getIntendedPos().x - getNextPos().x, 
                getIntendedPos().y - getNextPos().y};
    }
    d_coord getInterval(){return m_intervalValues;}
    f_coord getIntendedPos(){return m_intendedPos;}
    f_coord getNextPos(){return m_nextPos;}
    duration getWaitTime(){return m_waitTime;}
    duration getWalkTime(){return m_walkTime;}
};

#endif /*NPC_H*/