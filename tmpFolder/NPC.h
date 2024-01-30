#ifndef NPC_H
#define NPC_H

#include "processCoreLocal.h"

//TODO: Set up proposeStep, to be less presumptuous than directly immediately applying the new step after adding the interval.
//Also change nextPos to CurrentPos. Maybe nextpos calls proposeStep like a peek() function.
//rework getDistance,combinedDistance, and pythagoras distance to use proper data types you nincompoop.

//Don't you dare try to get rid of f_coord. There's sensitive double->float rounding that loses data and that matters for calculation. Don't mess with it.

enum state {FIRST,FIRST_WALK,SECOND_WALK,WALK,WAIT,FINISH,BEGIN}; // First 3 states may not apply to every room?
enum commonSpeed{STANDARD,SLOWER}; //How many speeds exist? -- can't enum a float... so its used in a map.
enum commonBallSize{SMALL = 1, MEDIUM = 3, LARGE = 4}; //More probably exist.
//maybe there's a better way to name enums...

//figure out circular dependency
//Assignment operators? Copy constructors?
class d_coord {
    public:
    double x,z,y; //How to add a to f_coord function?
    d_coord(){
        x = 0;
        z = 0;
        y = 0;
    }
    d_coord(double a,double b, double c): x(a), z(b), y(c){}
};
class f_coord {
    public:
    float x,z,y;
    f_coord(){
        x = 0;
        z = 0;
        y = 0;
    }
    f_coord(float a, float b, float c): x(a), z(b), y(c){}
    d_coord toDCoord(){
        return d_coord(double(x),double(z),double(y));
    }
    //blame powerpc data type rounding.
};
// Template function? Auto? For independent conversion function?



//VECTOR MATH FUNCTIONS. ONLY NECESSARY FOR COLLISION CALCS.



//all of these "vector" operations are just functions handling coordinates. These are for vectors in the math sense, not std::vectors lol.
d_coord vectorSub(d_coord minuend, d_coord subtrahend){
    return {minuend.x - subtrahend.x,minuend.y-subtrahend.y}; //named because the order of the parameters matters here.
} //also make f_coord version?

d_coord vectorAdd(d_coord a, d_coord b){
    return {a.x+b.x,a.z,a.y+b.y}; //Not an idiot, in the code this function does not consider z!
    //Takes a.z by default, not sure if this is the correct behaviour.
}

d_coord vectorSquare(d_coord a){ // my creation NEED Z AAAAAAAAAAAAH
    return {a.x*a.x,a.z*a.z,a.y*a.y}; 
    //alt: return {pow(a.x,2),pow(a.z,2),pow(a.y,2)};
}
double vectorCombine(d_coord a){ //my creation SHOULD INCL Z AND NON INCL Z VERSIONS
    return {a.x+a.z+a.y};
}
double vectorCombine_No_Z(d_coord a){
    return {a.x + a.y};
}

double vectorDistance(d_coord a, d_coord b){ //same as pythag distance but this makes no assumptions about the input data -_- DOES NOT CONSIDER Z
    //return sqrt(vectorCombine(vectorSquare(vectorSub(a,b)))); CONSIDER Z?????????? -- 
    return sqrt(pow((a.x-b.x),2) + pow((a.y-b.y),2)); //Will never be a negative result.
    //alt: return sqrt(vectorSquareDistance(a,b));
}

double vectorSquareDistance(d_coord a, d_coord b){ //DOES NOT CONSIDER Z
         return pow((a.x-b.x),2) + pow(a.y-b.y,2);
}

double vectorMagnitude(d_coord in){ //CONSIDERS Z
    return sqrt((in.x*in.x) + (in.z*in.z) + (in.y*in.y)); //sum of squared elements, followed by a sqrt.
    //return sqrt(pow(in.x,2) + pow(in.z,2) + pow(in.y,2));
    //alt: return sqrt(vectorCombine(vectorSquare(in)));
}

double vectorSquareMag(d_coord in){ //CONSIDERS Z
    return (in.x*in.x) + (in.z * in.z) + (in.y*in.y);
//  return (vectorCombine(vectorSquare(in)));

}

d_coord vectorScale(double factor, d_coord a){ //Does not consider Z.
    return {a.x*factor,a.z,a.y*factor}; //Note not the same as vectorMultiply as that would imply I take two vecs and multiply them together.
}















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
    //maybe organize this by stuff that gets input by the constructor.
    game m_game = COLO;
    d_coord m_anchor;
    d_coord m_intervalValues; //More like a tuple
    f_coord m_nextPos;
    f_coord m_intendedPos;
    
    commonBallSize m_collisionBallSize = MEDIUM; //Default value.

    float m_angle = 0;
    float m_speedFactor = 0;
    duration m_waitTime = duration{0};
    duration m_walkTime = duration{0};
    d_coord m_CombinedDistances; //tuple
    int m_state = FIRST;
    std::string m_name = "";
    std::map <commonSpeed,float> walkingSpeed = { //This map association could be a global but its not an enum 
        {STANDARD,0.29032257199287415},
        {SLOWER,0.28125}
    }; //add as more speeds are found.

    public:

    //NPC(d_coord anchor, std::string name = "", commonSpeed speed = STANDARD); //id is covered by NPC crew class
    NPC(d_coord anchor, std::string name = "", commonSpeed speed = STANDARD, commonBallSize ballSize = MEDIUM)
    : m_anchor(anchor), m_name(name),m_collisionBallSize(ballSize) 
    {
        m_nextPos = {float(anchor.x),0,float(anchor.y)}, //returns valid f_coord?
        m_speedFactor = walkingSpeed[speed];
    }
    
    void chooseDestination(u32 &seed);
    float angleLogic(float angle);
    float computeAngle(d_coord distance);

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
    void setGame(game g){m_game = g;}
    void setCollisionBallSize(commonBallSize b){m_collisionBallSize = b;}

    int getState(){return m_state;}
    float getAngle(){return m_angle;}
    float getSpeedFactor(){return m_speedFactor;}
    std::string getName(){return m_name;}
    d_coord getAnchor(){return m_anchor;}
    d_coord getCombinedDistance(){return m_CombinedDistances;} //Not a true coordinate, since its distance, its more like a tuple and should eventually be reclassed as such.
    d_coord getDistance(){ //Same, tuple, this is a bit reckless tbh
        return {getIntendedPos().x - getNextPos().x,
                getIntendedPos().z - getNextPos().z, 
                getIntendedPos().y - getNextPos().y};
    }
    d_coord getInterval(){return m_intervalValues;} //same, tuple
    f_coord getIntendedPos(){return m_intendedPos;}
    f_coord getNextPos(){return m_nextPos;}
    duration getWaitTime(){return m_waitTime;}
    duration getWalkTime(){return m_walkTime;}
    game getGame(){return m_game;}
    int getCollisionBallSize(){return m_collisionBallSize;}
};


#endif /*NPC_H*/