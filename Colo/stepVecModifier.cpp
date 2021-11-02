#include "../processCore.h"

int main(){

    int stepsOffset = 1;

    std::vector<int>quilavaSteps{5,10,15,20,25,30,35,39,44,49,54,67,88,100,111,119,
    126,132,138,143,148,153,158,163,168,173,177,182,187,192,197,202,210,217,
    227,237,252,268,284,292,297,302,307,312,316,321,326,331};

    std::vector<int>fixedSteps = quilavaSteps;
    std::cout << "{";
    for (unsigned int i = 0; i < quilavaSteps.size(); i++)
    {
        quilavaSteps.at(i) = fixedSteps.at(i) + stepsOffset;
        std::cout << quilavaSteps.at(i);
        if (i < quilavaSteps.size()-1){
         std::cout << ",";
        }
        
    }
    std::cout << "};";

    return 0;
}