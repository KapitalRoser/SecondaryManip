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
    npc.finishCycle(seed,framesWalked);
}

int main(){
    //remember to try locking rng seed to 0 when doing phenac tests.
    u32 seed = 0x20ff71DA;
    int cycleCount = 3;
    //Define all NPCs.
    NPC redGirl = NPC({4,24});
    for (int i = 0; i < cycleCount; i++)
    {
        advanceCycle(seed,redGirl,i);
        redGirl.printNPCData(i);   
    }

    return 0;
}