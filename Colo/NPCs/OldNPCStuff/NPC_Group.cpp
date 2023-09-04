#include "NPC_Group.h"

NPC_Group::NPC_Group(std::vector<NPC> in, u32 &inSeed) : set(in)
{
    for(NPC& npc : set){
        status += npc.npcAction_Self(inSeed);
    }
};
