#include "../processCore.h"
enum bgFrames {H = 116, L = 76, A = 114, O = 154}; //High, Low, Alpha, Omega

int col_consultPattern(int i, region gameRegion){
    std::vector<int>NTSCUPattern = {H,H,L,H,L}; //HHLHL
    std::vector<int>PAL60Pattern = {
    A,H,
    H,L,H, O,L,
    H,H,L, O,H,
    L,H,L, O,H,
    H,L,H, O,L,
    H,L,H, O,H,
    L,H,L, O,H,
    L,H,H, O,L,
    H,L,H, O,L,
    H,H,L, O,H,
    L,H,H, A,H //Last A,H necessary?
    };
    std::vector<int>::iterator iter;
    int offset = 3; //const?
    int range = 0;
    switch (gameRegion)
    {
    case NTSCU:
        iter = NTSCUPattern.begin();
        range = NTSCUPattern.size();
        //Does this logic work for all patterns now?
        if (i < offset){
            iter += (i % offset);
        } else {
            iter += (i-offset) % range;
        }
        break;
    case PAL60:
        iter = PAL60Pattern.begin();
        range = PAL60Pattern.size();
        //logic
        break;
    case PAL50:
        //pattern
        //logic
        break;
    case NSTCJ:
        //pattern
        //logic
        break;
    default:
        break;
    }
    return *iter;
}

//slightly depreciated (currently just used in npcs.cpp):
bool col_CheckStepPath(std::vector<int>secondarySteps,u32& seed,int i,int stepCalls){
  if (binary_search(secondarySteps.begin(),secondarySteps.end(),i+1)){
        LCGn(seed,stepCalls);
        return true;
  } else {
        return false;
  }
}
int colo_RollBackground(u32 &seed,int i, region gameRegion){    
      return LCGn(seed,col_consultPattern(i,gameRegion));
}