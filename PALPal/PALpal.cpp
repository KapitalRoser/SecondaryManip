#include "../processCore.h"

int main(){

    const int namingValue = 2;
    const int rumbleValue = 40; //note in colo this is 20 calls instead.
    const int memcardValue = 1009;
    u32 userInputRerollSeed = 0xEE5DC61E;
    u32 userInputTargetSeed = 0x0; //eventually, once I locally implement rerolling, will Only need this input.

    int targetInput = 562;
    u32 seed = userInputRerollSeed;
    int target = targetInput - 115;
    u32 debugSeed = 0;
    int x = targetInput;
    int instructions[4] = {0,0,0,0};
    /*

    METHOD:
    Get the seed of the NTSC REROLL at the bottom, on the final reroll's page. Unfortunately can't copy/paste.

    Get Seed of Target espeon.

    Reroll up to the target like you normally would.

    Program calc's distance from reroll seed to target, and then gives you instructions to get to a value 2 frames before
    the target seed, which is 1000 frames before the seed you see in the program.

    From there, follow the program's instructions: 
    [0] = memcard reload. take memcard out and continue without saving. then reinsert memcard, back to main menu and re enter. 1009 calls.
    [1] = Rumble switch. Go into options menu, flip it from on to off or vice versa, and accept the save prompt. 40 calls.
    [2] = Naming screen back outs. Enter name screen and press b to return to main menu. 2 calls.
    [3] = Traditional re-roll. WIP. Unknown call amount. Will know once implemented.


    example: target is ntsc 268 rerolls out. 
    351C941E initial.
    8A55887A 10 rerolls to ntsc target.
    Frame 562.

    F9FD38FF on title screen.

    may need to locally implement rerolling function T_T
    */
    while (x > 0){
        if (x >= 1009){
        instructions[0]++;
        x -= 1009;
    } else {
        if (x >= 40){
            instructions[1] = x / 40; //integer division intentional, want to always round down for safety.
            x -= instructions[1]*40;
        } else {
            if (x > 1){
                instructions[2] = x / 2;
                x -= instructions[2]*2;
            } else {
                std::cout << "Rip - uneven";
                x = 0;
            }
        } //expect 14
    }
    }
    


    // LCGn(seed,2);
    // std::cout << "Initial 2 calls, result: " << std::hex << seed << std::endl;
    // for (int i = 0; i < 724; i++)
    // {
    //     rollRNGwithHiSeed(seed);
    // }


    // for (int i = 0; i < target; i++)
    // {
    // rollRNGwithHiSeed(seed);
    // debugSeed = seed;
    // if (i == target -1){
    //     std::cout <<"AIM FOR THIS: " << std::hex << debugSeed << std::endl;
    // }
    // LCGn(debugSeed,1000);
    // }
    

    // for (int i = 0; i < 116; i++){
    //     rollRNGwithHiSeed(seed);
    // }
    // std::cout << "NameScreen Seed: " << std::hex << seed << std::endl;
    std::cout << "Debug seed: " << debugSeed << " at target: " << std::dec << target + 115 << std::endl << std::endl;
    std::cout << "To reach target, peform: \n"
    << instructions[0] << " memory card reload(s),\n"
    << instructions[1] << " rumble switch(es),\n"
    << instructions[2] << " naming screen back-outs.\n";
    



    return 0;
}