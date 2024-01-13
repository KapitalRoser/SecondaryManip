#ifndef NPC_H
#define NPC_H

#include "processCoreLocal.h"

enum state {FIRST,FIRST_WALK,SECOND_WALK,WALK,WAIT,FINISH,BEGIN}; // First 3 states may not apply to every room?
enum commonSpeed{STANDARD,SLOWER}; //How many speeds exist? -- can't enum a float... so its used in a map.

//figure out circular dependency
class d_coord {
    public:
    double x,y; //add a toFloat or toF_Coord function?
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


class duration {
    public:
    //This is super important to get right, as the numbers produced here will directly impact rng manip in the future!
    duration(float m_seconds);
    duration(int inputframes30);
    //Shouldn't ever call duration with a 60fps number, right? if so then add a /2 in the parameter.
    //both XD and Colo have 30fps overworlds and there shouldn't be a reason in XD to ever track npcs in the pause menu.
    float getFrames60fromSeconds();
    int getFrames30();
    int getFrames60();
    float getSeconds();
    void setFrames30(int input);
    void setSeconds(float input);
    private:
    float m_seconds = 0;
    int m_frames30 = 0;
};


class NPC {
    private:
    d_coord m_anchor = {0,0};
    d_coord m_intervalValues = {0,0}; //More like a tuple
    f_coord m_nextPos = {0.0};
    f_coord m_intendedPos = {0,0};
    
    float m_angle = 0;
    float m_speedFactor = 0;
    duration m_waitTime = duration{0};
    duration m_walkTime = duration{0};
    d_coord m_CombinedDistances = {0,0}; //tuple
    int m_state = FIRST;
    std::string m_name = "";
    std::map <commonSpeed,float> walkingSpeed = {
        {STANDARD,0.29032257199287415},
        {SLOWER,0.28125}
    }; //add as more speeds are found.
    d_coord (*validationFunctionPtr)(d_coord pos, bool XorY) = nullptr;

    public:

    //NPC(d_coord anchor, std::string name = "", commonSpeed speed = STANDARD); //id is covered by NPC crew class
    NPC(d_coord anchor, std::string name = "", commonSpeed speed = STANDARD, d_coord(*f)(d_coord pos, bool XorY) = nullptr) 
    : m_anchor(anchor), m_name(name),validationFunctionPtr(f) 
    {
        m_nextPos = {float(anchor.x),float(anchor.y)}, //returns valid f_coord?
        m_speedFactor = walkingSpeed[speed];
    }
    
    void chooseDestination(u32 &seed);
    float angleLogic(float angle);
    float computeAngle(d_coord distance);

    // bool circleDistanceCheck(d_coord pos, d_coord circleCentre, double radius);
    // double snapToCircleEdge(d_coord pos,d_coord circleCentre, double r, int modeSelection);
    f_coord validatePosition(f_coord inputPos, bool XorY);

    d_coord computeInterval();
    void applyStep(int factor);
    double pythagorasDistance (d_coord distance);
    bool incrementPosition(int factor);
    bool evaluateStop(double pre, double post);
    
    float waitTimerCalculation(u32 &seed);
    void decrementWaitTimer();

    void printNPCData(int currentCycle);

    void beginCycle(u32 &seed);
    void finishCycle(u32 &seed);

    //putting it all together
    std::string npcAction_Self(u32 &seed);
    

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
    void setSpeed(float input){m_speedFactor = input;}

    int getState(){return m_state;}
    float getAngle(){return m_angle;}
    float getSpeedFactor(){return m_speedFactor;}
    std::string getName(){return m_name;}
    d_coord getAnchor(){return m_anchor;}
    d_coord getCombinedDistance(){return m_CombinedDistances;} //Not a true coordinate, since its distance, its more like a tuple and should eventually be reclassed as such.
    d_coord getDistance(){ //Same, tuple, this is a bit reckless tbh
        return {getIntendedPos().x - getNextPos().x, 
                getIntendedPos().y - getNextPos().y};
    }
    d_coord getInterval(){return m_intervalValues;} //same, tuple
    f_coord getIntendedPos(){return m_intendedPos;}
    f_coord getNextPos(){return m_nextPos;}
    duration getWaitTime(){return m_waitTime;}
    duration getWalkTime(){return m_walkTime;}
};


#endif /*NPC_H*/