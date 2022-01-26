#include "../../processCore.h"


//This is all trying to keep things sequential instead of pregenerated like I did for XD, but I'm wondering
//if that might be the simpler option honestly. 


struct offsetData{
  int frameOffset; //up to 201 (i think)
  int patternOffset; //0-2
  int i; //frameIdx
  int j; //patternIdx
};



int emuAdvance(int i, int &j, int offset, std::vector<int>intervals, int &nextAlterFrame){
    //simple case
    bool shouldAdvance = 0; //either 1 or 0.9. In reality it's a long many digit float
    if (i % 2){
        shouldAdvance = 1;
    } else {
        shouldAdvance = 0;
    }

    std::cout << "Pre- " << shouldAdvance << " ";
    if (i == nextAlterFrame){
        shouldAdvance = modifyFrame(i,offset);
        
    }
    //altered case 
    int curGap = consultPattern(i,offset,intervals);
    int alterFrame = i + curGap; //local! -- 1 -1 0.9 repeat.
    // if (i = )
    // if (j == nextAlterFrame){
    //     std::cout << "ENTERED ALT FRAME";
    //     if (i % 3 == offset){
    //         shouldAdvance = 0;
    //         std::cout << " - Altered to zero!";
    //     } else {
    //         shouldAdvance = 1;
    //         std::cout << " - Altered to one!";
    //     }
    //     std::cout << "Next:" << alterFrame;
    //     nextAlterFrame = alterFrame;
    //     //each call, this is incremented.
    //     j++;
    // }
    
}

bool modifyFrame(int i,int offset){
    std::vector<int>gaps = {1,201,98};
    if (consultPattern(i,offset,gaps) == 1){
        return 0; //0.9
    } else {
        return 1; //1
    }
}

int main(){
    offsetData offsets;
    offsets.frameOffset = 77;
    offsets.patternOffset = 1;
    //int offset = 77;
 std::vector<int>gaps = {1,201,98};
    int alt = offsets.frameOffset;
    int localIndex = 0;
    for (offsets.i = 0; offsets.i < 1000; offsets.i++)
    {
        std::cout << offsets.i << ": ";
        bool advance = emuAdvance(offsets.i,localIndex,offsets.frameOffset,gaps,alt);
        if (advance){
            std::cout << " - odd";
        } else {
            std::cout << " - even";
        }
        std::cout << "\n";
    }
    

    return 0;
}