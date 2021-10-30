#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
//XD TOOL NOT COLO
#include "../processCore.h"
using namespace std;



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
    string FILE_NAME = "";
    cout << "Enter filename: ";
    getline(cin, FILE_NAME);
    int rolls = 0;
    uint32_t seed = 0x0;

    //Colo-Specific identifiers
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
