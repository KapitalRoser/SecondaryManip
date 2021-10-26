#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <numeric>

//Typedef block -- thx aldelaro5
typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int64_t s64;

//Useful functions block
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
float LCGPercentage(uint32_t& seed){
  float percentResult = 0;
  uint32_t hiSeed = 0;
  LCG(seed);
  hiSeed = seed >> 16;
  percentResult = static_cast<float>(hiSeed)/65536;
  return percentResult;
}

//File Reading funcs
std::vector<int> decimalReadNumbersFromFile(std::string fileName)
{
    uint32_t value;
    int lineRead = 0;
    std::vector<int> data; //Setup
    std::ifstream file(fileName);
    std::cout << "Read some data! \n";
    if (file.fail())
    {
        std::cout << "File inaccessible";
        exit(EXIT_FAILURE);
    }
    while (!(file.fail()))
    {  
        file >> lineRead; 
        data.push_back(lineRead);
    }
    file.close();
    return data;
}
std::vector<uint32_t> hexReadNumbersFromFile(std::string fileName)
{
    uint32_t value;
    std::string lineRead = "";
    std::stringstream hexConvert;
    std::vector<uint32_t> data; //Setup
    std::ifstream file(fileName);
    std::cout << "Read some data! \n";
    if (file.fail())
    {
        std::cout << "File inaccessible";
        exit(EXIT_FAILURE);
    }
    while (!(file.fail()))
    {                 
        getline(file,lineRead); 
        hexConvert << std::hex << lineRead; //cuz i just HAD to have my docs in hexa and not int...;
        hexConvert >> value;
        hexConvert.clear();
        hexConvert.str("");
        data.push_back(value);
        // std::cout << "Lines read: " << data.size() << endl;
    }
    file.close();
    return data;
}
std::vector<std::string> readStringFromFile(std::string fileName)
{
    std::vector<std::string> data; //Setup
    std::string lineRead = "";
    std::ifstream file(fileName);
    if (file.fail())
    {
        std::cout << "File inaccessible";
        exit(EXIT_FAILURE);
    }
    while (!(file.fail()))
    {
        getline(file,lineRead);
        data.push_back(lineRead);
    }
    file.close();
    std::cout << "lines from " + fileName + " read: " << data.size() << std::endl;
    return data;
}

//debugging
void debugPrint2DVec(std::vector<std::vector<int>>set){
  //FOR DEBUG:
        for (unsigned int i = 0; i < set.size(); i++){
          for (unsigned int j = 0; j < set.at(i).size(); j++){
            std::cout << set.at(i).at(j) << ", ";
          }
          std::cout << std::endl;
        }
}
void debugPrintVec(std::vector<int>set){
    for (unsigned int i = 0; i < set.size(); i++)
    {
        std::cout << set.at(i) << ", ";
    }
    std::cout << std::endl;
}