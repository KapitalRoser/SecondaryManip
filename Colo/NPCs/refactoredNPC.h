#ifndef NPC_H
#define NPC_H
/*
THIS IS A LOCAL COPY OF THE HEADER.
*/
#include "../../processCore.h"
#include "duration.h"
enum state {WALK,WAIT,BEGIN,FINISH};
enum commonSpeed{STANDARD,SLOWER};
std::map<commonSpeed,float> walkingSpeed {
    {STANDARD,0.29032257199287415},
    {SLOWER,0.28125}
    }; //add as more speeds are found.

class d_coord {
    public:
    double x,y; //add a toFloat or toF_Coord function?
    f_coord toFCoord(){
        f_coord r;
        r.x = float(x);
        r.y = float(y);
        return r;
        // oneliner:
        //return f_coord({float(x),float(y)});
    };
};
class f_coord {
    public:
    float x,y;
    d_coord toDCoord(){
        d_coord r;
        r.x = double(x);
        r.y = double(y);
        return r;
    }
    //blame powerpc data type rounding.
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
    NPC(d_coord anchor);
    NPC(d_coord anchor, commonSpeed speed);
    NPC(d_coord anchor, commonSpeed speed, int ID);
    NPC(d_coord anchor, commonSpeed speed, std::string name);
    NPC(d_coord anchor, commonSpeed speed, int ID, std::string name); //declare optionals here?
    
    void InitialXY(u32 &seed);
    void chooseDestination(u32 &seed);

    void angleLogic(float angle);
    float computeAngle(d_coord distance);

    d_coord computeInterval();
    void applyStep(int factor);
    double combineDistance (d_coord distance);
    bool incrementPosition(int factor);
    
    float waitTimerCalculation(u32 &seed);
    void decrementWaitTimer();

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