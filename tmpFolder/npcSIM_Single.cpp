// //#include "../OLD_ProcessCore.h"
// #include "NPC.h"
// #include "processCoreLocal.h"
// //#include "NPC.h"
// enum bgFrames {H = 116, L = 76, A = 114, O = 154}; //High, Low, Alpha, Omega

// //begin (-b) is defined as the first frame in which the npc's cur position has changed.
// //f is defined as the first frame in which the npc's wait timer is visible.

// //THIS FILE REALLY SHOULD BE CALLED PHENAC_SIM
// //Includes a lot of stuff that is phenac specific.

// //THESE ARE EXAMPLE FUNCTIONS FOR COLLISION CHECKING. HOWEVER THE END USER WILL LIKELY NEED/WANT TO WRITE THEIR OWN.
// bool circleDistanceCheck(d_coord pos, d_coord circleCentre, double radius){
//     d_coord pDist;
//     pDist.x = pos.x - circleCentre.x;
//     pDist.y = pos.y - circleCentre.y;
//     double distToCentre = sqrt(pow(pDist.x,2) + pow(pDist.y,2));
//     return distToCentre < radius;
// }
// double snapToCircleEdge(double givenCoord,double circleGiven, double circleSolved, double r, bool circleSideChosen){
//     /*sets a given x or y to its correct point on the edge of the circle.
//     x = h +- sqrt(r^2 - (y-k)^2)
//     y = k +- sqrt(r^2 - (x-h)^2)
//     (x-h)^2 + (y-k)^2 = r^2
//     */
//    //if given y, solve for x and vice versa. applies for circle centre coords as well.
//    double solvedCoord = sqrt(pow(r,2) - pow(givenCoord - circleGiven,2));
//     //reduces the choice of x or y to whatever coord is given.
//     //still needs user to choose side arbitrarily.
//    if (circleSideChosen){ //pos
//     return circleSolved + solvedCoord;
//    }
//    return circleSolved - solvedCoord;
// }

// bool bridgeCheck(d_coord pos){
//     //raycast the bridge.
//     return 0;
// }

// d_coord randallAdjust(d_coord inputPos, bool XorY){
//     //data
//     d_coord circleCentre;
//     circleCentre.x = 0;
//     circleCentre.y = 185;
//     double radius = 100;
//     if (!circleDistanceCheck(inputPos,circleCentre,radius)){
//         return inputPos;
//     }
    
//     //now bridge check
//     if (bridgeCheck(inputPos)){
//         return inputPos; //Safe I think, sometimes weird clipping around the bridge corner.
//         //If cause problems, look here.
//     }
//     //so in circle, not in bridge == reject.
//     //for x
//     d_coord result;
//     if (XorY){ //TRUE == X
//          result.x = snapToCircleEdge(inputPos.y,circleCentre.y,circleCentre.x,radius,0); //should be left side.
//          result.y = inputPos.y;
//     } else {
//         result.x = inputPos.x;
//         result.y = snapToCircleEdge(inputPos.x,circleCentre.x,circleCentre.y,radius,1); //should be upper side
//     }
//     return result;
// }




// int col_consultPattern(int i, region gameRegion){
//     std::vector<int>NTSCUPattern = {H,H,L,H,L}; //HHLHL
//     std::vector<int>PAL60Pattern = {
//     A,H,
//     H,L,H, O,L,
//     H,H,L, O,H,
//     L,H,L, O,H,
//     H,L,H, O,L,
//     H,L,H, O,H,
//     L,H,L, O,H,
//     L,H,H, O,L,
//     H,L,H, O,L,
//     H,H,L, O,H,
//     L,H,H, A,H //Last A,H necessary?
//     };
//     std::vector<int>::iterator iter;
//     int offset = 3; //const?
//     int range = 0;
//     switch (gameRegion)
//     {
//     case NTSCU:
//         iter = NTSCUPattern.begin();
//         range = NTSCUPattern.size();
//         //Does this logic work for all patterns now?
//         if (i < offset){
//             iter += (i % offset);
//         } else {
//             iter += (i-offset) % range;
//         }
//         break;
//     case PAL60:
//         iter = PAL60Pattern.begin();
//         range = PAL60Pattern.size();
//         //logic
//         break;
//     case PAL50:
//         //pattern
//         //logic
//         break;
//     case NTSCJ:
//         //pattern
//         //logic
//         break;
//     default:
//         break;
//     }
//     return *iter;
// }

// bool col_CheckStepPath(std::vector<int>secondarySteps,u32& seed,int i,int stepCalls){
//   if (binary_search(secondarySteps.begin(),secondarySteps.end(),i+1)){
//         LCGn(seed,stepCalls);
//         return true;
//   } else {
//         return false;
//   }
// }
// int colo_RollBackground(u32 &seed,int i, region gameRegion){    
//       return LCGn(seed,col_consultPattern(i,gameRegion));
// }

// /*
// TODO: Solve issue where Jim only burns 1 incrementStep() during beginCycle() instead of two.
// Solve issue where Jim gets completely stuck on phenac gym door.
//     This may require implementing some simulation of collision. 
// */


// void advanceCycle (u32 &seed,NPC &npc, int currentCycle){
//     int framesWalked = 0;
//     npc.beginCycle(seed);
//     //First step:
//     npc.incrementPosition(1);
//     //remaining steps
//     bool distance_decreasing = true;
//     do
//     {
//         distance_decreasing = npc.incrementPosition(2);
//         framesWalked++;
//     } while (distance_decreasing);
//     //adjust for overshoot
//     npc.finishCycle(seed);
//     npc.printNPCData(currentCycle);
// }
// //THIS IS THE FUNCTION USED IN THIS FILE. NOT THE ONE IN NPC.H - THAT FUNCTION IS FOR QUILAVADWITHNPCS.CPP
// std::string npcAction(u32 &seed,NPC &npc, int i){
//     std::string action = "";
//     int factor = 2;
//     switch (npc.getState())
//         {
//         case WALK:
//             action = "**";
//             if (i == 0){ //This initial rate is dependent on the specific map, and entrance.
//                factor = 1;
//             }
//             else if (i == 1){
//                 factor = 5;
//             }
//             npc.incrementPosition(factor);
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
//             npc.incrementPosition(factor); //standard practice? -- WHEN WHERE AND HOW?
//             // if (!(npc.getName()[0] == 'J') || i != 199){
//             //     npc.incrementPosition(factor);
//             // }

//             break;
//         default:
//             std::cout << "STATE INVALID!";
//             break;
//         }
//         return action;
// }
// void initializeNPCSet(u32 &seed,std::vector<NPC>&npcSet,std::string &action,std::ofstream &outF){
//     for (unsigned int i = 0; i < npcSet.size(); i++) //this version returns action.
//     {
//         action += npcSet[i].npcAction_Self(seed);
//     }
//     outF << "Initial cycles begin!\n";
//     outF << "\n";
// }

// void outputToFile(u32 seed, std::string action, std::ofstream &outF,std::ofstream &rawF, bool step){
//     outF << action;
//     outF << std::hex << " : " << seed << " : " << std::dec;
//     if (step){
//         outF << " ++ STEP!";
//     }
//     outF << std::endl;
//     rawF << std::uppercase << std::hex << seed << std::dec << std::endl;
// }

// int main(){

//     //New build command: g++ C:\Users\greni\OneDrive\Documents\GitHub\SecondaryManip\Colo\npcSIM.cpp C:\Users\greni\OneDrive\Documents\GitHub\SecondaryManip\processCore.cpp C:\Users\greni\OneDrive\Documents\GitHub\SecondaryManip\Colo\NPCs\NPC.cpp C:\Users\greni\OneDrive\Documents\GitHub\SecondaryManip\Colo\NPCs\duration.cpp -o npcSIM_NEW
//     std::ofstream outF("npcSim.txt");
//     std::ofstream outFRaw("npcSimRaw.txt");

//     //~~~~~~~~~CONFIG~~~~~~~~~~~~
//     u32 inputSeed = 0xD3439443;//0xE8043E11 --D3439443 becomes: BA93C44 :
//     int frameWindow = 1000;
//     int callsPerPlayerStep = 1;
//     region version = NTSCU;
//     bool trackSteps = false;
//     //~~~~~~~~~~~~~~~~~~~~~~~~~~~
//     u32 seed = inputSeed;
//     std::string action = "";

//     NPC Girly    = NPC({85,-150}, "G");
//     std::vector<NPC>npcSet = {Girly};
//     std::vector<int>quilavaSteps{5,10,15,20,25,30,35,39,44,49,54,67,88,100,111,119,
//     126,132,138,143,148,153,158,163,168,173,177,182,187,192,197,202,210,217,
//     227,237,252,268,284,292,297,302,307,312,316,321,326,331};
//     std::vector<int>croconawSteps{};
//     std::vector<int>bayleefSteps{};


//     outF << "SEED USED: " << std::hex << inputSeed << std::dec << "\n";
//     initializeNPCSet(seed,npcSet,action,outF);
//     outputToFile(seed,action,outF,outFRaw,0);
//     for (int i = 0; i < frameWindow; i++)
//     {   //standard items:
//         colo_RollBackground(seed,i,version);
//         bool step = false;
//         if (trackSteps){
//         step = col_CheckStepPath(quilavaSteps,seed,i,callsPerPlayerStep);     
//         }
//         //NPCs:
//         std::string action = "";
//         outF << std::setw(3) << i << ": ";
//         for(unsigned int j = 0; j < npcSet.size(); j++){
//             action += npcAction(seed,npcSet.at(j),i);
//             //DEBUG
//             if (npcSet.at(j).getState() == FINISH){
//                 std::cout << npcSet.at(j).getName() << ": Intend: " << std::setprecision(17)<< npcSet[j].getIntendedPos().x << ": " << npcSet[j].getIntendedPos().y << std::endl;
//             //std::cout << "G Wait: " << npcSet.at(2).getWaitTime().getSeconds() << "\n";

//             }
            
//         }
//         // if (i >= 170 && i <= 190){
//         //         std::cout << "DEBUG KAIB:" << npcSet.at(0).getWaitTime().getSeconds() << "\n";
//         //     }
//         //Output
//         outputToFile(seed,action,outF,outFRaw,step);
//         //outF << "(G wait: " << npcSet.at(2).getWaitTime().getSeconds() << ")\n";
//         // if (npcSet.at(4).getState() == FINISH){ //This tracks randall I guess?
//         //     std::cout << "intended: " << std::setprecision(17)<< npcSet[4].getIntendedPos().x << " : " << npcSet[4].getIntendedPos().y << std::endl;
//         // }
        
//     }
//     //     if (i == 199){
//     //         std::cout << std::setprecision(17)<< npcSet[1].getIntendedPos().x << " : " << npcSet[1].getIntendedPos().y << std::endl;
//     //     }
//     //     if (i >= 198 && i < 205){
//     //         std::cout << "x pos: " << std::setw(18) << npcSet[1].getNextPos().x << " y pos: "<< npcSet[1].getNextPos().y << std::endl;
//     //     }
//     // }
//     std::cout << "COMPLETE!";
//     return 0;
// }