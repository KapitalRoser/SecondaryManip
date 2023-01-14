#ifndef NPC_H
#define NPC_H
/*
THIS IS A LOCAL COPY OF THE HEADER.
*/
#include "../../processCore.h"
#include "duration.h"
enum state {WALK,WAIT,BEGIN,FINISH,FIRST};
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
    int m_state = FIRST;
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

    float angleLogic(float angle);
    float computeAngle(d_coord distance);

    d_coord computeInterval();
    void applyStep(int factor);
    double pythagorasDistance (d_coord distance);
    bool incrementPosition(int factor);
    bool shouldStop(double pre, double post);
    
    float waitTimerCalculation(u32 &seed);
    void decrementWaitTimer();

    void printNPCData(int currentCycle);

    void beginCycle(u32 &seed);
    void finishCycle(u32 &seed);

    //putting it all together
    //These are what is currently called by QuilavaDWithNPCs.cpp
    void initializeNPC_Self(u32 &seed);
    std::string npcAction_Self(u32 &seed, int frameNum);
    

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