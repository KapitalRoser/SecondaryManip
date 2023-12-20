#include "../NPC.h"
#include "../processCoreLocal.h"


void outputToFile(u32 seed, std::string action, std::ofstream &outF,std::ofstream &rawF){
    outF << action << std::hex << " : " << seed << " : " << std::dec << std::endl;
    rawF << std::uppercase << std::hex << seed << std::dec << std::endl;
}

int main(){

    //New build command: g++ C:\Users\greni\OneDrive\Documents\GitHub\SecondaryManip\Colo\npcSIM.cpp C:\Users\greni\OneDrive\Documents\GitHub\SecondaryManip\processCore.cpp C:\Users\greni\OneDrive\Documents\GitHub\SecondaryManip\Colo\NPCs\NPC.cpp C:\Users\greni\OneDrive\Documents\GitHub\SecondaryManip\Colo\NPCs\duration.cpp -o npcSIM_NEW
    std::ofstream outF("npcSim.txt");
    std::ofstream outFRaw("npcSimRaw.txt");

    //~~~~~~~~~CONFIG~~~~~~~~~~~~
    const u32 inputSeed = 0xD3439443;//0xE8043E11 --D3439443 becomes: BA93C44 :
    const int frameWindow = 1000;
    //This file tracks no steps, assumes noiseless.
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~
    u32 seed = inputSeed;
    std::string action = "";
    NPC Girlie = NPC({85,-150}, "G"); //Example
    // std::vector<NPC>npcSet = {Girlie}; //This technically allows you to add more npcs as needed.
    
    //Init
    outF << "SEED USED: " << std::hex << inputSeed << std::dec << "\n";
    action += Girlie.npcAction_Self(seed); //-- may not need this, may need to remove the phenac init accomodations...
    outputToFile(seed,action,outF,outFRaw); //for init above.
    
    //Core loop
    for (int i = 0; i < frameWindow; i++)
    {   
        action = "";
        outF << std::setw(3) << i << ": ";
        action += Girlie.npcAction_Self(seed);
        outputToFile(seed,action,outF,outFRaw);  
    }
    std::cout << "COMPLETE!";
    return 0;
}