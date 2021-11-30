#include "../../processCore.h"
#include "../../NPC.h"

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
    switch (npc.getState())
        { //Do we ever do multiple actions on a given frame?
        case WALK:
            action = "**";
            if (i == 0){
                npc.incrementPosition(1);
            } else {
                npc.incrementPosition(2);
            }
            break;
        case WAIT:
            action = "--";
            npc.decrementWaitTimer();
            break;
        case BEGIN:
            action = npc.getName() + " began cycle!";
            npc.beginCycle(seed);
          //  std::cout << npc.getName() << " began!";
            break;
        case FINISH:
            action = npc.getName() + " finished cycle!";
            npc.finishCycle(seed);
          //  std::cout << "finished!";
            break;
        default:
            std::cout << "STATE INVALID!";
            break;
        }
        //std::cout << "NPC: " << npc.getName();
        return action;
}

int main(){
    //remember to try locking rng seed to 0 when doing phenac tests.

    //WHAT IS ORDER OF OPERATIONS: 
    //PlayerStep, Background, NPC call?
    //Background happens before PlayerStep
    //Multiple on the same frame is rare but possible and happens.
    std::ofstream outF("npcSim.txt");

    u32 inputSeed = 0x354FCCC3;
    int frameWindow = 400;
    int callsPerPlayerStep = 1;
    std::vector<int>quilavaSteps{5,10,15,20,25,30,35,39,44,49,54,67,88,100,111,119,
    126,132,138,143,148,153,158,163,168,173,177,182,187,192,197,202,210,217,
    227,237,252,268,284,292,297,302,307,312,316,321,326,331};

    u32 seed = inputSeed;
    //Define all NPCs.
    //int cycleCount = 3;
    //NPC redGirl = NPC({4,24}); //Yes NPCs have Z value but not used for wandering, only xy matter.

    //PHENAC NPCS LOAD IN THIS ORDER:
    NPC punk = NPC({85,-150},"Punk");
    NPC gym = NPC({15,-10},"Gym");
    NPC grandma = NPC({-140,-10},"Grandma");
    NPC athlete = NPC({90,90},"Athlete");
    NPC shop = NPC({-90,110},"Shop");
    NPC reporter = NPC({30,300},"Reporter");
    std::vector<NPC>npcSet = {punk,gym,grandma,athlete,shop,reporter};    // LCGn(seed,2);
    // grandma.beginCycle(seed);
    // grandma.incrementPosition(1);
    // //remaining steps
    // bool distance_decreasing = true;
    // do
    // {
    //     distance_decreasing = grandma.incrementPosition(2);
    // } while (distance_decreasing);
    // //adjust for overshoot
    // grandma.finishCycle(seed);
    // grandma.printNPCData(0);

    

    
    for (int i = 0; i < frameWindow; i++)
    {   
        //This is where background goes.

        //if steps happen before npc:
        bool step = false;
        if (std::binary_search(quilavaSteps.begin(),quilavaSteps.end(),i)){
            LCGn(seed,callsPerPlayerStep);
            step = true;
        }

        //NPCs:
        std::string action = "";
        outF << std::setw(3) << i << ": ";
        for(unsigned int j = 0; j < npcSet.size(); j++){
            action += npcAction(seed,npcSet.at(j),i);
        }
    
        //Output
        outF << action;
        if (step){
            outF << " ++ STEP!";
        }
        outF << std::endl;
    }
   
    // std::cout << "~~~~~PUNK~~~~~~~~~~~\n";
    // npcSet[0].printNPCData(0);
    // std::cout << "~~~~~GYM~~~~~~~~~~~~\n";
    // npcSet[1].printNPCData(0);
    // std::cout << "~~~~~GRANDMA~~~~~~~~\n";
    // npcSet[2].printNPCData(0);
    // std::cout << "~~~~~ATHLETE~~~~~~~~\n";
    // npcSet[3].printNPCData(0);
    // std::cout << "~~~~~SHOP~~~~~~~~~~~\n";
    // npcSet[4].printNPCData(0);
    // std::cout << "~~~~~REPORTER~~~~~~~\n";
    // npcSet[5].printNPCData(0);
    
    

    /*
    flow:

    First input seed
    initialize all NPCS in fixed order.
    
    Need:
    A) frame window provided, so need to keep a running tally of
    how many frames are used by an npc since intialization.
    B) Any Step calls interfering, and on what frame.

    So on each frame i, advance all npcs walk or wait timers.
    If a player stepcall occurs, inject that in. Then when cycle ends,
    seed is correct for the next npc cycle, where applicable.





    */







    // for (int i = 0; i < cycleCount; i++)
    // {
    //     advanceCycle(seed,redGirl,i);
    //     redGirl.printNPCData(i);   
    // }

    return 0;
}