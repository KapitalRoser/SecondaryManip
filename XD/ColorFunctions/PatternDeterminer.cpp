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

struct RNG_ObjData {
      uint32_t startAddr;
      uint32_t comparisonAddr;
      int comparisonMainVal;
      float flipTimer;
      float timer1;
      float timer2;
    };

void redFunction (uint32_t &seed){
  //Contains some quat and vec functions, likely something to do with camera movement.
  uint32_t redVariable = 0;
  
  float redFloat = 0;

  //BITMAGIC BLOCK
  uint32_t rv_Initial = redVariable & 3;
  uint32_t rv_Large = redVariable & 128;
  uint32_t rv_Medium = redVariable & 64;
  uint32_t rv_Small = redVariable & 32;
  uint32_t rv_Micro = redVariable & 16;
  // functionally achieves a Mod to a power of 2, but it's a bit weird so I'm leaving it in it's bitmagic form.


  //redVariable initialization from r5's addr.

  redVariable += 265;

  //Can potentially simplify further.
  //Can get called but redvariable (r0) could be something super high like 40.
  if (rv_Initial != 2){
    if (rv_Initial < 2) {
      if(rv_Initial != 0){
        if (rv_Medium == 0){
          LCGPercentage(seed);
        }
        if (rv_Large == 0){
          LCGPercentage(seed);
        }
        if (rv_Small == 0){ 
          LCGPercentage(seed);
        }
        return;
      }
      if (0x804eda38 < redFloat){ // || 0x804eda38 < fvar2 after some calcs. ) && dvar < 0x804eda38, which is gained from some memory addr. 
        if (rv_Large == 0){
          LCGPercentage(seed);
        }
        if (rv_Small == 0){
        LCGPercentage(seed);
        }
      }
      if (rv_Micro == 0){
        LCGPercentage(seed);
      }
      return;
    }
    if (rv_Initial < 4){
      if (rv_Large == 0){
        LCGPercentage(seed);
      }
      if (rv_Small == 0){
        LCGPercentage(seed);
      }
    }
    return;
  }
  return;
}

void BlueFunction(uint32_t &seed, float f1, uint32_t structAddr, uint32_t localAC){
//6 seperate rng calls can happen, some looping changes this.
int addrPUVar6 = structAddr + 252;
int addrVar2 = 0; //where dis come from.

for (int i = 0;i<2;i++){
  redFunction(seed);

  addrVar2 = addrPUVar6 + 29;
  if ((addrVar2 & 2) == 0){
    LCGPercentage(seed);
    LCGPercentage(seed);
    if (addrPUVar6 + 29 < 32){ 
      LCGPercentage(seed);
    }
  } else {
    if (addrVar2 < 32){
      LCGPercentage(seed);
    }
  }
  LCGPercentage(seed); //guaranteed.
  LCGPercentage(seed);
}

}
uint32_t bluLoopEntryConditions (uint32_t &seed, RNG_ObjData cutsceneData,float param_1){
  const double recurringDouble = 0.000009999999747378752;
  const double SecondRecurringDouble = 0; //literally 0.

  double d_Main = 0; //dvar 10
  double d_Secondary = 0; //dvar 12
  uint32_t localAc = 0;
  double localb0 = 2;

  if (cutsceneData.flipTimer < 64){
    if(cutsceneData.flipTimer >= 8 || cutsceneData.comparisonAddr + 24 <= cutsceneData.timer1){
      cutsceneData.flipTimer = static_cast<uint32_t>(cutsceneData.flipTimer) | 8;
      if (cutsceneData.flipTimer < 16 || 
      cutsceneData.flipTimer < (cutsceneData.comparisonAddr + 24) + (cutsceneData.comparisonAddr + 28)){
        //the cutsceneData.comparisonAddr + 24, and the +10 is usually == 0.
        cutsceneData.timer1 = static_cast<uint32_t>(cutsceneData.timer1) | 10;
        d_Main = LCGPercentage(seed);
        d_Main = d_Main * (cutsceneData.comparisonAddr + 10) + cutsceneData.comparisonMainVal;

        //if dvar 12 matters, then if statements and math go here:

        if (recurringDouble < static_cast<float>(cutsceneData.comparisonAddr + 28)){
          d_Main = (d_Main * recurringDouble) * param_1 + 0.14898967742919922; //startAddr+ 0x34
          localb0 = round(d_Main); //dvar 12?
          localAc = localb0;
          if (localb0 == 0 && cutsceneData.timer2 == 0){
            localAc = 1;
          }
        } else {
          localb0 = round(d_Main); // dvar 12 factors in at all?
        }
      }
    }
  }
  return localAc;
}


void lateBlueHandler (uint32_t &seed, RNG_ObjData cutsceneData){
  int availableAddrs;
  uint32_t ptr1 = 0;
  uint32_t ptrToSomething = 0;
  int rawCutsceneDataOffset = 0;
  int cutsceneDataOffset1 = 0;
  int cutsceneDataOffset2 = 0;
  
  if (cutsceneData.startAddr + 0x19 == 0){
    availableAddrs = 0;
    return;
  } else {
    //function runs to check if memory is used for a given offset area.
    availableAddrs = 2; //does this change?
    for(int i = availableAddrs; i > 0; i--){
      //two pointers increment each loop.
      
      lateBlue(seed,ptr1,rawCutsceneDataOffset, cutsceneDataOffset1, cutsceneDataOffset2, ptrToSomething);
      ptr1 += 0x28;
      ptrToSomething += 0x48;
    }

  }

}

void lateBlue (uint32_t &seed,int rawStructOffset,int structOffset1, int StructOffset2, uint32_t ptr1,uint32_t ptr2){
  //structoffset are values not addrs here, can convert somewhere else.
  if ((StructOffset2 & 1) == 1){
    LCGPercentage(seed);
   
    if ((structOffset1 & 0x40) != 0){
      LCGPercentage(seed);
    }

    if (structOffset1 & 0x20 !=0){
    LCGPercentage(seed); // used as part of an if statement condition for some other code.
    }
    
    LCGPercentage(seed);

    if ((structOffset1 & 0x300000) == 0x300000){
      LCGPercentage(seed);
    }









  }
  return;
}

void PurpleFunction (uint32_t &seed,float param_1, vector<RNG_ObjData>RNGCycle, int zero, uint32_t addrPlus8){
  // struct data{
  //   uint32_t originData = 65;
  //   uint32_t data18 = 0; ===flipTimer //0x12 also written as +0x40 ingame. 38 to start, then after some time becomes 18. 
  //   uint32_t ptrData63 = 1; //0x3f also written as +0xfc ingame. contains addr. 8100E31C
  //   uint32_t data12 = 2.485816240310669; //0xC or 0x30 ingame. also a timer, goes up to 20s roughly. -- timer1
  //   uint32_t data74 = 0; //0x4A  --varies, some kind of timer, max value 1160ish.
  //   uint32_t data14 = 0;
  //   cutscene comaprisonAddr + 28 -- +1c, typically ==20.
  // };
  double d_Main = 0;
  double d_Secondary = 0;
  uint32_t localAc = 0;
  double localb0 = 2;
  const double recurringDouble = 0.000009999999747378752;
  const double SecondRecurringDouble = 0; //literally 0.

//SETUP THIS LOOP AT SOME POINT DUMMY.
RNG_ObjData cutsceneData = RNGCycle.at(0);


//Normally ranges between 1 2 or 3.
localAc = bluLoopEntryConditions(seed,cutsceneData,param_1);
  
  if (localAc != 0){
    if (10 < localAc){
      localAc = 10;
    }
    if (addrPlus8 + 7 < recurringDouble){
      param_1 = SecondRecurringDouble; //sets param_1 to 0.
    }
    BlueFunction(seed,param_1,cutsceneData.startAddr,localAc);
      //blue func. gets called with param_1, 0x8100FAE4 (addr for cutscene data struct) and localAc
  }

    while (cutsceneData.flipTimer != 0.0){
      //Some rng calls?
      
      cutsceneData.flipTimer + 4;
    }



return;
}

void greenFunction(uint32_t &seed, vector<RNG_ObjData> cycleAddr, float param_1){
  PurpleFunction(seed,param_1,cycleAddr,0,cycleAddr.at(0).comparisonAddr + 0x108 + 0x8);
  return;
}

int main(){

    //callerGreen and Green are set up with the exception of param_1
    //Purple needs to work out how lateblue is looped within purple
    //blue loop is almost done
    //red is 99% done and can be simulated I think.

    uint32_t seed = 0x0;

    uint32_t memoryAddr = 0; 


    

    vector<RNG_ObjData>rngObjCycle; //NO MATTER THE PATTERN, ALWAYS LOADS OBJECTS IN THIS ORDER.

    //break down these structs further?
    //FOUNTAINS
    RNG_ObjData obj1; //South fountain
    obj1.startAddr = 0x8100FAE0; // likely the start of active, changing data
    obj1.comparisonAddr = 0x8100E31C; //+C for mainVal, doesn't change. Likely original constants of the obj.
    obj1.comparisonMainVal = 60;
    obj1.flipTimer = 0;
    //Add startAddr + 0x19, static value. Pretty sure it's a boolean that flips on or off.

    RNG_ObjData obj2; //Middle fountain
    obj2.startAddr = 0x8101b460;
    obj2.comparisonAddr = 0x81019c5c;
    obj2.comparisonMainVal = 80;

    RNG_ObjData obj3;
    obj3.startAddr = 0x8102D720; //top fountain
    obj3.comparisonAddr = 0x8102BF1C;
    obj3.comparisonMainVal = 40;



    //these two objects crash the game if their pointers are gone. Must be important.
    //Entering Pkmn/PDA/Item menu causes these to be unloaded.  
    //After cutscene, these are no longer called?
    RNG_ObjData obj4;
    obj4.startAddr = 0x81322D60; //crashes lol
    obj4.comparisonAddr = 0x8100585C;
    obj4.comparisonMainVal = 20;

    RNG_ObjData obj5;
    obj5.startAddr = 0x8131EB80;
    obj5.comparisonAddr = 0x8100585C; //huh
    obj5.comparisonMainVal = 20;


//param 1 stops regularly updating after cutscene finishes. However this doesn't impact overall rng right? 







    //loops thru these addrs. upon hitting 0, repeat from start.
    vector<uint32_t>addrs {0x8100FAE0,0x8101b460,0x8102D720,0x81322D60,0x8131EB80};
    vector<uint32_t>secondaryAddrs;
    float param_1 = 0;
    //pass this addr into green along with f1.

    greenFunction(seed,rngObjCycle,param_1);



    for (unsigned int i = 0; i<addrs.size()-1;i++){
        RNG_ObjData obj;
        obj.startAddr = addrs.at(i);
      rngObjCycle.push_back(obj);
    }


/*
1 purple
2 red
5 blue
2 red
13 blue
2 red
5 blue
2 red
13 blue
1 purple
2 red
5 blue
2 red
13 blue == 68 calls.


almost always, Red is going to be called twice since the loop parameter passed from purple 
into blue is usually == 2
*/






  // //First RNG Call
  // if (((weirdParam) & 0x40) == 0){
  //   if (((weirdParam) & 8) != 0){ // || load new pointer and compare that.
  //   weirdParam = weirdParam | 8; 
  //   if (((weirdParam) & 0x10) == 0){ // || param2 at new pointer <= Some more pointer math.
  //   LCGPercentage(seed);
  //   }
  //   }
  // }


  // //Enter purple:
  // if (memoryAddr < weirdParam + 0x1c){
  //   //Purple -> blue.
  // }

  // //simplify?
  // if (weirdParam <100){
  //   if (weirdParam < 10){
    
  //   }
  // }

  // redVariable += 265;
  // rv_Initial = redVariable & 3;


//& 0x80 just checks if number is > 128, and if so, result is == 128.

//WITHIN RED: 

  //The structure of this is very carefully thought out. May impact other paramters that affect blue or purple funcs.


redFunction(seed);












 


  //May need to return some float values.
    /*
Purple -> Blue:
Purple check is == whatever local_b0 is == to. If it is == 0, then set _ac to 1 and thus enter the blue func.
First: (param_2 + 0x12) & 0x40) == 0), if true:
FLOAT_804eda38 < param_2[+3f] + 28  (1c). FLOAT_804eda38 seems to be important. -- ==  3727C5AC?
If either of these ifs fail, then blue never enters.

param_2 == r3?


R31 and R30 important sources of map data.


in red: param 4 = blue Param_2 + 265 = R31 holds a pointer. 
*/



    return 0;
}