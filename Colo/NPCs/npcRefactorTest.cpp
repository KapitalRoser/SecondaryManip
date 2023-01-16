#include "../../processCore.h"
#include "NPC.h"
//#include "../NPC.h"

int main(){

    d_coord anchor;
    anchor.x = 85;
    anchor.y = -150;
    NPC kaib = NPC(anchor,"K",STANDARD);
    // NPC kaib = NPC(anchor,"K");
    // NPC Beatrice = NPC(anchor,"B");

    // NPC kaib = NPC({})

    // u32 seed = 0x7B016A28;
    // kaib.InitialXY(seed);
    // std::cout << kaib.getName() << ": " << kaib.getAnchor().x << "," << kaib.getAnchor().y << "\n"
    // << "Speed: " << kaib.getSpeedFactor() << ", State:" << kaib.getState() << "\n"
    // << "Intended: (" << kaib.getIntendedPos().x << ", " << kaib.getIntendedPos().y << ")\n\n";
    // seed = 0x7B016A28;
    // Beatrice.chooseDestination(seed);
    // std::cout << Beatrice.getName() << ": " << Beatrice.getAnchor().x << "," << Beatrice.getAnchor().y << "\n"
    // << "Speed: " << Beatrice.getSpeedFactor() << ", State:" << Beatrice.getState() << "\n"
    // << "Intended: (" << Beatrice.getIntendedPos().x << ", " << Beatrice.getIntendedPos().y << ")";
    




    return 0;
}