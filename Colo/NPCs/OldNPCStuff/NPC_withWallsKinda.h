#ifndef NPC_H
#define NPC_H

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

    bool m_hasCorner = false;
    bool m_alteredDestination = false;
    f_coord m_alteredIntended = {0,0};


    public:
    //constructor
    //Probably don't need all these constructors,
    //the people using this code will have all the important details anyway.
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
    NPC(d_coord anchor, int speedIdx, std::string name, int ID){
        setAnchor(anchor);
        f_coord temp;
        temp.x = anchor.x;
        temp.y = anchor.y;
        setNextPos(temp);
        setName(name);
        setID(ID);
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
    if (f_working > loosePiApprox || f_working <-loosePiApprox){
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
    //std::cout << "ANGLE: " << getAngle() << ". AT SEED: " << std::hex << seed << std::dec << "\n";
    setOriginalIntended(destinationPos); //for finishing the cycle
    setIntended(destinationPos); //for walking calculations

    if (m_hasCorner){
        validatePosition(m_alteredIntended);
        setIntended(m_alteredIntended);
    }
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
        if (getAngle() > loosePiApprox || getAngle() < -loosePiApprox || m_hasCorner){
            setAngle(intervalAngle);
        }
        setAngle(intervalAngle*2);
        float sinResult = static_cast<float>(sin(intervalAngle)) * 0.9999999403953552;
        float cosResult = static_cast<float>(cos(intervalAngle));
        const float speedFactor = getSpeedFactor(); //this is different for reporter for some reason??
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
        double sumSq = pow(distance.x,2) + pow(distance.y,2); 
        double fractional = 1/sqrt(sumSq);
        double fractionalSq = pow(fractional,2);
        return -(fractionalSq*sumSq-3) * (fractional/2) * sumSq;
    }
    bool incrementPosition(int factor){
        double preStep = combineDistance(getDistance());
        applyStep(factor);
        double postStep = combineDistance(getDistance());
        if (m_hasCorner){
            //std::cout << "PRE:" << preStep << ". POST: " << postStep << std::endl;
        }
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
    void validatePosition (f_coord &inputPos){

        //On destination version: (this seems to work)
        float slope = 0.102;
        float availableX = inputPos.x;
        float availableY = slope*availableX - 12.959385871887207; //y=mx+b baby;
        float coordsAngle = computeAngle()*2;
        if (inputPos.y <= availableY && inputPos.x >= 10 && coordsAngle > 1.6 && coordsAngle < 3.1415){
            std::cout << "ALTERED DESTINATION!!!!!!!!!!!!!!!\n";
            setAltered(true);
            inputPos.y = availableY;
            inputPos.x = 9.859516143798828;
        }
    //This is used when an npc is in a tight corner and ends up cancelling their cycle early.
    //A single edge on it's own doesn't cancel a movement, because "as long as distance is being reduced, continue"
    //however when a 2nd edge (creating a corner/nook) interferes, this halts all walk progress so the cycle ends.
    
    //check each edge:
    //To obtain boundaries for the door, which is a half circle, I'd need lots of points to work with, and may need a complex
    //arc drawing tool for c++ to generate the points I need. FRrom there i'd need to check whatever point jim is closest to and
    //determine how far he can move. 
    //I'd be incrementing X, evaluating how much Y distance this frees, and doing that, then continuing until I reach the other edge.
    //From there the distances available get smaller and smaller until eventually the decrementing distance logic pops and we exit the cycle.
    //Hopefully Jim doesn't get stuck on the right-hand side of the stairs, cuz there's nothing we can do about that.
    //For now, open the damn door.
    //Some kind of y = mx+b stuff?
    // for (unsigned int i = 0; i < edges.size(); i++)
    // {
    //     float availableX = edges[i].x - getNextPos().x; //inclusive?
    //     float availableY = edges[i].y - getNextPos().y;
    
    //     if (availableX == 0 || availableY == 0){

    //     } //How to handle if both are 0? Surely we just minimize until past target right?
    
    // }
        //OnPosition version:
        //find available x or y:
        //custom, need a better way to store and modify this, maybe a child corner class.
        
        // float availableX = currentStep.x;
        // float availableY = slope*availableX - 12.58; //y=mx+b baby;
        // if (availableX >= 9.9 && currentStep.y < availableY){
        //     availableX = 9.9;//??Basically need to send signal to stop moving.
        // }
        // if (currentStep.y < availableY && (availableX >= 0 || availableX <=9.9 )){
        //     currentStep.y = availableY; //snap to the wall. is this the right appraoch?
        // } 
    }
    float waitTimerCalculation(u32 &seed){
        const int factorTime = 3;//these *might* vary by npc
        const int baseTimeS = 5; 
        //std::cout << "SEED INTO WAIT: " <<std::hex<< seed<<std::dec << std::endl;
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
        //std::cout << getName() <<  " - WAIT TIME: " << m_waitTime.getSeconds() << "\n";
        //setNextPos(getOriginalIntendedPos()); //account for overshoot
        setNextPos(getOriginalIntendedPos());
        if (m_hasCorner && getAltered()){
            setNextPos({9.859516143798828,-12.959385871887207});
            setAltered(false);
        }
    }

    //putting it all together
    void initializeNPC_Self(u32 &seed){
        beginCycle(seed);
        npcAction_Self(seed,0); //first two steps happen on first frame
        npcAction_Self(seed,1);
    }
    std::string npcAction_Self(u32 &seed, int i){
        std::string action = ""; //optional
        int factor = 2;
        if (i == 0){
            factor = 1;
        }
        else if (i == 1){
            factor = 5;
        }

        switch (getState())
            { 
            case WALK:
                action = "**";
                incrementPosition(factor);
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
                incrementPosition(factor); //standard practice?
                incrementPosition(factor);
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
    void setOriginalIntended(f_coord input){m_intendedPos = input;}
    void setIntended(f_coord input){m_alteredIntended = input;}
    void setIntervals(d_coord input){m_intervalValues = input;}
    void setNextPos(f_coord input){m_nextPos = input;}
    void setState (int input){m_state = input;}
    void setWaitTime(duration input){m_waitTime = input;}
    void setWalkTime(duration input){m_walkTime = input;}
    void setName(std::string input){m_name = input;}
    void setID(int input){m_ID = input;}
    void setSpeed(float input){m_speedFactor = input;}
    void setHasCorner(bool input){m_hasCorner = input;}
    void setAltered (bool input){m_alteredDestination = input;}
    
    
    int getID(){return m_ID;}
    int getState(){return m_state;}
    bool getHasCorner(){return m_hasCorner;}
    bool getAltered(){return m_alteredDestination;}
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
    f_coord getIntendedPos(){return m_alteredIntended;}
    f_coord getOriginalIntendedPos(){return m_intendedPos;}
    f_coord getNextPos(){return m_nextPos;}
    duration getWaitTime(){return m_waitTime;}
    duration getWalkTime(){return m_walkTime;}



};



//ACCESSORY FUNCTIONS, FIND THESE A BETTER HOME.
void NPCAdvanceCycle (u32 &seed,NPC &npc, int currentCycle){
    int framesWalked = 0;
    npc.beginCycle(seed);
    //First step:
    npc.incrementPosition(1);
    //remaining steps
    bool distance_decreasing = true;
    do
    {
        distance_decreasing = npc.incrementPosition(2);
        framesWalked++;
    } while (distance_decreasing);
    //adjust for overshoot
    npc.finishCycle(seed);
    npc.printNPCData(currentCycle);
}
std::string npcAction_Condensed(u32 &seed,NPC &npc, int i){
    std::string action = ""; //optional
    int factor = 2;
    if (i == 0){
        factor = 1;
    }
    else if (i == 1){
        factor = 5;
    }

    switch (npc.getState())
        { 
        case WALK:
            action = "**";
            npc.incrementPosition(factor);
            break;
        case WAIT:
            action = "--";
            npc.decrementWaitTimer();
            break;
        case FINISH:
            action = npc.getName() + "f";
            npc.finishCycle(seed);
            break;
        case BEGIN:
            action = npc.getName() + "b";
            npc.beginCycle(seed);
            npc.incrementPosition(factor); //standard practice?
            npc.incrementPosition(factor);
            break;
        default:
            std::cout << "STATE INVALID!";
            break;
        }
        return action;
}
void initializeNPCSet_h(u32 &seed,std::vector<NPC>&npcSet, int i){
for (unsigned int i = 0; i < npcSet.size(); i++)
  {
    npcSet[i].beginCycle(seed);
    npcAction_Condensed(seed,npcSet[i],0); //first two steps happen on first frame
    npcAction_Condensed(seed,npcSet[i],1);
  }
}







#endif /*NPC_H*/