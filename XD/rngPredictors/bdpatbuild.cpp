#include <iostream>
#include <vector>
#include <algorithm>

std::vector<std::vector<int>> buildChangeFrameVectorForMap(int headerIdx, int nudgePairA, int nudgePairB,int numGroups){
    std::vector<std::vector<int>>set;
    std::vector<int>group;
    const std::vector<int> constantsModel {27,28,29,30};
    int valIdx = 0;
    int amtSegmentsInGroup = 9; //makeparam?
    int valBound = 60; //60 or 59 frame align.

    for (int j = 0; j < numGroups; j++){
        for (int k = 0; k < amtSegmentsInGroup;k++){
            // valBound = 60;
            //This is the one that matters the most.
            int pair = k % 9;

            if (pair == 0){ 
                valIdx = headerIdx;
            } else if (pair == nudgePairA){
                valBound = 59;
            } else if (pair == nudgePairB){
                valIdx = 3;
                valBound = 61;
            } else {
                valIdx = 2;
                valBound = 60;
            }
            group.push_back(constantsModel.at(valIdx)); //lead and low pair
            group.push_back(valBound-constantsModel.at(valIdx)); //follow and hi pair.
        }
        set.push_back(group);
        group.clear();
    }

    return set;
}
std::vector<int>convertFrom3LSegmentToFrames(std::vector<std::vector<std::vector<int>>> cFV,int initialOffset){ 
//Accomplishes 2 things: consolidates data into 1 long vec of segments, then converts them to a specific frame.
  int frame = initialOffset;
  std::vector<int>framesList;
  //3 layer vec:
  for (unsigned int set = 0; set < cFV.size(); set++)
  {
    for (unsigned int group = 0; group < cFV.at(set).size(); group++)
    {
     for (unsigned int segment = 0; segment < cFV.at(set).at(group).size(); segment++)
     {
        frame += cFV.at(set).at(group).at(segment);
        framesList.push_back(frame);
     }
      
    }
  }
//   std::cout << "FList: " << framesList.size();
  return framesList;
}
std::vector<int>convertFrom2LSegmentToFrames(std::vector<std::vector<int>> cFV,int initialOffset){ 
//Accomplishes 2 things: consolidates data into 1 long vec of segments, then converts them to a specific frame.
  int frame = initialOffset;
  std::vector<int>framesList;
  //2 layer vec:
    for (unsigned int group = 0; group < cFV.size(); group++)
    {
        for (unsigned int segment = 0; segment < cFV.at(group).size(); segment++)
        {
            frame += cFV.at(group).at(segment);
            framesList.push_back(frame);
        }
      
    }
//   std::cout << "FList: " << framesList.size();
  return framesList;
}
std::vector<int> buildChangeFrameVectorForFightCommon(){
    std::vector<std::vector<int>>set;
    std::vector<int> interruptLengths{9,10};
    std::vector<int> mainLengths{97,98,99};
    std::vector<int>group; //just handles lengths, not values.
    const int NUM_GROUPS = 9;
    
    for (int sets = 0; sets < NUM_GROUPS; sets++)
    {
        group = {97,9,98,9,97,9,97,10,97,10}; //add to declaration statement later.
        set.push_back(group);
    }
    //add standard variance block:
    //assumes 9 groups, if need more just add more vectors.
    std::vector<int> dfSet0 = {1,2,4,6,8};
    std::vector<int> dfSet1 = {1,2,4,4,8};
    std::vector<int> dfSet2 = {0,2,4,6};
    std::vector<int> dfSet3 = {0,0,2,4,4,8};
    std::vector<int> dfSet4 = {0,0,4,4,8};
    std::vector<int> dfSet5 = {0,0,4,4,8};
    std::vector<int> dfSet6 = {0,2,4,4,8};
    std::vector<int> dfSet7 = {0,0,2,5,6};
    std::vector<int> dfSet8 = {0,0,4,5};

    std::vector<std::vector<int>> allDelayFrames = {dfSet0,dfSet1,dfSet2,dfSet3,
                                                    dfSet4,dfSet5,dfSet6,dfSet7,dfSet8};

    for (unsigned int i = 0; i < set.size(); i++)
    { //iterates through dfSets
     for (unsigned int j = 0; j < set.at(i).size(); j++)
     { //iterates through set values.
        while (std::binary_search(allDelayFrames.at(i).begin(),allDelayFrames.at(i).end(),j)){
            set.at(i).at(j)++;
            allDelayFrames.at(i).erase(allDelayFrames.at(i).begin());
        }
     }
    }
    return convertFrom2LSegmentToFrames(set,-59);
    //reference chart:
    /*
    0   1   2     3  4    5  6   7   8   9
    37, 10, 99,   9, 98,  9, 98, 10, 98, (10),
    97, 10, 99,   9, 99,  9, 97, 10, 98, (10),
    98,  9, 99,   9, 98,  9, 98, 10, 97, (10), 

    99,  9, 99, (9), 99,  9, 97, 10, 98, (10),
    99,  9, 98, (9), 99,  9, 97, 10, 98, (10),

    99,  9, 98, (9), 99,  9, 97, 10, 98, 10,
    98,  9, 99, (9), 99,  9, 97, 10, 98, 10, 
    99,  9, 99, (9), 97, 10, 98, 10, 97, 10,

    99,  9, 98, (9), 98, 10, 45, 10, 96, (10),
    73
    */
        
}
std::vector<int> buildCFVFightCommonValues(){
  std::vector<std::vector<int>> sets; //uses a 2 layer, then converted into one layer for the simplicity in adding the split segments.
  //could be consolidated into 1 I suppose.
  std::vector<int>outputVal;
  //placements do not account for split 8 & 9 segments.
  for (int i = 0; i < 9; i++)
  {
    for (int j = 0; j < 5; j++)
    {
      switch (j)
      {
      case 0: //assumes splits are worth 9, will add 8 before or after the 9 as needed.
        sets.at(i).push_back(9);
        break;
      case 1:
        sets.at(i).push_back(9);
        break;
      case 2:
        sets.at(i).push_back(8);
        break;
      case 3:
        sets.at(i).push_back(8);
        break;
      case 4:
        sets.at(i).push_back(9);
        break;
      }
      
    }
    //adds back the 2nd component of the split segments.
    if (i < 5 || i == 8){
    sets.at(i).push_back(8);  //appears after the 9
    }else {
      sets.at(i).insert(sets.at(i).begin()+1,8); //appears before the 9.
    }
  }
  for (unsigned int i = 0; i < sets.size(); i++)
  {
    for (unsigned int j = 0; j < sets.at(i).size(); j++)
    {
      outputVal.push_back(sets.at(i).at(j));
    }
  }
  
  return outputVal;
}
int main(){

    int initialOffset = 0;
    std::vector<int> fullSequence = buildChangeFrameVectorForFightCommon();






    //FULL PRINT LOOPS.
    int count = 0;
    for (unsigned int i = 0; i < fullSequence.size(); i++)
    {
        std::cout << fullSequence.at(i);
        if (i < fullSequence.size()-1){
            std::cout << ", ";
        }
        if (i % 10 == 9){
            std::cout << "\n";
        }
        
    }

    return 0;
}

        // for (int segments = 0; segments < 10; segments++)
        // {
        //     //may actually consider switch case here lmao.
        //     //This might be the most confusing way to encode this pattern, im so sorry.
        //         //Handles interrupts.

        //         //fixed values, in all sets.
        //         if (segments == 7 || segments == 9){
        //             group.push_back(interruptLengths.at(1));
        //         } else if (segments == 3){ // would also like to include seg 1 in this but oh well.
        //             group.push_back(interruptLengths.at(0));
        //         }
        //         //variable values, only at bookends of the data.
        //         if (segments == 1){
        //             if(sets == 0 || sets == 1){
        //                 group.push_back(interruptLengths.at(1));
        //             } else {
        //                 group.push_back(interruptLengths.at(0));
        //             }
        //         }
        //         if (segments == 5){
        //             if (sets == 7 || sets == 8){
        //                 group.push_back(interruptLengths.at(1));
        //             } else {
        //                 group.push_back(interruptLengths.at(0));
        //             }
        //         }
        //         //Handles mains.
        //         //fixed ish. limited to 2 of 3 values.
        //         if (segments == 2){
        //             if (sets == 4 || sets == 5 || sets == 8){
        //                 group.push_back(mainLengths.at(1));
        //             } else {
        //                 group.push_back(mainLengths.at(2));
        //             }
        //         }
        //         if (segments == 6){
        //             if (sets == 0 || sets == 2 || sets == 7){
        //                 group.push_back(mainLengths.at(1));
        //             } else {
        //                 group.push_back(mainLengths.at(0));
        //             }
        //         }
        //         if (segments == 8){
        //             if (sets == 2 || sets == 7 || sets == 8){
        //                 group.push_back(mainLengths.at(0));
        //             } else {
        //                 group.push_back(mainLengths.at(1));
        //             }
        //         }

        //         //loose values, all 3 are possible on their own.
        //         if (segments == 0){
        //             if (sets < 2){
        //                 group.push_back(mainLengths.at(0));
        //             } else if (sets == 2 || sets == 6){
        //                 group.push_back(mainLengths.at(1));
        //             } else {
        //                 group.push_back(mainLengths.at(2));
        //             }
        //         }
        //         if (segments == 4){
        //             if (sets == 7){
        //                 group.push_back(mainLengths.at(0));
        //             } else if (sets == 0 || sets == 2 || sets == 8){
        //                 group.push_back(mainLengths.at(1));
        //             } else {
        //                 group.push_back(mainLengths.at(2));
        //             }
        //         }
        //     }