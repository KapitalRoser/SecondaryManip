#include "../../processCore.h"
#include <map>  
struct alteration {
  int frame, amount;
};

void buildChangeFramesBlink(std::vector<alteration> &set,int frameOffset, int patternOffset){
    //This function may be generic enough to be used in other places too
    //where you want to combine/sync two pattern layers
    std::vector<int> vals {1,1,0};
    std::vector<int> gaps {98,1,201};
    int size = 4000; //in total frames, can also set to desired number of gaps, just change the middle item of the for loop back to pattern offset.
    for (patternOffset; frameOffset < size; patternOffset++)
    {
      //std::cout << frameOffset << " : " << vals[patternOffset%vals.size()] << "\n";
      alteration a;
      a.frame = frameOffset;
      a.amount = vals[patternOffset%vals.size()];
      set.push_back(a);
      frameOffset+= gaps[patternOffset%gaps.size()];
    }
    
    //debug
    for (unsigned int i = 0; i < set.size(); i++)
    {
      printf("%d : %d\n",set[i].frame,set[i].amount);
    }
}

int main(){
  std::vector<alteration> set;
  buildChangeFramesBlink(set,12,0);
  std::cout << set.size() << " is the size of the set";



    
    return 0;
}

