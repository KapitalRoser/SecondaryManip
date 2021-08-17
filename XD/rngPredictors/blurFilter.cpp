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

    string frameLabel = "";
    string comment = "";
    //Will need to reverse this to figure out where the pattern starts. 
    int positionOffset = 37;
    int nineteenLength = 30;

    int filterAmt = 20; //OBVIOUSLY CHANGE THIS AS NEEDED.
    int frames = 0;

    for (unsigned int i = 0; i < inputValues.size();i++){
      frames = inputValues.at(i);
      if (inputValues.at(i) == 19){
        frameLabel = "Nineteen";
      } else if (inputValues.at(i) == 20) {
        frameLabel = "Twenty";
      } else {
        frameLabel = "---- WEIRD CASE ----";
      }


    // if (i == 576){
    //   positionOffset--;
    // }
  
    //Changes and special cases:
    if(i == 576){
      positionOffset--;
    }
    
    

    if (i % 60 == positionOffset){ 
      filterAmt = 20;
      comment = "60F align!";
    } else if (i % 60 == positionOffset - nineteenLength - 2) {
      filterAmt = 19;
    } else {
      comment = "";
    }
    if (i == 604 || i == 664){
      frames --;
    }

    frames -= filterAmt;

    if (frames == 0){
      frameLabel = "";
    }
    outputData << left << setw(10)   << frameLabel << " - "<< frames << " - " << comment << endl;
    }





    return 0;
}