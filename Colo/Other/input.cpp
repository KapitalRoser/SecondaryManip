#include "../../processCore.h"

int main(){
    /*
    finally have a cotool accurate timer, though it uses a rate at (or very near) 33.
    However in game rate is 30fps so idk????
    Three numbers exist: 
    4f, 5f for Eu 50 and 60 -- supposedly.
    33.375 is the correct rate for US and for PAL 60
    32.852 is the correct rate for JPN. *on old tool, new tool appears to use 33.375 for JPN as well.
    40.000 is the correct rate for PAL 50
    Probably should use these numbers to get the most accurate form of seed finding. 
    */
    std::cout << "Enter frame: ";
    float rate = 33.375; //this best matches the rate used by cotool 1.03's seed finder. 
    int frames = 0;
    std::string empt = "";
    std::getline(std::cin,empt);
    std::cout << "Begin!";
    std::chrono::milliseconds duration;
    while (empt != "s"){
        auto start = std::chrono::high_resolution_clock::now();
        std::getline(std::cin,empt);
        auto stop = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        std::cout << frames << " : " << duration.count() << " : " << (duration.count()-10)/rate;
        frames++;  
    }

    return 0;
}
