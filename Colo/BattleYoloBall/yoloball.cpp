#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>
#include <vector>
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
uint32_t LCGPercentage(uint32_t& seed){
  float percentResult = 0;
  uint32_t hiSeed = 0;
  LCG(seed);
  hiSeed = seed >> 16;
  percentResult = hiSeed/65536;
  return percentResult;
}


void rollToShake (uint32_t &seed){

}


int main(){
float greatBall = 1.5;
int pokeBall = 1;
int quilavaRate = 180;

//INPUTS:

uint32_t seed = 0;
float ballChosen = pokeBall;


//Config -- these shouldn't change unless you're doing something weird.
int maxHP = 84; //between 74 and 84, learn iv code later.
int naturalRate = quilavaRate;
int statusCon = 1; 
int curHP = maxHP - 0; 


double catchRateX = 0;

//could use uint8_t or something to sim the bitmagic but thats overkill.
double successRateY = 0;
bool caught = 0;


//VARIABLE HP EQUATION:
//courtesy of dragonflyCave's article https://www.dragonflycave.com/mechanics/gen-iii-iv-capturing
// catchRateX = (3 * maxHP - 2 * curHP) * (ballChosen * naturalRate)
// / (3 * maxHP) * statusCon;


//YOLOBALL EQUATION:
catchRateX = ballChosen * naturalRate /3;

double percentageX = catchRateX/255*100;


if (catchRateX > 255){
   caught = true;
} else {
    // successRateY =floor(16711680/catchRateX);
    // successRateY = floor(sqrt(successRateY));
    // successRateY = floor(sqrt(successRateY));
    // successRateY = floor(1048560/successRateY);
    successRateY = floor(1048560/floor(sqrt(floor(sqrt(floor(16711680/catchRateX)))))); // :)
}

cout << successRateY << endl;
cout << "Chance of success: " << setprecision(5) << pow(successRateY/65536,4)*100 << "%" << endl;

//breakout sim
for (int i = 0; i<3;i++){
    LCG(seed);
    if (seed >= successRateY){
        caught = false;
        break; //hehe
    } else {
        caught = true;
    }
}






if (catchRateX < 1){
    catchRateX = 1;
}



/* follow normal formulat?

a > 0.10 somewhere.
*/


    return 0;
}