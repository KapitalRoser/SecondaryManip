#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
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
  if (binary_search(changeFrames.begin(),changeFrames.end(),currentFrame)){ //If current frame is special, adjust alignment.
    if (toDecrement){
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

int main(){
  
  //delegate this to func() just before it is called in the mystObject alignment.
  vector<int>mysteryObjCFEights;
  vector<int>mysteryObjCFAs;
  vector<int>mysteryObjCFSevens {552,649,1151,1248,1750}; //97/502
  vector<int>mysteryObj;
  //saves me mindless typing + helps if I ever need to change the default sevens values.
  for (int i = 0; i < mysteryObjCFSevens.size();i++){
    mysteryObjCFEights.push_back(mysteryObjCFSevens.at(i)+1);
    mysteryObjCFAs.push_back(mysteryObjCFSevens.at(i)+5);
  }
   //for eights just +1 to all values.
   //woah, some alignment onto the changevalues mod 3 as well.
  vector<int>midFountCF {122,246,373,497,621,748,872,996,1123,1247,1371,1498,1622,1746,1873,1997}; //124 or 127 between changes.
  //{131,255,379,506,630,754,881,1005,1129,1256,1380,1504,1631,1755,1879}
  vector<int>northFountCF {248,499,750,998,1249,1500,1748,1999};
  //{258,508,757,1008,1258,1507,1758}
  /*
  122-121
  245-246 == 248-249
  373-372 == 499-500
          == 750-751
          == 998-999
          == 1249-1250
          == 1500-1501
          == 1748-1749
          == 1999-2000

  260-261
  508-509
  759-760
  1010-1011
  1258-1259
  1509-1510
  1760-1761
  */
  // vector<int>northFountCF ;
    const string FILE_EXTENSION = ".txt";
    string FILE_NAME = "";
    cout << "Enter filename: ";
    getline(cin, FILE_NAME);


    uint32_t seed = 0x0;
    int rolls = 0;

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

    //Aligning the two mystery objects, possibly camera related.
    string leadingPattern = "Sevens";

    int backAlignment = 0;
    int northFountAlign = 1;
    int midFountAlign = 2; //doesn't change with pattern
      if (leadingPattern == "Sevens"){
        mysteryObj = mysteryObjCFSevens;
        backAlignment = 2;
        } else if(leadingPattern == "As"){
          mysteryObj = mysteryObjCFAs;
          backAlignment = 1;
        } else {
          mysteryObj = mysteryObjCFEights;
        }
      // else if (leadingPattern == "Eights"){
      //   backAlignment = 0;
      //   }

    int anomalies = 0;
    bool npcsON = 0;
    int region = 0; //0 NTSC, 1 EU, 2 NSTCJ
    string frameType = "";
    vector<int>stepFrames;
    //File IN
    vector<uint32_t> rawPattern = readNumbersFromFile(FILE_NAME + FILE_EXTENSION);
    //File OUT
    ofstream refinedPattern("out" + FILE_NAME + FILE_EXTENSION); 

    if (rawPattern.empty()){
        cout << "File read unsuccessful";
    } else {
        cout << "File read successfully! ";
    }
    cout << "Values read: " << rawPattern.size() << endl;
    for (uint32_t i = 0; i<rawPattern.size()-1;i++){
        seed = rawPattern.at(i); // Compares a given seed to the next seed in the list. 
        refinedPattern << setw(8) << hex << seed << dec; //track seed
        rolls = 0;
        while(seed != rawPattern.at(i+1)){
            LCG(seed);
            rolls++;
        }

        //Filtering on rolls pre-classification goes here:
        
        //For output document only
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
        // } else if (find(stepValues.begin(), stepValues.end(), rolls) != stepValues.end() && i < 730 && i > 630){
        //     frameType = "---- STEP";
        //     stepFrames.push_back(i);
            //some SEVENS may appears as BAs. 
        } else {
            frameType = "OTHER -------"; //add classification for player step frames.
            if (!(npcsON)){
              anomalies++;
            }
        }
         

        /*experimental.


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
      */

      //FULL MODEL
      //500F Rule
      if (i % 500 == (503-10)){
        rolls -= 11;
      }
      //South
      rolls -= 23; //produces a couple extra anomaly frames. Same on sevens

      //   //THIS ASSUMES LUA SCRIPT FOR PERFECT INPUTS.
      //NORTH: -- condense this into int alignPattern(int rolls, vector<int>pattern,vector<int>changeFrames ) function?
      rolls -= 11;
      northFountAlign = alignTrio(northFountCF,i,northFountAlign,true);
      if (i % 3 == northFountAlign){
        rolls -= 10;
      }

        //MIDDLE: VARIES W PATTERN
        rolls -= 23;
      midFountAlign = alignTrio(midFountCF,i,midFountAlign,false);
      if (i % 3 != midFountAlign){
        rolls -= 11;
      }




      //Does leading as even exist in lua core. yes but rare?

        //MYSTERY OBJECT PAIR:
        backAlignment = alignTrio(mysteryObj,i,backAlignment,false);
        if (i % 3 == backAlignment){
            rolls -= 20;
            // frameType = "TWENTY";
          } else {
            rolls -= 11;
            // frameType = "ONES";
          }
        

        //redesign this eventually. Disable this for original pattern.
        if (rolls == 0){ 
          frameType = " ";
          anomalies --;
          } else if (rolls == 9){
            frameType = "POSITIVE";
          } else if (rolls == -9){
            frameType = "NEGATIVE";
          }
        

        //write results
        refinedPattern << " : " << setw(13) << frameType << " : " << setw(3) <<  rolls;
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
        if (   i == 584 || i == 589 || i == 596 
            || i == 603 || i == 613 || i == 618
            || i == 622 || i == 628 || i == 637
            || i == 645 || i == 657 || i == 675
            || i == 695 ){
              refinedPattern << " `` STEP FRAME ``";
            }
     refinedPattern << endl;

    }
    refinedPattern << "Anomalies: " << anomalies << ".\n";
    refinedPattern << "Step frames: ";
    for (unsigned int i = 0; i<stepFrames.size();i++){
      refinedPattern << stepFrames.at(i) << ",";
    }
    refinedPattern << ".\n";
    cout << "Output file produced! All done!";
    refinedPattern.close();
    
    
    
    /*
    Noticeable patterns:

    ABC
    ACB
    SEVENS B OFF
    SEVENS OFF B
    EIGHTS C OFF
    EIGHTS OFF C



    Using the pause menu right beside the npc then getting the dialogue box is hella stable. 
    The pattern of ca cb cc does switch up internally a little bit, but the overall pattern is easily reproduced.

    The fluctuation of patterns is somewhat caused by external factors not entirely related to rng value, 
    since injecting 0 to get recurring possible values does not prevent pattern change.
    */
    
    
    return 0;
    }
