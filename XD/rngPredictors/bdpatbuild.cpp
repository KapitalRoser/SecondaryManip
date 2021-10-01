#include <iostream>
#include <iomanip>
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
std::vector<int> debugPrint2DVec(std::vector<std::vector<int>>set){
  //FOR DEBUG:
        for (unsigned int i = 0; i < set.size(); i++){
          for (unsigned int j = 0; j < set.at(i).size(); j++){
            std::cout << set.at(i).at(j) << ", ";
            if (j % 2 == 1){
              std::cout << std::endl;
            }
          }
          std::cout << "\n\n";
          
        }
}
std::vector<int>convertFrom3DSegmentToFrames(std::vector<std::vector<std::vector<int>>> cFV,int initialOffset){ 
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
std::vector<int>convertFrom2DSegmentToFrames(std::vector<std::vector<int>> cFV,int initialOffset){ 
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
    
    for (int currentSet = 0; currentSet < NUM_GROUPS; currentSet++)
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
    return convertFrom2DSegmentToFrames(set,-59);
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
std::vector<int> SecondBuildChangeFrameVectorForFightCommon(){
    std::vector<std::vector<int>>set;
    std::vector<int> interruptLengths{9,10};
    std::vector<int> mainLengths{97,98,99};
    
    std::vector<int>SplitSegLengths = {3,7,4,6,7,3,1,8,7,3,2,7,9,1,3,6,5,4,5,4,8,1,9,1}; //THESE ARE NOT KNOWN, only a couple rules exist:
    //if it sums to 9, it goes Eight then Nine. if it sums to 10, it leads nine into eight.
    //also if the main segement length pattern is right then there must be at least 1 of each value in the split,
    //so at worst it is 8 frames of 50/50 variance.

    std::vector<int>group; //just handles lengths, not values.
    const int NUM_GROUPS = 9;
    
    for (int ithSet = 0; ithSet < NUM_GROUPS; ithSet++)
    {
        group = {97,9,98,-1,97,9,97,10,97,-1}; //add to declaration statement later.
        //-1 represents a split segment

        //constants block
        if (ithSet < 3){
          group.at(3) = 9;
        } else if (ithSet >= 5 && ithSet < 8){
          group.at(9) = 10;
        }

        set.push_back(group);
    }    

    
    //add standard delayFrame variance block:
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

    
    //customize pattern as outlined in dfSets.
    
    for (unsigned int i = 0; i < set.size(); i++)
    { //iterates through dfSets
     for (unsigned int j = 0; j < set.at(i).size(); j++)
     { //iterates through set values.
        while (std::binary_search(allDelayFrames.at(i).begin(),allDelayFrames.at(i).end(),j)){ 
          //When i.at(j) reaches a modify slot, modify the value there and remove from DF set.
            set.at(i).at(j)++;
            allDelayFrames.at(i).erase(allDelayFrames.at(i).begin());
        }
     }
     //All single slot changes for a row need to occur first before any insertion of split segments can happen.
     //as such the j loop is run twice.
     for (unsigned int j = 0; j < set.at(i).size(); j++)
     {
       if (set.at(i).at(j) == -1){
        set.at(i).insert(set.at(i).begin()+j,{SplitSegLengths.at(0),SplitSegLengths.at(1)});
        //Adds a pair of split lengths into row i at position j.

        SplitSegLengths.erase(SplitSegLengths.begin(),SplitSegLengths.begin()+2);
        //removes that item from the vec so that it iterates through all pairs.

        set.at(i).erase(set.at(i).begin()+j+2);
        //Erases the '-1' flag.
      }
     }
    }

    //ONE OFF CHANGES:
    set.at(8).at(7) -= 52;
    set.at(8).at(9)--;
    
    // //DEBUG:
    //     for (unsigned int i = 0; i < set.size(); i++){
    //       for (unsigned int j = 0; j < set.at(i).size(); j++){
    //         std::cout << set.at(i).at(j) << ", ";
    //       }
    //       std::cout << std::endl;
    //     }

    return convertFrom2DSegmentToFrames(set,-60);
    //reference chart:
    /*
    
  ixj 0   1   2     3  4    5  6   7   8   9
   0 37, 10, 99,   9, 98,  9, 98, 10, 98, (10),
   1 97, 10, 99,   9, 99,  9, 97, 10, 98, (10),
   2 98,  9, 99,   9, 98,  9, 98, 10, 97, (10), 

   3 99,  9, 99, (9), 99,  9, 97, 10, 98, (10),
   4 99,  9, 98, (9), 99,  9, 97, 10, 98, (10),

   5 99,  9, 98, (9), 99,  9, 97, 10, 98, 10,
   6 98,  9, 99, (9), 99,  9, 97, 10, 98, 10, 
   7 99,  9, 99, (9), 97, 10, 98, 10, 97, 10,

   8 99,  9, 98, (9), 98, 10, 45, 10, 96, (10),
    73
    */
        
}
std::vector<int> buildCFVFightCommonValues(){
  std::vector<std::vector<int>> sets; //uses a 2 layer, then converted into one layer for the simplicity in adding the split segments.
  
  //could be consolidated into 1 I suppose.
  std::vector<int>outputVal;
  int NUM_ROWS = 9;
  //placements do not account for split 8 & 9 segments.
  for (int i = 0; i < NUM_ROWS; i++)
  {
    //Assumes split values are 9, unless otherwise changed. broadly speaking a 9x88x
    sets.push_back({7,9,7,9,7,8,7,8,7,9});
    //adds back the 2nd component of the split segments.
    if (i < 5 || i == 8){
      sets.at(i).push_back(8);  //appears after the 9
    }
    if (i >= 3){
      sets.at(i).insert(sets.at(i).begin()+3,8);
    }

  }
  //DEBUG:
        for (unsigned int i = 0; i < sets.size(); i++){
          for (unsigned int j = 0; j < sets.at(i).size(); j++){
            std::cout << sets.at(i).at(j) << ", ";
          }
          std::cout << std::endl;
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

std::vector<int> buildCFVCamShake(){
  //How come new data is weird while old data is stable???
  std::vector<std::vector<int>>set;
  std::vector<int>group;
  const int NUM_GROUPS = 9;
  const int NUM_ROWS = 9;


  //Create standard format:
  for (int i = 0; i < NUM_GROUPS; i++)
  {
    for (int j = 0; j < NUM_ROWS; j++)
    {
      if (j == 0){ //header pair
        group.push_back(30);
        group.push_back(30);
      } else {
        group.push_back(32);
        group.push_back(28);
      }
    }
    set.push_back(group);
    group.clear();
  }
  //30-30 followed by 8 rows of 32-28 to make the group.
  
  //BEYOND THIS POINT THESE ARE RECORDED VALUES, AND NOT FULLY UNDERSTOOD YET.
  std::vector<int>lagLocation =  {5, -1, 3, 3, 3, 5, 5, 5, 4}; //row in group where the 27 lies.
  std::vector<int>dropFrameLoc = {4, -1, 2,17,-1,-1, 1,16,-1}; //in raw position in group, since single value changed.
  //(-1 == skip)
  for (unsigned int i = 0; i < set.size(); i++)
  {
    //Add 27-33 lags. 27 is always the 2nd in the pair and 33 is always immediately after.
    if (lagLocation.at(i)>=0){
      int first = lagLocation.at(i)*2+1;
      int second = (lagLocation.at(i)+1)*2;
      set.at(i).at(first)--; //becomes 27
      set.at(i).at(second)++; //becomes 33
    }
    //remove the weird dropped frames (semi pattern: -1 0 -1 -1 0 0 -1 -1  0)
    if (dropFrameLoc.at(i)>=0){
      int position = dropFrameLoc.at(i);
      set.at(i).at(position)--;
    }

  }
      //remove the dropped whole rows.
      set.at(5).erase(set.at(5).begin(),set.at(5).begin()+2); //2 dropped rows in group 6
      
      set.at(5).pop_back();
      set.at(5).pop_back(); //remove final pair.

      //Add in starting pre-pattern values.
      set.insert(set.begin(),{31,28,32,28});
      return convertFrom2DSegmentToFrames(set,0); //lucky lol.
}

int main(){

    int initialOffset = 0;
    std::vector<int> fullSequence = buildCFVCamShake();

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
/*
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

31, 28, 
32, 28,

30, 30,

32, 28,
31, 28, 

32, 28, 
32, 28, 
32, 27, 
33, 28, 
32, 28, 
32, 28,

30, 30, 
32, 28, 
32, 28, 
32, 28, 
32, 28, 
32, 28, 
32, 28, 
32, 28, 
32, 28,

30, 30, 
31, 28, 
32, 28, 
32, 27, 33, 28, 32, 28, 32, 28, 32, 28, 32, 28,
30, 30, 32, 28, 32, 28, 32, 27, 33, 28, 32, 28, 32, 28, 32, 28, 32, 27, 
30, 30, 32, 28, 32, 28, 32, 27, 33, 28, 32, 28, 32, 28, 32, 28, 32, 28,
28, 32, 28, 32, 28, 32, 28, 32, 27, 33, 28, 32, 28,
30, 29, 32, 28, 32, 28, 32, 28, 32, 28, 32, 27, 33, 28, 32, 28, 32, 28,
30, 30, 32, 28, 32, 28, 32, 28, 32, 28, 32, 27, 33, 28, 32, 28, 31, 28,
30, 30, 32, 28, 32, 28, 32, 28, 32, 27, 33, 28, 32, 28, 32, 28, 32, 28,
*/