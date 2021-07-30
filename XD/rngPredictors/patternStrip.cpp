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
    string FILE_NAME = "";
    cout << "Enter filename: ";
    getline(cin, FILE_NAME);

    uint32_t seed = 0x0;
    int rolls = 0;
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
        rolls = 0;
        while(seed != rawPattern.at(i+1)){
            LCG(seed);
            rolls++;
        }
    //write results
    refinedPattern <<  rolls;
    refinedPattern << endl;
    }
    cout << "Output file produced! All done!";
    refinedPattern.close();

    return 0;
    }
