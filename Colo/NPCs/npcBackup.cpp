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
            // std::cout << "WAIT SEED: " << std::hex << seed << std::dec << "\n";
            // if (npc.getState() == FINISH){
            //     action = npc.getName() + " fin!";
            //     npc.printNPCData(i);
            //     npc.finishCycle(seed);
            //  }
            break;
        case WAIT:
            action = "--";
            npc.decrementWaitTimer();
            break;
        case FINISH:
            action = npc.getName() + "f";
            npc.finishCycle(seed);
           // npc.printNPCData(i);
            //is this correct or a bandaid?
            //npc.decrementWaitTimer();
            break;
        case BEGIN:
            //std::cout << "BEGIN SEED: " << std::hex << seed << std::dec << std::endl;
            action = npc.getName() + "b";
            npc.beginCycle(seed);
            // npc.printNPCData(i);
            //std::cout << npc.getName() << std::endl;
            //if (npc.getName() != "R"){
                npc.incrementPosition(factor); //standard practice?
                npc.incrementPosition(factor);
            //}
            
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


void initializeNPCSet(u32 &seed,std::vector<NPC>&npcSet, int i,std::string &action,std::ofstream &outF){
    for (unsigned int i = 0; i < npcSet.size(); i++)
            {
                npcSet[i].beginCycle(seed);
                outF << npcSet[i].getName() << " began cycle!";
                action += npcAction(seed,npcSet[i],0); //first two steps happen on first frame
                action += npcAction(seed,npcSet[i],1);
            }
}


int main(){
    //remember to try locking rng seed to 0 when doing phenac tests.

    //WHAT IS ORDER OF OPERATIONS: 
    //PlayerStep, Background, NPC call?
    //Background happens before PlayerStep
    //Multiple on the same frame is rare but possible and happens.
    std::ofstream outF("npcSim.txt");

    // u32 seedFromBlink = 0x75CC30;
    // LCGn(seedFromBlink,4);
    // std::cout << std::hex << seedFromBlink << std::endl;
    // u32 inputSeed = seedFromBlink; //9B965ED0

    u32 inputSeed = 0x9B965ED0; //first seed hit from blink :D
    int frameWindow = 600;
    int callsPerPlayerStep = 2;
    region version = NTSCU;
    

    u32 seed = inputSeed;
    //Define all NPCs.
    //int cycleCount = 3;
    NPC redGirl = NPC({4,24},STANDARD,"RedGirl"); //Yes NPCs have Z value but not used for wandering, only xy matter.

    //PHENAC NPCS LOAD IN THIS ORDER:
    NPC kaib    = NPC({85,-150}, STANDARD,"K");
    NPC jim     = NPC({15,-10},  STANDARD,"J"); // -- fuck you jim, costs 5s to open door for him. Can get stuck permanently (:
    NPC grandma = NPC({-140,-10},STANDARD,"G");
    NPC boots   = NPC({90,90},   STANDARD,"B");
    NPC randall = NPC({-90,110}, STANDARD,"R"); // -- fuck you randall, getting wedged on the bridge & river.
    NPC heels   = NPC({30,300},  SLOWER,  "H");
    //Castform gets initialized before Kaib lol, and his trainer gets initialized before Lady, no impact on RNG tho.
    
    //This corner guarding is really janky and custom, needs a better fix. Also would need to simulate perma-stuck problem

    //jim.setHasCorner(true);
    //randall.setHasCorner(true); rare AF, right?


    std::vector<NPC>npcSet = {kaib,jim,grandma,boots,randall,heels};


    // kaib.beginCycle(seed);
    // kaib.incrementPosition(1);
    // //remaining steps
    // bool distance_decreasing = true;
    // do
    // {
    //     distance_decreasing = kaib.incrementPosition(2);
    // } while (distance_decreasing);
    // //adjust for overshoot
    // kaib.finishCycle(seed);
    // kaib.printNPCData(0);

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
            initializeNPCSet(seed,npcSet,i,action,outF);
            //npcSet.erase(npcSet.end()-1,npcSet.end());
        }
        // Npcs resolve in order, and background happens first, then player steps, then NPCs last.

        //background
        colo_RollBackground(seed,i,version);
        bool step = col_CheckStepPath(quilavaSteps,seed,i,callsPerPlayerStep);

        //NPCs:
        std::string action = "";
        outF << std::setw(3) << i << ": ";

        for(unsigned int j = 0; j < npcSet.size(); j++){
           // std::cout << "\n";
            action += npcAction(seed,npcSet.at(j),i);
        }
        


        // if (i >= 377 && i < 428){
        //     std::cout << std::dec << "\nFRAME: " << i << std::endl;
        //     std::cout << "Name: " << npcSet[4].getName() << std::endl;
        //     //std::cout << npcSet[4].getWalkTime().getFrames30() << std::endl;
        //     //std::cout << std::setprecision(17) << "X: " << npcSet[4].getDistance().x << " : " << npcSet[4].getInterval().x
        //     //<< "\nY: " << npcSet[4].getDistance().y << " : " << npcSet[4].getInterval().y <<  "\n";
        //     std::cout << npcSet[4].getIntendedPos().x << " : " <<npcSet[4].getIntendedPos().y << "\n";
        //     //std::cout << "Action: " << action << "\n";
        // }

        //Output
        outF << action;
        outF << std::hex << " : " << seed << " : " << std::dec;
        if (step){
            outF << " ++ STEP!";
        }
        // if (npcSet[0].getState() == WAIT){
        //     outF << std::setprecision(9) << npcSet[0].getWaitTime().getSeconds();
        // }
        // outF << npcSet[0].getName();
        outF << std::endl;
    }
    // LCG(inputSeed);
    // advanceCycle(inputSeed,jim,0);
    // inputSeed = 0xF622BA5C;
    // //LCG(inputSeed);
    // jim.InitialXY(inputSeed);
    // jim.computeInterval();
    //std::cout << "JIM: " << jim.getAngle();

    //advanceCycle(inputSeed,kaib,0);
    //advanceCycle(inputSeed,redGirl,0);
    // LCGn(inputSeed,5);
    // advanceCycle(inputSeed,heels,0);
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