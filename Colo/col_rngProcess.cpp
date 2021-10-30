#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
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
    const string FILE_NAME = "_col";
    uint32_t seed = 0x0;
    int rolls = 0;

    int hiFrame = 116;
    int loFrame = 76;
    int yFrame = 154; //PAL Only
    int xFrame = 114;
    
    vector<int> stepValues = {};

    int anomalies = 0;
    bool npcsON = 0;
    int region = 0; //0 NTSC, 1 EU, 2 JPN
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
        //For output document only
if (rolls == hiFrame){
            frameType = "HIGH";
        } else if (rolls == loFrame){
            frameType = "LOW";
        } else if (rolls == xFrame){
            frameType = "X";
            if (region != 1){
              anomalies++;
            }
        } else if (rolls == yFrame){
            frameType = "Y";
            if (region != 1){
              anomalies++;
            }
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
        // if (   i == 585 || i == 590 || i == 597 
        //     || i == 604 || i == 614 || i == 619
        //     || i == 623 || i == 629 || i == 638
        //     || i == 646 || i == 658 || i == 676
        //     || i == 696 ){
        //       refinedPattern << " `` STEP FRAME ``";
        //     }
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
