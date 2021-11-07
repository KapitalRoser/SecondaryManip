#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <math.h>
#include <numeric>

//TODO: before launching, move this into the game specific folders and update the paths.

//Typedef block -- thx aldelaro5
typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int64_t s64;

enum region {USA,EUR,JPN};
enum emuVer {STABLE,MODERN}; //Stable == 5.0, only matters for xd so far.
enum coloSecondary {QUILAVA,CROCONAW,BAYLEEF}; //xd only has teddy
//Useful functions block
u32 LCG(u32& seed){
  seed = seed * 214013 + 2531011;
  return seed;
}
u32 modpow32(u32 base, u32 exp)
{
  u32 result = 1;
  while (exp > 0)
  {
    if (exp & 1)
      result = result * base;
    base = base * base;
    exp >>= 1;
  }
  return result;
}
u32 LCGn(u32& seed, const u32 n)
  {
    u32 ex = n - 1;
    u32 q = 0x343fd;
    u32 factor = 1;
    u32 sum = 0;
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
float LCGPercentage(u32& seed){
  float percentResult = 0;
  u32 hiSeed = 0;
  LCG(seed);
  hiSeed = seed >> 16;
  percentResult = static_cast<float>(hiSeed)/65536;
  return percentResult;
}

//LCG BACK GOES HERE!


//basic generation -- only asks for gender ratio does not account for xd anti-shiny.
//Still unsure if I should put this here in the header, might need to modify it too much.
void generateMon(uint32_t inputSeed, int genderRatio){
//   uint32_t TID = 0; //Tid isn't important here since TID is already set. *might matter for shiny check on quil.
  uint32_t PID = 0;
  uint32_t seed = inputSeed;
  uint32_t outSeed = 0;
  std::string nature = " ";
  std::string hiddenPowerType = " ";
  int hiddenPowerStrength = 0;
  //Gender ratio reference:  0x7F for teddy (50/50) and 0x1F for johto starters (87.5/12.5)
  const std::string naturesList[25] = {"Hardy","Lonely","Brave","Adamant","Naughty","Bold","Docile","Relaxed",
    "Impish","Lax","Timid","Hasty","Serious","Jolly","Naive","Modest","Mild","Quiet","Bashful",
    "Rash","Calm","Gentle","Sassy","Careful","Quirky"};
  
  std::cout << std::left;

    //Some tid/sid stuff, possibly a dummy pid
    seed = LCGn(seed,4);

    outSeed = seed;
    //Would be High bits/Secret ID, used for Shiny.

    // LCG(seed); These do not appear on shadow generation.
    // LCG(seed); //originally from Dummy ID, leftover.

    //IVS
    LCG(seed);
    int hp = (seed >> 16) & 31;
    int atk = (seed >> 21) & 31;
    int def = (seed >> 26) & 31; 
 
    LCG(seed); 
    int spe = (seed >> 16) & 31;
    int spa = (seed >> 21) & 31;
    int spd = (seed >> 26) & 31; 

    
    LCG(seed); //Ability call
    //PID STUFF:
    uint32_t hId = LCG(seed) >> 16;
    uint32_t lId = LCG(seed) >> 16;
    PID = (hId << 16) | (lId);
    std::string displayNature = naturesList[PID % 25];
    bool pidGender = genderRatio > (PID & 0xFF) ? 1 : 0;
    std::string displayGender;
    if (pidGender){
        displayGender = "Female";
    } else {
        displayGender = "Male";
    }
    std::cout 
    << "Seed"  
    << ": " 
    << std::hex 
    << std::setw(8) << outSeed
    << " PID: " 
    << std::setw(8) << PID 
    << std::dec
    << "  " << std::setw(2) << hp << " " << std::setw(2) << atk << " " << std::setw(2) << def << " " << std::setw(2)
    << spa << " " << std::setw(2) << spd << " " << std::setw(2) << spe << "  "
    << std::setw(7) << displayNature << "  " << displayGender 
    << std::endl;
}


u16 rollRNGwithHiSeed(u32 &seed)
{ //mostly used in the naming screen, may have uses elsewhere, like blink.
  LCG(seed);
  u16 hiSeed = seed >> 16;
  if (static_cast<double>(hiSeed) / 65536.0 < 0.1){
    LCGn(seed, 4);
  }
  return hiSeed; //debugging.
}


//File Reading
std::vector<int> decimalReadNumbersFromFile(std::string fileName)
{
    u32 value;
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
std::vector<u32> hexReadNumbersFromFile(std::string fileName)
{
    u32 value;
    std::string lineRead = "";
    std::stringstream hexConvert;
    std::vector<u32> data; //Setup
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

//Debugging
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