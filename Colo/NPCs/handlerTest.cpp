#include "../../processCore.h"
#include "NPC_Group.h"
#include "NPC_Crew.h"

int main(){

    NPC kaib    = NPC({  85, -150},"K");
    NPC jim     = NPC({  15,  -10},"J"); // -- fuck you jim, costs 5s to open door for him.
    NPC grandma = NPC({-140,  -10},"G");
    NPC boots   = NPC({  90,   90},"B");
    NPC randall = NPC({ -90,  110},"R"); // -- fuck you randall, stop doing weird things on the curve.
    NPC heels   = NPC({  30,  300},"H",SLOWER); // -- fuck you heels, for having a slightly slower speed.

    u32 seed = 0x7B016A28;
    u32 seed2 = seed;
    u32 seed3 = seed;
    NPC_Group npcSet = NPC_Group({kaib,jim},seed);
    std::vector<NPC> setOfNPCs = {kaib,jim};
    NPC_Crew npcCrew = NPC_Crew({grandma,boots},seed3);

    for (NPC &npc : setOfNPCs) //This form of loop with the &npc is fine.
    {
        npc.npcAction_Self(seed2);
    }

    std::cout << std::hex << "seed: " << seed << " : " << seed2 << " : " << seed3;

    return 0;
}