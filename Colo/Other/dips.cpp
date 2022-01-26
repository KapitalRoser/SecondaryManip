#include "../../processCore.h"


int main(){
    std::vector<int> pat = {
    0,1,0,
    0,1,0,
    0,
    0,1,0,
    0,1,0,
    0,2,0,2,0,
    0,2,0,2,0,
    0,2,0,
    0,1,0,
    0,2,0,
    0,1,0,
    0,
    0,1,0,
    0,1,0,
    0,2,0,
    0,1,0,
    0,1,0,
    0,
    0,1,0,2,0,
    0,2,0,2,0,
    0}; //hope? nope.
    int smol = 56;
    int mid = 59;
    int wat = smol+mid;    

    int startFrame = 8334;
    int rate;
    int PatPos = 0;

    for (int i = 0, j = 0; i < 4200; i++,j++){
        if (j % rate == 0){
            switch (pat.at(PatPos))
            {
            case 0:
                rate = smol;
                break;
            case 1:
                rate = mid;
                break;

            default:
                rate = wat;
                break;
            }
        PatPos++;
        std::cout << startFrame + i << " : " << rate << "\n";
        if (PatPos >= pat.size()){
            PatPos = 0; //to lazy for proper iter setup.
            std::cout << "end! - ";
        }    
        j = 0;
        }
    }
    return 0;
}
/*
0,1,0,0,1,0,0,0,1,0,
0,1,0,0,2,0,2,0,0,2,
0,2,0,0,2,0,0,1,0,0,
2,0,0,1,0,0,0,1,0,0,
1,0,0,2,0,0,1,0,0,1,
0,0,0,1,0,2,0,0,2,0,
2,0,0

*/