#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <chrono>
using namespace std;


struct frameData{
  int visFrame; //expecting single value per frame.
  int reached;
  vector<int> outSeed; //expecting 20 of each per frame.
  vector<int> PID;
  vector<int> aToBlur;
  vector<int> blurDuration;
  vector<int> camOffset;
};

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
uint32_t LCGn(uint32_t seed, const uint32_t n)
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
float LCGPercentage(uint32_t& seed){
  float percentResult = 0;
  uint32_t hiSeed = 0;
  LCG(seed);
  hiSeed = seed >> 16;
  percentResult = static_cast<float>(hiSeed)/65536;
  return percentResult;
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
vector<uint32_t> hexReadNumbersFromFile(string fileName)
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
    auto start = chrono::high_resolution_clock::now();
    vector<uint32_t> srcBlurDuration = hexReadNumbersFromFile(FILE_NAME + FILE_EXTENSION);
    vector<vector<uint32_t>> allBlurDurations;
    vector<uint32_t>currentFrame;
    vector<uint32_t>dupes;

    vector<vector<uint32_t>> allSets;
    vector<uint32_t> seedSet;

    uint32_t seed = srcBlurDuration.at(0);
    ofstream results ("out" + FILE_NAME + FILE_EXTENSION);
    if (srcBlurDuration.empty()){
        cout << "File read unsuccessful";
    } else {
        cout << "File read successfully! ";
    }
    cout << "Values read: " << srcBlurDuration.size() << endl;

for (unsigned int i = 0; i < srcBlurDuration.size()-1;i++){
    seed = srcBlurDuration.at(i);
    seedSet.push_back(seed);

    if (srcBlurDuration.at(i+1) == 0xFFFFFFFF){ //sets up next batch
        //remove if wanting delimiters out of data.
        seedSet.push_back(srcBlurDuration.at(i+1));

        allSets.push_back(seedSet); //Adds current set to vector of sets.
        seedSet.clear(); //clears current set to be added to.
        i++;
        continue; //the 2nd i++ so we are back on track.
        }
}

int debugSetsToDelete = 0;
int valuesOut = 1;
for (unsigned int i = 0; i < allSets.size();i++){
    if (i < allSets.size()-2 && allSets.at(i).at(1) == allSets.at(i+1).at(1)){
        //if the first of the set is duped at least once, delete all but the first copy of that dupe.
        while (i+debugSetsToDelete+1 < allSets.size() && allSets.at(i).at(0) == allSets.at(i+1).at(0)){
            allSets.erase(allSets.begin()+i+1);
        }
    }
    for (unsigned int j = 0; j < allSets.at(i).size();j++){ //should always be 7 values.
        // results << dec << j << " : ";
        valuesOut++;
        results << hex << allSets.at(i).at(j);
        results << endl;
    }
    // results << endl;
    // cout<<"Processed:" << i << endl;
}
cout << "Values out: " << valuesOut << endl;
cout << "Values removed: " << srcBlurDuration.size() - valuesOut; 


// bool markOfBadness = false;
// int debugSetsToDelete = 0;
// for (unsigned int i = 0; i < allSets.size();i++){
//     if (i < allSets.size()-2 && allSets.at(i).at(0) == allSets.at(i+1).at(0) && allSets.at(i+1).at(0) == allSets.at(i+2).at(0)){
//         debugSetsToDelete = 2;
//         while (i+debugSetsToDelete+1 < allSets.size() && allSets.at(i).at(0) == allSets.at(i+debugSetsToDelete+1).at(0)){
//             debugSetsToDelete++;
//         }
//     }
//     for (unsigned int j = 0; j < allSets.at(i).size();j++){ //should always be 7 values.
//         results << dec << j << hex << allSets.at(i).at(j);
//         if (markOfBadness){
//             results << " --- deleted!";
//         }
//         results << endl;
//     }
//     results << endl;
//     if (debugSetsToDelete > 0){
//         debugSetsToDelete--;
//         markOfBadness = true;
//     } else {
//         markOfBadness = false;
//     }
// }



    // bool deleteSection = false;
    // for (unsigned int i = 0; i < srcBlurDuration.size()-1;i++){
    //     seed = srcBlurDuration.at(i);
    //     results << dec << i << " : " << hex << seed;
    //     if (i % 8 == 0){
    //       if (seed == srcBlurDuration.at(i+8) && seed == srcBlurDuration.at(i+16)){
    //         // srcBlurDuration.erase(srcBlurDuration.begin()+i+8,srcBlurDuration.begin()+i+22);
    //         results << " --- Dupes detected!";
    //         deleteSection = true;
    //       } else {
    //           deleteSection = false;
    //       }
    //     }
    //     if (deleteSection && i % 32 > 8 && i % 32 < 24){
    //         results << " --- DELETED";
    //     }
    //     if (srcBlurDuration.at(i+1) == 0xFFFFFFFF){ //sets up next batch
    //         results << "\n\n";
    //         seed = srcBlurDuration.at(i+2); //advances iterator
    //         allBlurDurations.push_back(currentFrame); //Adds current set to vector of sets.
    //         currentFrame.clear(); //clears current set to be added to.
    //         i++;
    //         continue; //the 2nd i++ so we are back on track.
    //     }
    //     results << "\n";
    // }









    return 0;
}