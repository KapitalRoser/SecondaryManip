#include "../processCore.h"


int main(){

    u32 seed = 0x555a147c;
    LCGn_BACK(seed,1002);
    std::cout << std::hex << seed;
    return 0;
}