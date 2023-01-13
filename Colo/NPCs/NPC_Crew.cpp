#include "NPC_Crew.h"

NPC_Crew::NPC_Crew(std::vector<NPC> in) : 
    set(in), 
    seedWasSpecified(false),
    firstFrame(true)
{
}
NPC_Crew::NPC_Crew(std::vector<NPC>in, u32 inSeed) : 
    set(in), 
    userInputSeed(inSeed),
    seedWasSpecified(true),
    firstFrame(true)
{
}
void NPC_Crew::push(NPC in)
{
    set.push_back(in);
}

bool NPC_Crew::pop()
{
    if (isEmpty()){
        return false;
    }
    set.pop_back();
    return true;
}

void NPC_Crew::advance(u32 &seed)
{   
    if (!seedWasSpecified){
        userInputSeed = seed;
    }
    if (firstFrame){
        if (!seedWasSpecified){
            userInputSeed = seed;
        }
        for(NPC npc : set){
            npc.initializeNPC_Self(userInputSeed);
        }
    } else {
        for(NPC npc : set){
            npc.npcAction_Self(seed,10); //2nd arg is here for testing purposes. 
            //Once done, remove and redo the two methods.
        }
    } 
}
