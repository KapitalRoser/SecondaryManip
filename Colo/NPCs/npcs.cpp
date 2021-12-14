#include "../../processCore.h"
#include "../../NPC.h"
#include "../coloCore.h"

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
void initializeNPCSet(u32 &seed,std::vector<NPC>&npcSet, int i,std::string &action,std::ofstream &outF){
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

int main(){
    std::ofstream outF("npcSim.txt");

    //~~~~~~~~~CONFIG~~~~~~~~~~~~
    u32 inputSeed = 0x9B965ED0; //first seed hit from blink :D
    int frameWindow = 600;
    int callsPerPlayerStep = 2;
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
    

    for (int i = 0; i < frameWindow; i++)
    {   
        if (i == 0){
            initializeNPCSet(seed,npcSet,i,action,outF);
        }
        colo_RollBackground(seed,i,version);
        bool step = col_CheckStepPath(quilavaSteps,seed,i,callsPerPlayerStep);
        //NPCs:
        std::string action = "";
        outF << std::setw(3) << i << ": ";
        for(unsigned int j = 0; j < npcSet.size(); j++){
            action += npcAction(seed,npcSet.at(j),i);
        }
        //Output
        outF << action;
        outF << std::hex << " : " << seed << " : " << std::dec;
        if (step){
            outF << " ++ STEP!";
        }
        outF << std::endl;
    }


    return 0;
}