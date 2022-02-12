#include "../processCore.h"

int main(){
    u32 seed = 0;
    int testAmt = 2147483647;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < testAmt; i++)
    {
        LCG(seed);
    }
    auto stop = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "Test LCG: " << duration.count() << " ms.\n";
    start = std::chrono::high_resolution_clock::now();
    LCGn(seed,testAmt);
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "Test LCGn: " << duration.count() << " ms.\n";
    //Result: LCGn is extremely efficient, takes nearly no time at all to execute. Love math.
    return 0;
}