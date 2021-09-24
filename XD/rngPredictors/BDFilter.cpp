#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <numeric>
using namespace std;

uint32_t LCG(uint32_t& seed){
  seed = seed * 214013 + 2531011;
  return seed;
}
uint32_t modpow32(uint32_t base, uint32_t exp)
{
  uint32_t result = 1;
  while (exp > 0)
  {
    if (exp & 1)
      result = result * base;
    base = base * base;
    exp >>= 1;
  }
  return result;
}
uint32_t LCGn(uint32_t& seed, const uint32_t n)
  {
    uint32_t ex = n - 1;
    uint32_t q = 0x343fd;
    uint32_t factor = 1;
    uint32_t sum = 0;
    while (ex > 0)
    {
      if (!(ex & 1))
      {
        sum = sum + (factor * modpow32(q, ex));
        ex--;
      }
      factor *= (1 + q);
      q *= q;
      ex /= 2;
    }
    seed = (seed * modpow32(0x343fd, n)) + (sum + factor) * 0x269EC3;
    return seed;
  }
vector<int> decimalReadNumbersFromFile(string fileName)
{
    uint32_t value;
    int lineRead = 0;
    vector<int> data; //Setup
    ifstream file(fileName);
    if (file.fail())
    {
        cout << "File inaccessible";
        exit(EXIT_FAILURE);
    }
    while (!(file.fail()))
    {   /*Keeping the pattern in raw ints of rolls is a bigger filesize than
          encoding everything as individual chars. However this makes it easy to import
          patterns with odd or new values */
        file >> lineRead; 
        data.push_back(lineRead);
    }
    file.close();
    return data;
}
vector<int> buildChangeFrameVector(string fountain, unsigned int size){ 
  //This function takes a predetermined set of pattern intervals for manipulation on a macro level.
  //It populates an array to convert from a list of durations -> a list of specific frames.
  //One could redesign later functions to use the same duration based format but not really worth the trouble.
  //Honestly can't tell definitively on what grounds do the offsets and alignments change between savestates and rng states.
  int initialOffset = 0; //Does this vary with pattern?
  int currentGap = 0;
  int alignment = 0;
  vector<int> possibleIntervals;
  if (fountain == "Mid"){ 
    possibleIntervals = {127,124,124};
    initialOffset = 4; //4 default, -2,[1],4,7
    alignment = 0;  //0,1,2. 0 default. dafuq is this based on??? can this be controlled for?? alignment hasn't shifted but initial offset has.
  } else if (fountain == "North"){
    possibleIntervals = {250,249,251};
    initialOffset = 7; //7 default, 7,10
    alignment = 2;  // 2 default.
  }
  //in this case, alignment is really just an anchor from which to begin the pattern cycle, rather than the literal position of an off value in the pattern
  //like I would for an aab.
  int averageValue = 0;
  int denom = possibleIntervals.size();
  if (denom != 0){
    averageValue = accumulate(possibleIntervals.begin(),possibleIntervals.end(),0)/denom;
  }
  vector<int>changeFrames{initialOffset};//initial offset  

  for (int i = alignment; i<(size-initialOffset)/averageValue + alignment;i++){ //condition is # of intervals will fit within the filesize.
    currentGap = possibleIntervals.at(i%3);
    if (i == alignment){
      changeFrames.at(0) = changeFrames.at(0) + currentGap;
    } else {
      changeFrames.push_back(changeFrames.back()+currentGap);
    }
  }
  return changeFrames;
}
void checkChangeFrame(vector<int>segments,int currentFrame, bool &delayFrame){
  if (binary_search(segments.begin(),segments.end(),currentFrame)){ //if current frame is one of the change frames, then execute.
    delayFrame = !delayFrame; //flips value. Only works with binary bool, could modify for use in fightCommon.
  }
}

std::vector<int>convertFrom3LSegmentToFrames(std::vector<std::vector<std::vector<int>>> cFV,int initialOffset){ //Accomplishes 2 things: consolidates data into 1 long vec of segments, then converts them to a specific frame.
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
  //cout << "FList: " << framesList.size();
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
std::vector<int> buildChangeFrameVectorForFightCommon(){
    std::vector<std::vector<int>>set;
    std::vector<int> interruptLengths{9,10};
    std::vector<int> mainLengths{97,98,99};
    
     std::vector<int>SplitSegLengths = {3,7,4,6,7,3,1,8,7,3,2,7,9,1,3,6,5,4,8,1,9,1}; //THESE ARE NOT KNOWN, only a couple rules exist:
    //if it sums to 9, it goes Eight then Nine. if it sums to 10, it leads nine into eight.
    //also if the main segement length pattern is right then there must be at least 1 of each value in the split,
    //so at worst it is 8 frames of 50/50 variance.

    std::vector<int>group; //just handles lengths, not values.
    const int NUM_GROUPS = 9;
    
    for (int sets = 0; sets < NUM_GROUPS; sets++)
    {
        group = {97,9,98,-1,97,9,97,10,97,-1}; //add to declaration statement later.
        //-1 represents a split segment
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
      if (set.at(i).at(j) == -1){
        // set.at(i).insert(set.at(i).begin()+i,SplitSegLengths.at(1));
        // set.at(i).insert(set.at(i).begin()+i,SplitSegLengths.at(0));
        set.at(i).insert(set.at(i).begin()+i,{SplitSegLengths.at(0),SplitSegLengths.at(1)});
        SplitSegLengths.erase(SplitSegLengths.begin(),SplitSegLengths.begin()+1);
        set.at(i).erase(set.at(i).begin()+i+2);
      }
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


std::vector<int> establishCFVMap(int initialOffset)
{
  //Primary fault in this pattern is that I don't know if these sets repeat, however getting more data without lua is extremely time consuming.
  std::vector<std::vector<int>>setA = buildChangeFrameVectorForMap(0,3,4,3);
  std::vector<std::vector<int>>setB = buildChangeFrameVectorForMap(1,5,6,3);
  std::vector<std::vector<int>>setC = buildChangeFrameVectorForMap(0,10,10,2); //10s effectively remove the 29-30-30-31 sections.
  std::vector<std::vector<int>>setD = buildChangeFrameVectorForMap(0,3,4,2);

  std::vector<std::vector<std::vector<int>>>fullSequence;


  //corrections, unexplained fluctuations:
  setA.at(2).erase(setA.at(2).begin()+14,setA.at(2).end()); //cuts off final set 2 pairs early.
  setA.at(1).at(5)--; //lost frame, happens every 2nd group (proven in this entity). Specific location within group not yet known.

  setB.at(2).at(0)--; //Final header is 1 idx less than the other two.
  setB.at(2).at(1)++;

  setB.at(0).at(7)--; //lost frame adjustments.
  setB.at(2).at(4)--;

  setC.at(1).at(1)--; //lost frame adjustment.

  setD.at(0).erase(setD.at(0).begin()+16,setD.at(0).end()); //First group ends 1 pair early
  setD.at(1).erase(setD.at(1).begin()+3,setD.at(1).end()); //end of data set, not a fluctuation in itself.
  setD.at(1).at(1)--; //lost frame adjustment.

  fullSequence = {setA,setB,setC,setD};

  //F it, 3 layered vectors are too much headache. Prep the vec for conversion to frames.
  
  return convertFrom3LSegmentToFrames(fullSequence,initialOffset);
}

int main(){
    const std::string FILE_EXTENSION = ".txt";
    std::string FILE_NAME = "";
    std::cout << "Enter filename: ";
    getline(cin, FILE_NAME);
    std::vector<int>inputValues = decimalReadNumbersFromFile(FILE_NAME + FILE_EXTENSION);
    std::cout << inputValues.size() << endl;
    ofstream outputData ("out" + FILE_NAME + FILE_EXTENSION);
    std::vector<int>segmentLengths;
    std::vector<vector<int>>variances;
    std::vector<int> immediateVarSeg;
    std::vector<int> segVals;
    

    //FOR FIRST:
    std::vector<int>fightCommonLengths = buildChangeFrameVectorForFightCommon();
    std::vector<int>fightCommonValues = buildCFVFightCommonValues();
    
    std::vector<int>fightCommonInterruptLengths;
    std::vector<int>fightCommonMainLengths;

    for (unsigned int i = 0; i < fightCommonLengths.size(); i++)
    {
      /* code */
    }
    

    //FOR MAP:
    int mapOffset = 16;
    bool delayFrame = 0; //1 == 12
    vector<int>mapSegments = establishCFVMap(mapOffset); //yeah it's a 3 layer nested vector, sue me.
    int unexplainedVariant = 1123;


    int segment = 0;
    int current = 0;
    int numPossibleValues = 6;

    bool filterApress = false; //a2b == deckArchive 1, presumably the player.
   
    bool filterFightCommon = true; //Fight_Common
    bool filterMAP = false; //M5_Out_bf
    bool filterCameraShake = false; //camera_shake.fsys
    bool filterDeckArchive = false; // == deckArchive 2, presumably naps.

    bool filterSum = false;

    bool filterCamera = false; //possibly the same as deckArchive2.


    string frameLabel = "";

    //CHANGE THIS AS NEEDED.
    string comment = "";
    int filterAmt = 0; //OBVIOUSLY CHANGE THIS AS NEEDED.

    //Will need to reverse this to figure out where the pattern starts. 
    int positionOffset = 0;
    int nineteenLength = 30;



    int frames = 0;
    int varSegCount = 0;


    for (unsigned int i = 0; i < inputValues.size();i++){

      //segment tracking - now works for multiple values but must define number of values.
      frames = inputValues.at(i);
      if (i == 0){
        current = frames;
      }
      if (current == inputValues.at(i)){
        segment++;
      } else {
        segVals.push_back(current);
        current = inputValues.at(i);
        segmentLengths.push_back(segment);
        
        segment = 1;
      }
      if (i == inputValues.size()-1){
        segVals.push_back(current);
        segmentLengths.push_back(segment);
      }
      

      if (filterApress){
        //visual Labelling. The 'processor' of sorts.
        if (inputValues.at(i) == 19){
          frameLabel = "Nineteen";
        } else if (inputValues.at(i) == 20) {
          frameLabel = "Twenty";
        } else {
          frameLabel = "---- WEIRD CASE ----";
        }
        
        //alignment changes, will expand with more data:
        if(i == 576){
          positionOffset--;
        }
        //debug + actual filtering.
        if (i % 60 == positionOffset){ 
          filterAmt = 20;
          comment = "60F align!";
        } else if (i % 60 == positionOffset - nineteenLength - 2) {
          filterAmt = 19;
        } else {
          comment = "";
        }
        //Special cases.
        if (i == 604 || i == 664){ //accounts for weird extra frame expansion for these two sections only. One 19 frame becomes a 20 frame.
          frames --;
        }
      }  
      if (filterFightCommon){
        if (inputValues.at(i) == 7){
          frameLabel = "Seven";
        } else if (inputValues.at(i)==8){
          frameLabel = "Eight";
        } else if (inputValues.at(i)==9){
          frameLabel = "Nine";
        } else {
          frameLabel = "OTHER --- ";
        }
        

        // if (filterAmt == 7){ //default case
        //   fightCommonMainLengths.at(0)--;
        // } else {
        //   fightCommonInterruptLengths.at(0)--; ///decrement interrupt.
        //   //when interrupt ends:
        //   if (fightCommonInterruptLengths.at(0) <= 0){
        //     fightCommonInterruptLengths.erase(fightCommonInterruptLengths.begin());
        //     filterAmt = 7;
        //     comment = "";
        //     //restore default case
        //   }
        // }
        // if (comment == "s7"){ //default case
        //   fightCommonMainLengths.at(0)--;
        // } else {
        //   fightCommonInterruptLengths.at(0)--; ///decrement interrupt.
        //   //when interrupt ends:
        //   if (fightCommonInterruptLengths.at(0) <= 0){
        //     fightCommonInterruptLengths.erase(fightCommonInterruptLengths.begin());
        //     // filterAmt = 7;
        //     comment = "s7";
        //     //restore default case
        //   }
        // }
        //for each frame processed as a 7 or 8 or 9, exhaust one from current segment.


        //This block sets the interrupt pattern.
        // if (fightCommonMainLengths.at(0) <= 0){
        //   //handle interrupt.
        //   if (fightCommonInterruptTypes.at(0) == 0){
        //      filterAmt = 8;
        //     //comment = "safe Eights";
        //     fightCommonInterruptTypes.push_back(fightCommonInterruptTypes.at(0));
        //     fightCommonInterruptTypes.erase(fightCommonInterruptTypes.begin());
        //   } else if (fightCommonInterruptTypes.at(0) == 1){
        //     filterAmt = 9;
        //     //comment = "safe Nines";
        //     fightCommonInterruptTypes.push_back(fightCommonInterruptTypes.at(0));
        //     fightCommonInterruptTypes.erase(fightCommonInterruptTypes.begin());
        //   } else if (fightCommonInterruptTypes.at(0) == 2){
        //     filterAmt = 0;
        //     comment = "Special Divided Set";
        //     fightCommonInterruptTypes.push_back(fightCommonInterruptTypes.at(0));
        //     fightCommonInterruptTypes.erase(fightCommonInterruptTypes.begin());
        //     //Fill this in later lol.
        //   } else {
        //     comment = "";
        //   }

        //   fightCommonMainLengths.erase(fightCommonMainLengths.begin());
          //do not renew main pattern, until I actually find a pattern lol.
          //Need better data across more frames.
        //}
      }
      if (filterMAP){
        // if (i % 60 == 16){
        //   comment = "60F Align!";
        // } else {
        //   comment = "";
        // }
        if (inputValues.at(i) == 11){
          frameLabel = "Eleven ~~~~";
        } else if (inputValues.at(i) == 12) {
          frameLabel = "Twelve";
        } else {
          frameLabel = "---- WEIRD CASE ----";
        }

        //9 pairs per group, 2-3 groups per set. Always a low pair, followed by a high pair, to line up at the 60 or 120 frame mark.
        //exact organization of pairs within a group varies, hence the existence of 3 unique sets at least. No repetetion of sets yet,
        //will need more testing eventually to confirm.

        checkChangeFrame(mapSegments,i,delayFrame);
        if (delayFrame){
          filterAmt = 12;
        } else {
          filterAmt = 11;
        } 

        //final accomodations
        if (i == unexplainedVariant){
          filterAmt = 11;
        }
        if (i < mapOffset){
          filterAmt = 12;
        }
      }
      if (filterCameraShake){
        if (inputValues.at(i) == 7){
          frameLabel = "Seven ~~~~";
        } else if (inputValues.at(i) == 8) {
          frameLabel = "Eight";
        } else {
          frameLabel = "---- WEIRD CASE ----";
        }
      }
      if (filterCamera){
        if (inputValues.at(i) == 8){
          frameLabel = "Eight ~~~~";
        } else if (inputValues.at(i) == 9) {
          frameLabel = "Nine";
        } else {
          frameLabel = "---- WEIRD CASE ----";
        }

        // if (i % 60 == 46){ 
        //   comment = "60F align!";
        //   comment += " Begin Eights!";
        // } else if (i % 60 == 17){
        //   comment = "End Eights!";
        // } else {
        //   comment = "";
        // }
        if (i % 60 == 0){
          comment = "60F align!";
        } else if (i % 60 > 0 && i % 60 < 27){
          comment = "safe 9";
        } else if (i % 60 == 30){
          comment = "Eights Align!";
        } else if (i % 60 > 30 && i % 60 < 57){
          comment = "safe 8";
        } else {
          comment = "----- Variance!";
          immediateVarSeg.push_back(inputValues.at(i));
          if (immediateVarSeg.size() == 3){
            variances.push_back(immediateVarSeg);
            immediateVarSeg.clear();
          } 
        }
        // if (i % 60 == 27 || i % 60 == 57){
        //   comment = "~ ~ ~ ~ Align!";
        // }
        // if (i % 63 == 31 || i % 63 == 58){
        //   comment = "Safe Eights!";
        // } else if (i % 63 == 0 || i % 63 == 26) {
        //   comment = "Safe Nines!";
        // } else if (i % 63 > 26 && i % 63 < 31 || i % 63 > 58){
        //     comment = "Variance Frame";
        // } else {
        //   comment = "~~";
        // }
      }
      if (filterDeckArchive){ //replace with switch case lol.
        if (inputValues.at(i) == 6){
          frameLabel = "Six";
        } else if (inputValues.at(i) == 5) {
          frameLabel = "Five ~~~~";
        } else {
          frameLabel = "---- WEIRD CASE ----";
        }

        // if (i % 60 == 0){
        //   comment = "60F align!";
        // } else {
        //   comment = "";
        // }
        // else
         
        // if (i % 60 < 27){
        //   comment += "safe 5";
        // // } else if (i % 60 == 29){
        // //   comment = "Sixes Align!";
        // } else if (i % 60 >= 27 && i % 60 < 59){
        //   comment += "safe 6";
        //  } else {
        //    comment += "----- Variance!";
        // comment = "";
        // //   immediateVarSeg.push_back(inputValues.at(i));
        // //   if (immediateVarSeg.size() == 3){
        // //     variances.push_back(immediateVarSeg);
        // //     immediateVarSeg.clear();
        // //   } 
        //}
        
      }


    

    frames -= filterAmt;

    if (frames == 0){
      frameLabel = "";
    }
    outputData << left << setw(10)   << frameLabel << " - "<< frames << " - " << comment << endl;
    }

    outputData << "All segment lengths:\n";
    for (int i = 0; i<numPossibleValues;i++){
      outputData << "Segment " << i << ": "; 
      for (int j = 0; j < segmentLengths.size();j++){
        if (j % numPossibleValues == i){
          outputData << segmentLengths.at(j) << ", ";
        }
      }
      outputData << endl;
    }
    outputData << endl;
    // for (unsigned int i = 0; i < segmentLengths.size();i++){
      
    //   // if (i % numPossibleValues == 0){
    //   //   outputData << segmentLengths.at(i) <<", ";
    //   // }
    // }
  // outputData << "Full sequence:\n";
  // for (unsigned int i = 0; i < segmentLengths.size();i++){
  //       outputData << segmentLengths.at(i) << ", ";
  //       if (i % 3 == 1 && i > 1){
  //         outputData << endl;
  //       }
  // }
  outputData << "Lows:\n";

  int line = 0;
  for (unsigned int i = 0; i < segmentLengths.size(); i++)
  {
    if (segmentLengths.at(i) < 11){
      outputData << segmentLengths.at(i);
      
      if (i < segmentLengths.size()-1){
        outputData << ", ";
      }
      if (line == 5){
        outputData << endl;
        line = 0;
      }
      line++;
    }
  }
  outputData << "\nHighs:\n";
  line = 1;
  segmentLengths.erase(segmentLengths.begin());
  for (unsigned int i = 0; i < segmentLengths.size(); i++)
  {
    if (segmentLengths.at(i) > 11){
        outputData << segmentLengths.at(i);   
      if (i < segmentLengths.size()-1){
        outputData << ", ";
      }
      if (line == 5){
        outputData << endl;
        line = 0;
      }
      line++;
    }
  }
  outputData << "\nMixed:\n";
  line = 1;
  segmentLengths.erase(segmentLengths.begin());
  for (unsigned int i = 0; i < segmentLengths.size(); i++)
  {
        outputData << segmentLengths.at(i);   
      if (i < segmentLengths.size()-1){
        outputData << ", ";
      }
      if (line == 7){
        outputData << endl;
        line = 0;
      }
      line++;
  }
  outputData << "\n\nPairs:\n";
  
  for (unsigned int i = 0; i < segmentLengths.size(); i++)
  {
    int sum = 0;
    if (i < segmentLengths.size()-1){
      if (segmentLengths.at(i) + segmentLengths.at(i+1) == 10 || segmentLengths.at(i) + segmentLengths.at(i+1) == 9){
        segmentLengths.insert(segmentLengths.begin()+i,segmentLengths.at(i)+segmentLengths.at(i+1));
        segmentLengths.erase(segmentLengths.begin()+i+1,segmentLengths.begin()+i+3);
        //should collapse split segments into their correct pairs.
      }
      outputData << segmentLengths.at(i);   
      if (i < segmentLengths.size()-1){
        outputData << ", ";
      }
     if (i % 2 == 1){
       sum = segmentLengths.at(i) + segmentLengths.at(i-1);
       outputData << " : " << sum;
       outputData << endl;
     }

    }
  }
  
  // outputData << "\nEdited:\n";
  // for (unsigned int i = 0; i < segmentLengths.size();i++){
  //   if (segmentLengths.at(i) > 11){
  //      outputData << segmentLengths.at(i) << ", ";
  //       if (i % 11 == 10){
  //         outputData << endl;
  //       }
  //   }
  // }

  outputData << "\nValues:\n";
  
  for (unsigned int i = 0; i < segVals.size(); i++)
  {
        outputData << segVals.at(i);   
      if (i < segVals.size()-1){
        outputData << ", ";
      }
     if (i % 11 == 10){
       outputData << endl;
     }
  }

       
        // if (i % numPossibleValues == 0){
        //   outputData << segmentLengths.at(i) <<", ";

    // outputData << "All segment lengths: ";
    // for (unsigned int i = 0; i < segmentLengths.size();i++){
    //     outputData << segmentLengths.at(i) <<", ";
    // }
    // outputData << endl;




    return 0;
}

/*
Full sequence: MAP PATTERN:
16, 

Set 1: (1500-1 frames long, 50 seconds.)
27, 33,  27, 33,  27,(33), 

29, 31,  29, 31,  29, 31, Row A, 29/31s intended.
29, 31,  29, 30*  29, 31,

29, 30,  29, 30,  29, 30, Row B 29/30/30/31s intended.
30, 31,  30, 31,  30, 31,  

29, 31,  29, 31,  29, 31, Row C, == 2x Row A.
29, 31,  29, 31,  29, 31, 
29, 31,  29, 31,  0
29, 31,  29, 31,  
0        -1
 
Set 2: (1620-2 frames long, 54seconds.)
28, 32,  28, 32,  27, 33, Header - 28s except 3rd one.

29, 31,  29, 31,  29, 31, Row A matches
29, 31,  29, 31,  28* 31, 

29, 30*  29, 31,  29, 31, Row C first half
29, 31,  29, 31,  29, 31,

29, 30,  29, 30,  29, 30, Row B
30, 31,  30, 31,  30, 31, 

29, 31,  29, 31,  29, 31, Row C second half.
29, 31,  29, 31,  29, 31, 
-1       0        -1

-- very similar to patterns above.


Set 3:
27, 33,  27, 32*, header

29, 31,  29, 31, Row A
29, 31,  29, 31,

29, 31,  29, 31, Row B == A
29, 31,  29, 31,

29, 31,  29, 31, Row C == 2x Row A 
29, 31,  29, 31,
29, 31,  29, 31,
29, 31,  29, 31, 
0        -1

Set 4: (similar to set 1?)
27, 33, 27, 32*, Header

29, 31, 29,     Row A
29, 31, -1

29, 30,         Row B
30, 31, 

29, 31,         Row C -- Missing a set?
29, 31, 
29, 31, 
0

Every 2nd Pattern change loses 1 frame on the 60f align,
usually at the 27/33 header but not limited to that.
implies 120frame -1 = 119f align? 4 seconds at 30fps, but since
third set ends early, forces readjustment.

THIRD PATTERN:
Full sequence:
31, 28, 
32, 28, 
 
30, 30,  30, 30,  30, 30,  30, 30, 30,(30)

32, 28,  32, 28,  31, 28,  32, 28, 32, 28, 
31, 28,  32, 28,  32, 28,  32, 28, 32, 28, 

32, 28,  32, 28,  32, 27,  32, 27, 32, 27,
32, 28,  32, 28,  33, 28,  33, 28, 33, 28,

32, 27,  32, 28,  32, 28,  32, 28, 32, 28, 
33, 28,  32, 28,  32, 28,  32, 28, 32, 28, 

32, 28,  32, 28,  32, 28,  32, 28, 32, 28,  
32, 28,  32, 28,  32, 28,  32, 27, 32, 28, 
-1       0        -1       0       0??
                                    --missing??? where 3030?
                                    --missing??? where 3228?
                                    32, 28, 
                                    32, 28, 
                                    32, 28, 
                                    32, 27, 
                                    33, 28, 
                                    32, 28, 
                                    32, 28, 
                                    0??

                                    30, 29, --weird 3030?
                                    32, 28, 
                                    32, 28, 
                                    32, 28, 
                                    32, 28, 
                                    32, 27, 
                                    33, 28, 
                                    32, 28, 
                                    32, 28,
                                    -1??
30, 30,  30, 30, 

32, 28,  32, 28,
32, 28,  32, 28, 

32, 28,  32, 28, 
32, 28,  32, 27, 
32, 27,  33, 28,
33, 28,  32, 28,

32, 28,  32, 28, 
31, 28,  0 ===
-1

B4 CAM PATTERN:
Full sequence:
24, 27, 
33, 27, 
33, 27, 
33, 27, 

32, 28,
32, 28, 
32, 28, 
32, 

26, 34, 

28, 32, 
28, 32, 
28, 32, 
28, 32, 
28, 32, 
28, 32, 
28, 32, 

27, 33, 

25, 35,

27, 33,
27, 33, 
27,    -- lose 1?

32, 28,
32, 28, 
32, 28, 
32, 28, 
32, 28, 
32, 

26, 34, 

28, 32, 
28, 32, 
28, 

31, 29,

32, 28, 
32, 28, 
32,

27, 33,
27, 33, 

25, 35,

27, 33, 
27, -- lose 1?

32, 28,

31, 29,

32, 28, 
32, 28, 
32, 28, 
32, 28, 
32,

26, 34, 

28, 32, 
28, 32,
28, 

30, 30,

32,     ???

27, 33, 
27, 33, 
27, 33, 
27, 33, 

25, 35,

27,     ??? --lose 1?

32, 28, 
32, 28, 

31, 29,

32, 28, 
32, 28, 
32, 28, 
32, 28, 
32, 28, 
32, 28, 
32, 28, 
32, 28, 

30, 29, yikes. * = Lose 1.

33, 27, 
33, 27, 
33, 27, 
33, 27, 
33, 27, 

32, 28,
32, 28, 
32, 28, 

31, 29,

32, 28, 
32, 28, 
32, 28, 
32, 

26, 34, 

28, ==


Lows:
10, 9, 9, 10,
3, 7, N/E
10, 9, 9, 10, 
4, 6, N/E
9, 9, 9, 10,
7, 3, N/E
9,
1, 8, E/N
9, 10, 
7, 3, N/E
9, 
2, 7, E/N
9, 10, 
9, 1, N/E
9,
3, 6, E/N
9, 10, 10, 9,
5, 4, E/N
9, 10, 10, 9, 
5, 4, E/N
10, 10, 10, 9,
8, 1, E/N
10, 10, 
9, 1, N/E

Lows:
10, 9, 9, 10, (10),
10, 9, 9, 10, (10),

9, 9, 9, 10, (10),

9, (9), 9, 10, (10),
9, (9), 9, 10, (10),

9, (9), 9, 10, 10,
9, (9), 9, 10, 10,

9, (9), 10, 10, 10,
9, (9), 10, 10, (10), 

Mixed:

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


	  0    1	2	 3	  4   5	  6	  7	   8
1	  97,  97,	98,	 99,  99, 99, 98, 99,  99,
2	  10,  10,	 9,	  9,   9,  9,  9,  9,	9,
3	  99,  99,	99,	 99,  98, 98, 99, 99,  98,
4	   9,   9,	 9,	(9), (9),(9),(9),(9), (9),
5	  98,  99,	98,	 99,  99, 99, 99, 97,  98,
6	   9,   9,	 9,	  9,   9,  9,  9, 10,  10,
7	  98,  97,	98,	 97,  97, 97, 97, 98,  45,
8	  10,  10,	10,	 10,  10, 10, 10, 10,  10,
9	  98,  98,  97,	 98,  98, 98, 98, 97,  96,
10	(10),(10),(10),(10),(10), 10, 10, 10,(10),

Values:
7, 9, 7, 9, 7, 8, 7, 8, 7, 9, 8, 
7, 9, 7, 9, 7, 8, 7, 8, 7, 9, 8, 
7, 9, 7, 9, 7, 8, 7, 8, 7, 9, 8,

7, 9, 7, 8, 9, 7, 8, 7, 8, 7, 9, 8,
7, 9, 7, 8, 9, 7, 8, 7, 8, 7, 9, 8,

7, 9, 7, 8, 9, 7, 8, 7, 8, 7, 9,
7, 9, 7, 8, 9, 7, 8, 7, 8, 7, 9,
7, 9, 7, 8, 9, 7, 8, 7, 8, 7, 9,

7, 9, 7, 8, 9, 7, 8, 7, 8, 7, 9, 8,

3, 7,
4, 6,
7, 3,
1, 8,
7, 3,
2, 7,
9, 1,
3, 6,
5, 4,
8, 1,
9, 1,

3, 7, 4, 6, 7, 3, 1, 8, 7, 3, 2, 7, 9, 1, 3, 6, 5, 4, 8, 1, 9, 1,


37,   10, 
99,    9, 8
98,    9, 7
98,   10, 8
98, (10), 8
97,   10, 7
99,    9, 8
99,    9, 8
97,   10, 7
98, (10), 8
98,    9, 7
99,    9, 8
98,    9, 7
98,   10, 
97, (10), 

99,    9,
99,  (9),
99,    9,
97,   10,
98, (10),
99,    9, 
98,  (9), 
99,    9, 
97,   10, 
98, (10),

99,    9, 
98,  (9), 
99,    9, 
97,   10, 
98,   10,
98,    9, 
99,  (9), 
99,    9, 
97,   10,
98,   10, 
99,    9, 
99,  (9), 
97,   10, 
98,   10, 
97,   10,

99,    9,
98,  (9), 
98,   10, 
45,   10, 
96, (10),
73


97 always followed by 10.


97 is never followed by a 9.
Column 3 must always be either 99 or 98.

99 is never followed by a 10.
8th column is always natural 10s.
Therefore column 7 and 9 is either 98 or 97.

Last column is always Worth 10.
2nd column is always worth 9.

The sum of a pair is either 107 or 108.
108's may appear next to each other but 107 will never.
*/