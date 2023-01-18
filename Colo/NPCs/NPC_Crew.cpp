#include "NPC_Crew.h"

NPC_Crew::NPC_Crew(std::vector<NPC> in) : 
    set(in)
{
}
NPC_Crew::NPC_Crew(std::vector<NPC>in, u32 &inSeed) : 
    set(in),
    userInputSeed(inSeed)
{
    advance(inSeed); //special case knocked out of the way.
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

bool NPC_Crew::isEmpty()
{
    return false;
}

unsigned int NPC_Crew::size()
{
    return set.size();
}

void NPC_Crew::advance(u32 &seed)
{   
    for(NPC npc : set){
        status += npc.npcAction_Self(seed); //2nd arg is here for testing purposes. 
    } 
}   
    //Once done, remove 2nd arg. 
    //If is first frame and seed wasn't specified, then all npcs in the set will 
    //still have the "First" state, will use the current seed, and will run the special
    //case accordingly. Only unanswered question is if the special case actually "exhausts"
    //a frame's action, which it ought to, similar to the Begin state, which also increments 2x
    //on the first frame, just at regular intervalFactors.

std::string NPC_Crew::getStatus(){
    return status;
}

void NPC_Crew::reset()
{
    //TODO
    //How to handle copy and assignment stuff with NPC objects.
    //Should we preserve or lose the position data?
    //What are the implications?
}
