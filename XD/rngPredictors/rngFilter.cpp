#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <numeric>
//XD TOOL NOT COLO
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

vector<uint32_t> readNumbersFromFile(string fileName)
{
    uint32_t value;
    string lineRead = "";
    stringstream hexConvert;
    vector<uint32_t> data; //Setup
    ifstream file(fileName);
            cout << "Read some data! \n";
    if (file.fail())
    {
        cout << "File inaccessible";
        exit(EXIT_FAILURE);
    }
    while (!(file.fail()))
    {                 
        getline(file,lineRead); 
        hexConvert << hex << lineRead; //cuz i just HAD to have my docs in hexa and not int...;
        hexConvert >> value;
        hexConvert.clear();
        hexConvert.str("");
        data.push_back(value);
    }
    file.close();
    return data;
}

int alignTrio (vector<int>changeFrames, int currentFrame, int oldAlign, bool toDecrement){
  int alignment = oldAlign;
  if (binary_search(changeFrames.begin(),changeFrames.end(),currentFrame)){ 
    //If current frame is special, adjust alignment by returning which i % 3 value gets unique treatment.
    if (toDecrement){ //reorganize original pattern so that there is no need to decrement? Would simplify things.
      alignment--;
      alignment = alignment % 3;     
    } else {
      alignment++;
    }
    //Over/underflow correction:
    if (alignment >=3){ //has to be this way. Would need euclidean mod implementation to handle negative values.
      alignment = 0;
    } else if (alignment < 0){
      alignment = 2;
    }
  }
  return alignment;
}
string getFrameType(int rolls){
    int AFrame = 98; //78
    int BFrame = 79; //59
    int CAFrame = 68; //48

    int CBFrame = 78; //Occasionally sprinkled in, doesn't seem intentional
    int CCFrame = 57; //Only seen during initial loading in. 

    int offFrame = 89;

    int rare250Frame = 67; //occur with pause method only.

    int dbl7 = 77;
    int dbl8 = 88;

    int back1 = 11;
    int back2 = 20;

    string frameType = "";
    if (rolls == AFrame){
            frameType = "AA";
        } else if (rolls == BFrame){
            frameType = "BA";
        } else if (rolls == CAFrame){
            frameType = "CA";
        } else if (rolls == CBFrame){
            frameType = "CB";
        } else if (rolls == CCFrame){
            frameType = "CC";
        } else if (rolls == offFrame){
            frameType = "OFF";
        } else if (rolls == rare250Frame){
            frameType = "250 !!";
        } else if (rolls == dbl7){
          frameType = "SEVENS";
        } else if (rolls == dbl8){
          frameType = "EIGHTS";
          } else if (rolls == back1){
          frameType = "ONES";
          } else if (rolls == back2){
          frameType = "TWENTY";
          } else {
            frameType = "OTHER -------";
          }
          return frameType;
       
}

vector<int> updateCFV(vector<int>vec, int operand){
  vector<int>updatedResult;
  for (int i = 0;i<vec.size();i++){
      updatedResult.push_back(vec.at(i)+operand); //adds operand to the param vector and copies into result.
      //could add other functionality
    }
    return updatedResult;
}
vector<int> modifyChangeFrameVector(vector<int> cfSource,string pattern,int variableOperand){
  //pattern only matters for backAlign.
  //Still operates on a recorded frame basis for changing pattern, rather than building source from interval.
  vector<int>cfActive;
  if (pattern == "Sevens"){
    cfActive = cfSource;
  } else if (pattern == "Eights"){
    cfActive = updateCFV(cfSource,variableOperand); //default 1?
  } else if (pattern == "As"){
    cfActive = updateCFV(cfSource,variableOperand); // 2,5??
  }
  return cfActive;
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
  //in this case, alignment is really just an anchor from which to begin the pattern cycle, rather than the literal position of a off value in the pattern
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

int main(){   
    uint32_t seed = 0x0;
    string leadingPattern = "Eights";
    int backOffset = 1; //1,2,5 DOES NOT VARY W RNG, ONLY STATE AND PATTERN. 1 for eights, 5 for As, 0 for sevens.
    int lagOffset = 1; //any connection to the changes in initial offset for the mid and north fountains? negative 2 default, varies with rng seed at start.
    const int REGION = 0; //0 NTSC, 1 EU, 2 JPN
    
    
    int rolls = 0;
    int anomalies = 0;
    string frameType = "";
    vector<int>fiveHundoRule;


    //File IN/OUT
    const string FILE_EXTENSION = ".txt";
    string FILE_NAME = "";
    cout << "Enter filename: ";
    getline(cin, FILE_NAME);
    vector<uint32_t> rawPattern = readNumbersFromFile(FILE_NAME + FILE_EXTENSION); //loads all seeds from file into RAM.
    ofstream refinedPattern("out" + FILE_NAME + FILE_EXTENSION); 
    //Debug
    if (rawPattern.empty()){ 
        cout << "File read unsuccessful";
    } else {
        cout << "File read successfully! ";
    }
    cout << "Values read: " << rawPattern.size() << endl;



    for (int i = 0; i<(rawPattern.size()/500) + 1; i++){
      fiveHundoRule.push_back(500*i+lagOffset);
    }


    //Align high level patterns.
    vector<int>mysteryObjSource {552,649,1151,1248,1750}; //97/502 -- full pattern? Not enough data to extrapolate. -- frame 466 & 565 errors are consistent across instance of pattern. Inverted from 7s to 8s, so more investigation needed there.
    vector<int>mysteryObj = modifyChangeFrameVector(mysteryObjSource,leadingPattern,backOffset);

    
    vector<int>midFountCF = buildChangeFrameVector("Mid",rawPattern.size());
  // vector<int>midFountCF {131,255,379,506,630,754,881,1005,1129,1256,1380,1504,1631,1755,1879}; //124 or 127 between changes.
    for (int i = 0; i<midFountCF.size();i++){
      cout << midFountCF.at(i) << ", ";
    }
    cout << endl;

    vector<int>northFountCF = buildChangeFrameVector("North",rawPattern.size());
    // vector<int>northFountCF {258,508,757,1008,1258,1507,1758}; //doesn't change? changes? 249/251 pattern?
    for (int i = 0; i<northFountCF.size();i++){
      cout << northFountCF.at(i) << ", ";
    }
    cout << endl;
    //Align and set low level alignments.
    int backAlignment = 0; //Eights, default
    int northFountAlign = 1;
    int midFountAlign = 2; //doesn't change with pattern
      if (leadingPattern == "Sevens"){
        backAlignment = 2;
        } else if(leadingPattern == "As"){
          backAlignment = 1;
        }
 

    //Now the fun begins.
    for (uint32_t i = 0; i<rawPattern.size()-1;i++){
        seed = rawPattern.at(i); // Compares a given seed to the next seed in the list. 
        refinedPattern << setw(8) << hex << seed << dec; //track seed
        rolls = 0;
        while(seed != rawPattern.at(i+1)){ //Seed interval crunch.
            LCG(seed);
            rolls++;
        }

      //Identify Block
      frameType = getFrameType(rolls);
      if (frameType == "OTHER -------"){
        anomalies++;
      }
         

     
      //FULL MODEL

      //South
      rolls -= 23; //produces a couple extra anomaly frames. Same on sevens
      //500F rule occurs once this fountain is introduced, so will include here. 
      if (binary_search(fiveHundoRule.begin(),fiveHundoRule.end(),i)){
        rolls -= 11;
      }
      //may need to rework for performance reasons? Is binary search slow?



      //THIS ASSUMES LUA SCRIPT FOR PERFECT INPUTS.
      //NORTH: -- condense this into int alignPattern(int rolls, vector<int>pattern,vector<int>changeFrames ) function?
      rolls -= 11;
      northFountAlign = alignTrio(northFountCF,i,northFountAlign,true);
      if (i % 3 == northFountAlign){
        rolls -= 10;
      }
        //MIDDLE: VARIES W PATTERN BUT NOT RNG (and not with state either??? not fully tested.)
      rolls -= 23;
      midFountAlign = alignTrio(midFountCF,i,midFountAlign,false);
      if (i % 3 != midFountAlign){
        rolls -= 11;
      }

        //MYSTERY OBJECT PAIR:
      backAlignment = alignTrio(mysteryObj,i,backAlignment,false); // no shift necessary for eights. 
      if (i % 3 == backAlignment){
          rolls -= 20;
        } else {
          rolls -= 11;
        }
        
        //redesign this eventually. Disable this for original pattern.
        if (rolls == 0){ 
          frameType = " ";
          } else if (rolls == 9){
            frameType = "POSITIVE";
          } else if (rolls == -9){
            frameType = "NEGATIVE";
          }
        

        //write results
        refinedPattern << " : " << setw(13) << frameType << " : " << setw(3) <<  rolls;
        
        if (   i == 584 || i == 589 || i == 596 
            || i == 603 || i == 613 || i == 618
            || i == 622 || i == 628 || i == 637
            || i == 645 || i == 657 || i == 675
            || i == 695 ){
              refinedPattern << " `` STEP FRAME ``";
            }

            //some SEVENS may appears as BAs. 
     refinedPattern << endl;
    } //END OF MAIN LOOP.
    refinedPattern << "Anomalies: " << anomalies << ".\n";
    // 
    refinedPattern << ".\n";
    cout << "Output file produced! All done!";
    refinedPattern.close();
    
    
     
    return 0;
    }


   /*experimental.

refinedPattern << "Step frames: ";
    // for (unsigned int i = 0; i<stepFrames.size();i++){
    //   refinedPattern << stepFrames.at(i) << ",";
    // }

      // } else if (find(stepValues.begin(), stepValues.end(), rolls) != stepValues.end() && i < 730 && i > 630){
        //     frameType = "---- STEP";
        //     stepFrames.push_back(i);

    
       Sevens and eights patterns are the same, assuming backAlignment is correct.
        



        south Fountain, 23 per frame always.
        rolls -= 23;

        middle fountain, 34 - 34 - 23 -- subject to changing order?
       
        north fountain 11 - 11 - 21
               
        N+S expected: 

        34 - 34 - 44


        leading back follows 90/91 into 501/502 pattern

        common denominator ?
        rolls -= 11;

        the rest depends on loaded fountains.

    Noticeable patterns:

    Using the pause menu right beside the npc then getting the dialogue box is hella stable. 
    The pattern of ca cb cc does switch up internally a little bit, but the overall pattern is easily reproduced.

    The fluctuation of patterns is somewhat caused by external factors not entirely related to rng value, 
    since injecting 0 to get recurring possible values does not prevent pattern change.

    
  Mapping all controller inputs doesn't seem to make any difference. The only frames that do are the step frames.
    //  if ( i == 389 || i == 390 || i == 391
        //   || i == 472 || i == 473 || i == 474
        //   || i == 489 || i == 490 || i == 491
        //   || i == 518 || i == 519 || i == 520
        //   || i == 537 || i == 538 || i == 539){
        //     refinedPattern << "   ~~~~~ A button press ~~~~~";
        // } else if (i>539 && i<719){
        //     refinedPattern << " +++++++++ DPAD - DOWN +++++++++";
        // } else if (i<=630 || i>=719) {
        //     refinedPattern << "   ~ B button hold ~";
        // }
    */