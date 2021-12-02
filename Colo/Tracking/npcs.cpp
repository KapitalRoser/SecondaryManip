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
    int factor = 2;
    switch (npc.getState())
        { //Do we ever do multiple actions on a given frame?
        case WALK:
            action = "**";
            if (i == 0){
               factor = 1;
            }
            else if (i == 1){
                factor = 5;
            }
             npc.incrementPosition(factor);
             if (npc.getState() == FINISH){
                action = npc.getName() + " fin!";
                npc.finishCycle(seed);
             }
            break;
        case WAIT:
            action = "--";
            npc.decrementWaitTimer();
            if (npc.getState() == BEGIN){
                action = npc.getName() + " beg!";
                npc.beginCycle(seed);
            }
            break;
        // case BEGIN:
        //     action = npc.getName() + " began cycle!";
        //     npc.beginCycle(seed);
        //   //  std::cout << npc.getName() << " began!";
        //     break;
        // case FINISH:
        //     action = npc.getName() + " finished cycle!";
        //     npc.finishCycle(seed);
        //   //  std::cout << "finished!";
        //     break;
        default:
            std::cout << "STATE INVALID!";
            break;
        }
        //std::cout << "NPC: " << npc.getName();
        return action;
}
int consultPattern(int i, region gameRegion){
      const int loFrame = 76;
      const int hiFrame = 116;

      const int xframe = 114;
      const int yframe = 154;
      
      int mainPattern[5] = {hiFrame,hiFrame,loFrame,hiFrame,loFrame}; //HHLHL
      int* selector = mainPattern + ((i - 3) % 5);
      if (i <= 2){
        selector = mainPattern + (i % 3);
      }
      return *selector;
}
int rollBackground(u32 &seed,int i, region gameRegion){    
      int fcount = consultPattern(i,gameRegion);
      LCGn(seed,fcount); //application of rules.
      return fcount;
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
    int callsPerPlayerStep = 2;
    region version = USA;
    std::vector<int>quilavaSteps{5,10,15,20,25,30,35,39,44,49,54,67,88,100,111,119,
    126,132,138,143,148,153,158,163,168,173,177,182,187,192,197,202,210,217,
    227,237,252,268,284,292,297,302,307,312,316,321,326,331};

    u32 seed = inputSeed;
    //Define all NPCs.
    //int cycleCount = 3;
    NPC redGirl = NPC({4,24},"RedGirl"); //Yes NPCs have Z value but not used for wandering, only xy matter.

    //PHENAC NPCS LOAD IN THIS ORDER:
    NPC punk = NPC({85,-150},"Punk");
    NPC gym = NPC({15,-10},"Gym");
    NPC grandma = NPC({-140,-10},"Grandma");
    NPC athlete = NPC({90,90},"Athlete");
    NPC shop = NPC({-90,110},"Shop");
    NPC reporter = NPC({30,300},"Reporter");
    std::vector<NPC>npcSet = {punk,gym,grandma,athlete,shop,reporter};    // LCGn(seed,2);
    // punk.beginCycle(seed);
    // punk.incrementPosition(1);
    // //remaining steps
    // bool distance_decreasing = true;
    // do
    // {
    //     distance_decreasing = punk.incrementPosition(2);
    // } while (distance_decreasing);
    // //adjust for overshoot
    // punk.finishCycle(seed);
    // punk.printNPCData(0);

    //NPCs:

    std::string action = "";
    
    //action += npcAction(seed,npcSet[0],0);
    // outF << std::setw(3) << 0 << ": ";
    // for(unsigned int j = 0; j < npcSet.size(); j++){
    //     action += npcAction(seed,npcSet.at(j),0);
    // }
    // //Output
    // outF << action;
    // outF << std::hex << " : " << seed << " : " << std::dec;
    // outF << std::endl;

    

    for (int i = 0; i < frameWindow; i++)
    {   
        if (i == 0){
            for (unsigned int i = 0; i < npcSet.size(); i++)
            {
                npcSet[i].beginCycle(seed);
                outF << npcSet[i].getName() << " began cycle!";
                action += npcAction(seed,npcSet[i],0); //first two steps happen on first frame
                action += npcAction(seed,npcSet[i],1);
            }
            //npcSet.erase(npcSet.begin()+1,npcSet.end());
            
        }
        //hypothesis is correct --
        // Npcs resolve in order, and background happens first, then player, then NPCs.
        
        //background
        rollBackground(seed,i,version);


        //Player's steps
        bool step = false;
        u32 outSeed = seed;
        if (std::binary_search(quilavaSteps.begin(),quilavaSteps.end(),i+2)){
            LCGn(seed,callsPerPlayerStep);
            step = true;
        }


        //NPCs:
        std::string action = "";
        outF << std::setw(3) << i << ": ";
        for(unsigned int j = 0; j < npcSet.size(); j++){
           // std::cout << "\n";
            action += npcAction(seed,npcSet.at(j),i);
        }
        // if (i < 35){
        //     std::cout << npcSet[0].getWalkTime().getFrames30() << std::endl;
        //     std::cout << std::setprecision(17) << "X: " << npcSet[0].getNextPos().x << " : " << npcSet[0].getInterval().x
        //     << "\nY: " << npcSet[0].getNextPos().y << " : " << npcSet[0].getInterval().y <<  "\n";
        //     std::cout << "Action: " << action << "\n";
        // }

        //Output
        outF << action;
        outF << std::hex << " : " << outSeed << " : " << std::dec;
        if (step){
            outF << " ++ STEP!";
        }
        //outF << npcSet[0].getName();
        outF << std::endl;
    }
   


    //advanceCycle(inputSeed,punk,0);
    //advanceCycle(inputSeed,redGirl,0);
    //9E615FEB
    //EF671641


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