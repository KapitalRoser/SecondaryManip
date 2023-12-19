#include "../../processCore.h"
#include "../../NPC.h"

enum bgFrames {H = 116, L = 76, A = 114, O = 154};



int col_consultPattern(int i, region gameRegion){
    std::vector<int>NTSCUPattern = {H,H,L,H,L}; //HHLHL
    std::vector<int>PAL60Pattern = {
    A,H,
    H,L,H, O,L,
    H,H,L, O,H,
    L,H,L, O,H,
    H,L,H, O,L,
    H,L,H, O,H,
    L,H,L, O,H,
    L,H,H, O,L,
    H,L,H, O,L,
    H,H,L, O,H,
    L,H,H, A,H //Last A,H necessary?
    };
    std::vector<int>::iterator iter;
    int offset = 3; //const?
    int range = 0;
    switch (gameRegion)
    {
    case NTSCU:
        iter = NTSCUPattern.begin();
        range = NTSCUPattern.size();
        //Does this logic work for all patterns now?
        if (i < offset){
            iter += (i % offset);
        } else {
            iter += (i-offset) % range;
        }
        break;
    case PAL60:
        iter = PAL60Pattern.begin();
        range = PAL60Pattern.size();
        //logic
        break;
    case PAL50:
        //pattern
        //logic
        break;
    // case NTSCJ:
    //     //pattern
    //     //logic
    //     break;
    default:
        break;
    }
    return *iter;
}

bool col_CheckStepPath(std::vector<int>secondarySteps,u32& seed,int i,int stepCalls){
  if (binary_search(secondarySteps.begin(),secondarySteps.end(),i+1)){
        LCGn(seed,stepCalls);
        return true;
  } else {
        return false;
  }
}
int colo_RollBackground(u32 &seed,int i, region gameRegion){    
      return LCGn(seed,col_consultPattern(i,gameRegion));
}

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
    std::ofstream rawF("npcSimRaw.txt");

    //~~~~~~~~~CONFIG~~~~~~~~~~~~
    u32 inputSeed = 0x9B965ED0; //first seed hit from blink :D
    int frameWindow = 600;
    int callsPerPlayerStep = 2;
    region version = NTSCU;
    bool trackSteps = false;
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

    for (int i = 0; i < frameWindow; i++)
    {   
        if (i == 0){
            initializeNPCSet(seed,npcSet,i,action,outF);
        }
        colo_RollBackground(seed,i,version);
        bool step = false;
        if(trackSteps){
            step = col_CheckStepPath(quilavaSteps,seed,i,callsPerPlayerStep);
        }
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
        rawF << std::uppercase << std::hex << seed << std::dec << std::endl;
    }


    return 0;
}