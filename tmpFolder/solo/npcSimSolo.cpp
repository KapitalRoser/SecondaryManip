// #include "NPC.h"
// #include "processCoreLocal.h"


// std::string npcAction(u32 &seed,NPC &npc, int i){
//     std::string action = ""; //optional -- exists only for debugging.
//     int intervalFactor = 2;
//     switch (getState())
//         {
//         case WALK:
//             action = "**";
//             npc.incrementPosition(intervalFactor);
//             break;
//         case WAIT:
//             action = "--";
//             npc.decrementWaitTimer();
//             break;
//         case FINISH:
//             action = npc.getName() + "f";
//             npc.finishCycle(seed);
//             break;
//         case BEGIN:
//             action = npc.getName() + "b";
//             npc.beginCycle(seed);
//             npc.incrementPosition(intervalFactor); //standard practice? -- WHEN WHERE AND HOW?
//             npc.incrementPosition(intervalFactor);
//             // if (!(npc.getName()[0] == 'J') || i != 199){
//             //     npc.incrementPosition(intervalFactor);
//             // }

//             break;
//         default:
//             std::cout << "STATE INVALID!";
//             break;
//         }
//         return action;
// }