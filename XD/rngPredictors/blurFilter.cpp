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
int main(){
    const string FILE_EXTENSION = ".txt";
    string FILE_NAME = "";
    cout << "Enter filename: ";
    getline(cin, FILE_NAME);
    vector<int>inputValues = decimalReadNumbersFromFile(FILE_NAME + FILE_EXTENSION);
    cout << inputValues.size() << endl;
    ofstream outputData ("out" + FILE_NAME + FILE_EXTENSION);
    vector<int>segmentLengths;
    vector<vector<int>>variances;
    vector<int> immediateVarSeg;
    int segment = 0;
    int current = 0;

    bool filterApress = false;
    bool filterDuration = false;
    bool filterCamera = true;


    string frameLabel = "";
    string comment = "";
    //Will need to reverse this to figure out where the pattern starts. 
    int positionOffset = 18;
    int nineteenLength = 30;

    int filterAmt = 0; //OBVIOUSLY CHANGE THIS AS NEEDED.

    int frames = 0;
    int varSegCount = 0;


    for (unsigned int i = 0; i < inputValues.size();i++){

      //segment tracking - works for binary values like 19/20 or 8/9.
      frames = inputValues.at(i);
      if (i == 0){
        current = frames;
      }
      if (current == inputValues.at(i)){
        segment++;
      } else {
        current = inputValues.at(i);
        segmentLengths.push_back(segment);
        segment = 0;
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
      
      if (filterDuration){ //replace with switch case lol.
        if (inputValues.at(i) == 46){
          frameLabel = "Six";
        } else if (inputValues.at(i) == 47) {
          frameLabel = "Seven";
          } else if (inputValues.at(i) == 48) {
          frameLabel = "Eight";
          } else if (inputValues.at(i) == 49) {
          frameLabel = "Nine";
          } else if (inputValues.at(i) == 50) {
          frameLabel = "Fifty ---- ";
        } else {
          frameLabel = "---- WEIRD CASE ----";
        }

        if (i % 60 == 32){ 
          comment = "60F align!";
        } else {
          comment = "";
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
      


    

    frames -= filterAmt;

    if (frames == 0){
      frameLabel = "";
    }
    outputData << left << setw(10)   << frameLabel << " - "<< frames << " - " << comment << endl;
    }

    outputData << "All segment lengths: \nEights: ";
    for (unsigned int i = 0; i < segmentLengths.size();i++){
      if (i % 2 == 0){
        outputData << segmentLengths.at(i) <<", ";
      }
    }


    outputData << "\nNines: ";
    for (unsigned int i = 0; i < segmentLengths.size();i++){
      if (i % 2 == 1){
        outputData << segmentLengths.at(i) <<", ";
      }
    }
    outputData << endl
    << "Variance patterns: \n";
    cout << variances.size();
    for (unsigned int i = 0; i < variances.size();i++){
      for (unsigned int j = 0; j < 3;j++){
        outputData << variances.at(i).at(j);
        if (j < 2){
          outputData << ",";
        }
     }
      outputData << ".\n";
    }

    // outputData << "All segment lengths: ";
    // for (unsigned int i = 0; i < segmentLengths.size();i++){
    //     outputData << segmentLengths.at(i) <<", ";
    // }
    // outputData << endl;




    return 0;
}