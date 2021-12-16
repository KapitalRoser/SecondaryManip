#include "../../processCore.h"
#include "NPC.h" //LOCAL COPY OF HEADER
#include "../coloCore.h"


/*
TODO: Solve issue where Jim only burns 1 incrementStep() during beginCycle() instead of two.
Solve issue where Jim gets completely stuck on phenac gym door.
    This may require implementing some simulation of collision. 
*/


void advanceCycle (u32 &seed,NPC &npc, int currentCycle){
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
//THIS IS THE FUNCTION USED IN THIS FILE. NOT THE ONE IN NPC.H - THAT FUNCTION IS FOR QUILAVADWITHNPCS.CPP
std::string npcAction(u32 &seed,NPC &npc, int i){
    std::string action = "";
    int factor = 2;
    switch (npc.getState())
        {
        case WALK:
            action = "**";
            if (i == 0){ //This initial rate is dependent on the specific map, and entrance.
               factor = 1;
            }
            else if (i == 1){
                factor = 5;
            }
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
void initializeNPCSet(u32 &seed,std::vector<NPC>&npcSet,std::string &action,std::ofstream &outF){
    for (unsigned int i = 0; i < npcSet.size(); i++) //this version returns action.
    {
        npcSet[i].beginCycle(seed);
        outF << npcSet[i].getName() << " began cycle!";
        action += npcAction(seed,npcSet[i],0); //first two steps happen on first frame
        action += npcAction(seed,npcSet[i],1);
        // action += npcSet[i].initializeNPC_Self(seed); //This is two line version, doesn't use action.
        // outF << npcSet[i].getName() << " began cycle!";
    }
}

void outputToFile(u32 seed, std::string action, std::ofstream &outF,std::ofstream &rawF, bool step){
    outF << action;
    outF << std::hex << " : " << seed << " : " << std::dec;
    if (step){
        outF << " ++ STEP!";
    }
    outF << std::endl;
    rawF << std::hex << seed << std::endl;
}

int main(){
    std::ofstream outF("npcSim.txt");
    std::ofstream outFRaw("npcSimRaw.txt");

    //~~~~~~~~~CONFIG~~~~~~~~~~~~
    u32 inputSeed = 0x59B23146; //first seed hit from blink :D
    int frameWindow = 1372;
    int callsPerPlayerStep = 1;
    region version = NTSCU;
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~
    u32 seed = inputSeed;
    std::string action = "";

    //PHENAC NPCS LOAD IN THIS ORDER:
    NPC kaib    = NPC({85,-150}, STANDARD,"K");
    NPC jim     = NPC({15,-10},  STANDARD,"J");
    NPC grandma = NPC({-140,-10},STANDARD,"G");
    NPC boots   = NPC({90,90},   STANDARD,"B");
    NPC randall = NPC({-90,110}, STANDARD,"R"); 
    NPC heels   = NPC({30,300},  SLOWER,  "H");
    std::vector<NPC>npcSet = {kaib,jim,grandma,boots,randall,heels};
    std::vector<int>quilavaSteps{5,10,15,20,25,30,35,39,44,49,54,67,88,100,111,119,
    126,132,138,143,148,153,158,163,168,173,177,182,187,192,197,202,210,217,
    227,237,252,268,284,292,297,302,307,312,316,321,326,331};
    std::vector<int>croconawSteps{};
    std::vector<int>bayleefSteps{};



    initializeNPCSet(seed,npcSet,action,outF);
    outputToFile(seed,action,outF,outFRaw,0);
    for (int i = 0; i < frameWindow; i++)
    {   //standard items:
        colo_RollBackground(seed,i,version);
        bool step = col_CheckStepPath(quilavaSteps,seed,i,callsPerPlayerStep);
        //NPCs:
        std::string action = "";
        outF << std::setw(3) << i << ": ";
        for(unsigned int j = 0; j < npcSet.size(); j++){
            action += npcAction(seed,npcSet.at(j),i);
        }
        //Output
        outputToFile(seed,action,outF,outFRaw,step);
        if (i == 199){
            std::cout << std::setprecision(17)<< npcSet[1].getIntendedPos().x << " : " << npcSet[1].getIntendedPos().y << std::endl;
        }
        if (i >= 198 && i < 205){
            std::cout << "x pos: " << std::setw(18) << npcSet[1].getNextPos().x << " y pos: "<< npcSet[1].getNextPos().y << std::endl;
        }
    }

    return 0;
}