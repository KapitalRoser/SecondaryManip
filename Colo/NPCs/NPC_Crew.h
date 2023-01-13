#include "../../processCore.h"
#include "refactoredNPC.h"
class NPC_Crew
{
private:
    /* data */
    std::vector<NPC> set = {};
    std::string status = "";
    u32 userInputSeed;
    bool seedWasSpecified = false;
    bool firstFrame = false;
    //Framecount here is frames since simulation began.
    //Will incremement until it is cleared with reset or redefined.
    //Only used for the very first frame. That said, might be nice for status?


    //Everything else goes here
public:
    NPC_Crew(std::vector<NPC>in);
    NPC_Crew(std::vector<NPC>in,u32 inSeed); //Since there aren't any good default values that fit u32, 
    //a 2nd constructor is worth it. It also can include the flag to determine whether it was specified in the first place
    //Assignment
    //Copy
    void push(NPC in);
    bool pop(); //returns success or failure.
    //erase at specific ID or Name?
    void advance(u32 &seed);
    std::string getStatus();
    void reset(); //resets to seedless. Option to add seed?
    bool isEmpty();
    unsigned int size();
    //Print contents? Or is it covered by status?
};
