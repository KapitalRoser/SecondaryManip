#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
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
int main(){
    const string FILE_EXTENSION = ".txt";
    const string FILE_NAME = "p8";


    uint32_t seed = 0x0;
    int rolls = 0;

    int AFrame = 98;
    int BFrame = 79;
    int CAFrame = 68;

    int CBFrame = 78; //Occasionally sprinkled in, doesn't seem intentional
    int CCFrame = 57; //Only seen during initial loading in. 

    int offFrame = 89;

    int rare250Frame = 67; //occur with pause method only.

    int dbl7 = 77;
    int dbl8 = 88;

    int xA = 31;
    int xB = 46;
    int xC = 22;


    int xAlpha = 20;
    int xBeta = 11;

    int BLUR = 9600;
  
    int anomalies = 0;
    bool npcsON = 0;
    int region = 0; //0 NTSC, 1 EU, 2 JPN
    string frameType = "";
    vector<int>stepFrames{};
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
    cout << rawPattern.size() << endl;
    // int prunedValues = 0;
    // for (uint32_t i = 0; i<rawPattern.size()-8;i++){
    //     while(rawPattern.at(i) == rawPattern.at(i+1)){
    //       rawPattern.erase(rawPattern.begin()+i+1);
    //       prunedValues++;
    //     }
    //   }
    // cout << "Completed Pruning! " << dec << prunedValues << " values removed!\n";
    // cout << rawPattern.size() << endl;

    for (uint32_t i = 0; i<rawPattern.size()-1;i++){
        seed = rawPattern.at(i); // Compares a given seed to the next seed in the list. 
        // refinedPattern << setw(8) << hex << seed << dec; //track seed
        refinedPattern << i+1 << ". ";
        rolls = 0;
        while(seed != rawPattern.at(i+1)){
            LCG(seed);
            rolls++;
        }
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
        } else if (rolls == xA){
            frameType = "xA";
        } else if (rolls == xB){
            frameType = "xB";
        } else if (rolls == xC){
            frameType = "xC";
         } else if (rolls == xAlpha){
            frameType = "xAlpha";
          } else if (rolls == xBeta){
            frameType = "xBeta";
        } else if (rolls == rare250Frame){
            frameType = "250 !!";
        } else if (rolls == dbl7){
          frameType = "SEVENS";
        } else if (rolls == dbl8){
          frameType = "EIGHTS";
        } else if (rolls == BLUR){
          frameType = "BLUR FRAME";
        // } else if ( rolls == AFrame + 2 || 
        //             rolls == BFrame + 2 ||
        //             rolls == CFrame + 2 ||
        //             rolls == DFrame + 2 ){
        //     frameType = "STEP";
        //     stepFrames.push_back(i);
        } else {
            frameType = "OTHER -------"; //add classification for player step frames.
            if (!(npcsON)){
              anomalies++;
            }
        }
        // if (   i == 585 || i == 590 || i == 597 
        //     || i == 604 || i == 614 || i == 619
        //     || i == 623 || i == 629 || i == 638
        //     || i == 646 || i == 658 || i == 676
        //     || i == 696 ){
        //       refinedPattern << " `` STEP FRAME ``";
        //     }

        //write results
         
        refinedPattern<< setw(13) << frameType << " : " << setw(3) <<  rolls;
        //add controller inputs.
        // if ( i == 0 || i == 1 || i == 2){
        //     refinedPattern << "   ~~~~~ A button press ~~~~~";
        // }
     refinedPattern << endl;

    } 
    
    
    refinedPattern << "Anomalies: " << anomalies << ".\n";
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
    */
    
    
    return 0;
    }
