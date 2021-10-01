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
int main (){
    int rolls = 0;
    uint32_t sums = 0;
    const string FILE_EXTENSION = ".txt";
    string FILE_NAME = "";
    cout << "Enter filename: ";
    getline(cin, FILE_NAME);
    auto start = chrono::high_resolution_clock::now();
    vector<uint32_t> srcBlurDuration = hexReadNumbersFromFile(FILE_NAME + FILE_EXTENSION);
    vector<uint32_t> srcReached = hexReadNumbersFromFile("leadingAsExtendedAsSeed.txt");
    vector<vector<uint32_t>> allBlurDurations;
    vector<uint32_t>currentFrame;
    uint32_t seed = srcBlurDuration.at(0);
    ofstream results ("out" + FILE_NAME + FILE_EXTENSION);
    ofstream fightCommonDoc ("bd3FightCommonOnly.txt");
    ofstream mapDoc ("bd3MapOnly.txt");
    ofstream CamShakeDoc ("bd3CameraShakeOnly.txt");
    ofstream deckArchDoc ("bd3DeckArchiveOnly.txt");
    // ofstream comboValDoc ("bd3LastTwoValuesOnly.txt");
    if (srcBlurDuration.empty()){
        cout << "File read unsuccessful";
    } else {
        cout << "File read successfully! ";
    }
    cout << "Values read: " << srcBlurDuration.size() << endl;
    float percentComplete = 0;

    for (unsigned int i = 0; i < srcBlurDuration.size()-1;i++){ 
    
      seed = srcBlurDuration.at(i);
      rolls = 0;
      if (i == 0){
        currentFrame.push_back(seed);
        continue;
      }
      if (srcBlurDuration.at(i+1) == 0xFFFFFFFF){ //sets up next batch
          allBlurDurations.push_back(currentFrame);
          currentFrame.clear();
          i += 2; //advances loop to seed.
          currentFrame.push_back(srcBlurDuration.at(i)); //begin next set
          if (i % 10 == 0){
            percentComplete = static_cast<float>(i)/srcBlurDuration.size()*100;
            cout  << fixed << setprecision(2) << "  "  << setw(5) << percentComplete << "%" << "  " << i << " frames processed.\r";
          }
          
          
          continue; //resume as normal.
      }
      while(seed != srcBlurDuration.at(i+1)){ //core seeker. -- target this for performance improvements.
          LCG(seed);
          rolls++;
      }
      rolls = rolls / 9600; 
      currentFrame.push_back(rolls);
      // results << hex << srcBlurDuration.at(i) << " : " << dec << bFrames << " : " << rolls << endl; old debug line.
    }

    
    //File write blocks
    int ij = 0; //srcReached increment because weird out of bounds errors happen if I don't keep this seperate.
    bool splitFiles = true;
    const int VISUAL_FRAMES_START = 39710;
    int currentVisual = VISUAL_FRAMES_START;

    for (int i = 0; i < allBlurDurations.size();i++){ //Frames list.
      results << dec << currentVisual << ": ";
      currentVisual+=2;
      bool modified = false;
      bool oddVal = false;
      results << setw(5) << i << ": - Reached: " << setw(8) << hex << allBlurDurations.at(i).at(0) << " - Pattern: " << dec;

        if (allBlurDurations.at(i).at(1) != 2){
          allBlurDurations.at(i).insert(allBlurDurations.at(i).begin()+1,2);
          allBlurDurations.at(i).pop_back();
          modified = true;
          }//correction for missed first seed.
        
        //SUMMARY FILE WRITE BLOCK
        for (int j = 1; j < allBlurDurations.at(i).size();j++){ //individual value writing loop.
          if (allBlurDurations.at(i).at(j) != 5 && allBlurDurations.at(i).at(j) != 6 &&
              allBlurDurations.at(i).at(j) != 7 && allBlurDurations.at(i).at(j) != 8 &&
              allBlurDurations.at(i).at(j) != 9 && allBlurDurations.at(i).at(j) != 11 &&
              allBlurDurations.at(i).at(j) != 12 && allBlurDurations.at(i).at(j) != 2){
            oddVal = true;
          }
            results << allBlurDurations.at(i).at(j);
            if ( j < allBlurDurations.at(i).size()-1){
                results << ",";
            }

       }
        //results << hex << allBlurDurations.at(i).at(0);
        if (modified){
          results << " - modified";
        }
       
        if (oddVal){
          results << " --- BAD DATA";
        }
        
        if (ij < srcReached.size()-2){
          if (allBlurDurations.at(i).at(0) != srcReached.at(ij) ){
            if (allBlurDurations.at(i+1).at(0) == srcReached.at(ij+1)){
              results << "-- Wrong seed written. Correct seed: " << hex << srcReached.at(ij);
            } else {
              results << "\n--SKIPPED SEED: Reached: " << hex << srcReached.at(ij);
              while (allBlurDurations.at(i+1).at(0) != srcReached.at(ij+1) && ij < srcReached.size()-2){
                ij++;
              }
            }
          }
        }
        ij++;

        // sums = allBlurDurations.at(i).at(2) + allBlurDurations.at(i).at(4) + allBlurDurations.at(i).at(5);
        // results << sums;
        results << endl;

        //SPLIT FILE WRITE BLOCK:
        if (splitFiles){
         fightCommonDoc << allBlurDurations.at(i).at(2) << endl;
         mapDoc   << allBlurDurations.at(i).at(3) << endl;
         CamShakeDoc << allBlurDurations.at(i).at(5) << endl;
         deckArchDoc << allBlurDurations.at(i).at(6) << endl;

        //  comboValDoc << allBlurDurations.at(i).at(5) << "," << allBlurDurations.at(i).at(6) << endl;

        }



    }

    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
    cout << duration.count() << " ms elapsed.";
    return 0;
}