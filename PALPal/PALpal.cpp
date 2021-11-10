#include "../processCore.h"
enum class WantedShininess
  {
    notShiny,
    shiny,
    any
  };
static const std::array<std::array<int, 2>, 10> s_quickBattleTeamMaxBaseHPStat = {{{{322, 340}},
                                                                                   {{310, 290}},
                                                                                   {{210, 620}},
                                                                                   {{320, 230}},
                                                                                   {{310, 310}},
                                                                                   {{290, 310}},
                                                                                   {{290, 270}},
                                                                                   {{290, 250}},
                                                                                   {{320, 270}},
                                                                                   {{270, 230}}}};

u32 complexPID(u32& seed, const u32 hTrainerId,
                                                     const u32 lTrainerId, const u32 dummyId,
                                                     const WantedShininess shininess,
                                                     const s8 wantedGender, const u32 genderRatio,
                                                     const s8 wantedNature)
{
  u32 pid = 0;
  bool goodId = false;
  int nbrPidAttemptLeft = 3145728;
  while (nbrPidAttemptLeft > 0)
  {
    // A personality ID is generated as candidate, high then low 16 bits
    pid = simplePID(seed);
    nbrPidAttemptLeft--;

    // If we want a gender AND the gender of the pokemon is uncertain
    if (wantedGender != -1 && !(genderRatio == 0xff || genderRatio == 0xfe || genderRatio == 0x00))
    {
      if (wantedGender == 2)
      {
        u8 pokemonIdGender = genderRatio > (dummyId & 0xff) ? 1 : 0;
        u8 idGender = genderRatio > (pid & 0xff) ? 1 : 0;
        if (pokemonIdGender != idGender)
          continue;
      }
      else
      {
        u8 idGender = genderRatio > (pid & 0xff) ? 1 : 0;
        if (idGender != wantedGender)
          continue;
      }
    }

    // Reroll until we have the correct nature in the perosnality ID
    if (wantedNature != -1 && pid % 25 != wantedNature)
      continue;

    if (shininess != WantedShininess::any)
    {
      if (shininess == WantedShininess::notShiny)
      {
        if (!isPidShiny(lTrainerId, hTrainerId, pid))
          return pid;
      }
      else if (shininess == WantedShininess::shiny)
      {
        if (isPidShiny(lTrainerId, hTrainerId, pid))
          return pid;
      }
    }
    else
    {
      return pid;
    }
  }
  return pid;
}

void condensedGenerateMon (PokemonProperties &main, u32 &seed, int genderRatio){
    u32 TID, SID;
    setIDs(seed,TID,SID);
    main.trainerId = TID;
    LCGn(seed,2); //Dummy Pid
    extractIVs(main,seed);
    LCG(seed); //Ability
    fillStarterGenHiddenPowerInfo(main);
    u32 PID = complexPID(seed, SID, TID, 0, WantedShininess::any,-1,0,-1);
    //u32 PID = simplePID(seed); //for xd, is there any difference between using complexpid and simplepid? Can't tell any in-game filters
    main.isShiny = isPidShiny(TID,SID,PID);
    main.genderIndex = getPidGender(genderRatio,PID);
    main.natureIndex = PID % 25;
}
bool foundRunnable(PokemonProperties candidate, PokemonRequirements reqs){
    return (
        candidate.hpIV >= reqs.hpIV &&
        candidate.atkIV >= reqs.atkIV &&
        candidate.defIV >= reqs.defIV &&
        candidate.spAtkIV >= reqs.spAtkIV &&
        candidate.spDefIV >= reqs.spDefIV &&
        candidate.speedIV >= reqs.speedIV &&
        candidate.hiddenPowerPower >= reqs.hiddenPowerPower &&
        reqs.validHPTypes[candidate.hiddenPowerTypeIndex] &&
        reqs.validNatures[candidate.natureIndex]
    );
}
void determineInstructions(int instructions[4], int remainingCalls){
//Instruction determiner.

    while (remainingCalls > 0){
        if (remainingCalls >= 1009){
            instructions[0] = remainingCalls / 1009;
            remainingCalls -= instructions[0] * 1009;
        } else {
            if (remainingCalls >= 40){
              
                instructions[1] = remainingCalls / 40; //integer division intentional, want to always round down for safety.
                remainingCalls -= instructions[1]*40;
            } else {
              std::cout <<"REM: " << remainingCalls << "\n";
                if (remainingCalls > 1){
                  
                    instructions[2] = remainingCalls / 2;
                    remainingCalls -= instructions[2]*2;
                } else {
                    std::cout << "Rip - uneven, target is impossible, continuing to search.\n";
                    remainingCalls = 0;
                }
            }
        }
    }
}

std::array<u8, 6> generateEVs(u32& seed, const bool allowUnfilledEV, const bool endPrematurely)
{
  std::array<u8, 6> EVs = {0};
  int sumEV = 0;
  for (int j = 0; j < 101; j++)
  {
    sumEV = 0;
    for (int k = 0; k < 6; k++)
    {
      EVs[k] += (LCG(seed) >> 16) & 0xFF;
      sumEV += EVs[k];
    }
    if (allowUnfilledEV)
    {
      if (sumEV <= 510)
      {
        return EVs;
      }
      else
      {
        if (j >= 100)
          continue;
        EVs.fill(0);
        continue;
      }
    }
    else if (sumEV == 510)
    {
      return EVs;
    }
    else if (sumEV <= 490)
    {
      continue;
    }
    else if (sumEV >= 530)
    {
      if (j >= 100)
        continue;
      EVs.fill(0);
      continue;
    }
    else
    {
      break;
    }
  }
  bool goodSum = false;
  while (!goodSum && !endPrematurely)
  {
    for (int k = 0; k < 6; k++)
    {
      if (sumEV < 510)
      {
        if (EVs[k] < 255)
        {
          EVs[k]++;
          sumEV++;
        }
      }
      else if (sumEV > 510)
      {
        if (EVs[k] != 0)
        {
          EVs[k]--;
          sumEV--;
        }
      }

      if (sumEV == 510)
        goodSum = true;
    }
  }
  return EVs;
}
u32 myRollRNGToBattleMenu(u32 &seed, bool fromBoot, bool validMemcard)
{
  // bool fromBoot = false; //false would be from title screen.
  // bool validMemcard = true;
  if (fromBoot){
    LCGn(seed, 1137);
  }
  if (!validMemcard){
    LCGn(seed,1009); //w no memcard.
  }
  LCG(seed);
  LCG(seed);
  
  // Some dummy team gen going on...
  for (int i = 0; i < 4; i++)
  {
    // The player trainer ID is generated, low then high 16 bits
    u32 lTrainerId = LCG(seed) >> 16;
    u32 hTrainerId = LCG(seed) >> 16;
    for (int j = 0; j < 2; j++)
    {
      // Dummy personality ID (doesn't matter)
      LCG(seed);
      LCG(seed);
      // HP, ATK, DEF IV
      LCG(seed);
      // SPEED, SPATK, SPDEF IV
      LCG(seed);
      // Ability
      LCG(seed);
      complexPID(seed, hTrainerId, lTrainerId, 0, WantedShininess::notShiny,-1,0,-1);
      generateEVs(seed, true, true);
    }
  }
  return seed;
}

bool generateBattleTeam(u32 &seed, const std::vector<int> criteria,std::vector<int>m_criteria)
{
  // Player trainer name generation
  LCG(seed);
  // Player team index
  int playerTeamIndex = (LCG(seed) >> 16) % 5;
  if (playerTeamIndex != criteria[0] && criteria[0] != -1)
    return false;
  // Enemy team index
  int enemyTeamIndex = (LCG(seed) >> 16) % 5;
  if (enemyTeamIndex != criteria[1] && criteria[1] != -1)
    return false;
  // modulo 6 -- decides arena!
  LCG(seed);

  // Enemy gen

  std::vector<int> obtainedEnemyHP;
  // The enemy trainer ID is generated, low then high 16 bits
  u32 lTrainerId = LCG(seed) >> 16;
  u32 hTrainerId = LCG(seed) >> 16;
  // Pokemon gen
  for (int i = 0; i < 2; i++)
  {
    // Dummy personality ID (doesn't matter)
    LCG(seed);
    LCG(seed);
    // HP, ATK, DEF IV
    LCG(seed);
    u32 hpIV = (seed >> 16) & 31;
    // SPEED, SPATK, SPDEF IV
    LCG(seed);
    // Ability
    LCG(seed);
    complexPID(seed, hTrainerId, lTrainerId, 0, WantedShininess::notShiny,-1,0,-1);
    std::array<u8, 6> EVs = generateEVs(seed, false, false);
    int hp = EVs[0] / 4 + hpIV + s_quickBattleTeamMaxBaseHPStat[enemyTeamIndex + 5][i];
    if (hp != criteria[4 + i] && criteria[4 + i] != -1)
      return false;
    obtainedEnemyHP.push_back(hp);
  }

  // modulo 6 ???
  LCG(seed);

  // Player gen

  std::vector<int> obtainedPlayerHP;
  // The player trainer ID is generated, low then high 16 bits
  lTrainerId = LCG(seed) >> 16;
  hTrainerId = LCG(seed) >> 16;
  // Pokemon gen
  for (int i = 0; i < 2; i++)
  {
    // Dummy personality ID (doesn't matter)
    LCG(seed);
    LCG(seed);
    // HP, ATK, DEF IV
    LCG(seed);
    u32 hpIV = (seed >> 16) & 31;
    // SPEED, SPATK, SPDEF IV
    LCG(seed);
    // Ability
    LCG(seed);
    complexPID(seed, hTrainerId, lTrainerId, 0, WantedShininess::notShiny,-1,0,-1);
    std::array<u8, 6> EVs = generateEVs(seed, false, false);
    int hp = EVs[0] / 4 + hpIV + s_quickBattleTeamMaxBaseHPStat[playerTeamIndex][i];
    if (hp != criteria[2 + i] && criteria[2 + i] != -1)
      return false;
    obtainedPlayerHP.push_back(hp);
  }
    m_criteria.clear();
    m_criteria.push_back(playerTeamIndex);
    m_criteria.push_back(enemyTeamIndex);
    m_criteria.insert(m_criteria.end(), obtainedPlayerHP.begin(),
                                   obtainedPlayerHP.end());
    m_criteria.insert(m_criteria.end(), obtainedEnemyHP.begin(),
                                   obtainedEnemyHP.end());
  return true;
}
std::vector<u32> autoRollN(u32&seed,int n,std::vector<int>m_criteria){
  //Reroll loop!
  std::vector<u32>rerollSeeds;
  for (int i = 0; i < n; i++)
  { 
    generateBattleTeam(seed,m_criteria,m_criteria);
    //std::cout << getLastObtainedCriteriasString() << "\n";
    rerollSeeds.push_back(seed);
    //std::cout << std::hex << "Seed: "<< seed << std::dec << "\n\n";
    m_criteria = {-1, -1, -1, -1, -1, -1}; //refresh criteria slots.
  }
}
u32 singleRoll(u32&seed,std::vector<int>m_criteria){
  generateBattleTeam(seed,m_criteria,m_criteria);
  return seed;
}

int main(){
    const std::string hpTypes[16] = {"Fighting", "Flying", "Poison", "Ground", "Rock", "Bug", "Ghost", 
    "Steel", "Fire", "Water", "Grass", "Electric",  "Psychic", "Ice","Dragon","Dark"};
    enum hpTypeID {Fighting, Flying, Poison, Ground, Rock, Bug, 
     Ghost, Steel, Fire, Water, Grass, Electric, Psychic, Ice, 
     Dragon, Dark}; //could remake this into a map lol.

    const std::string naturesList[25] = {"Hardy","Lonely","Brave","Adamant","Naughty","Bold","Docile","Relaxed",
    "Impish","Lax","Timid","Hasty","Serious","Jolly","Naive","Modest","Mild","Quiet","Bashful",
    "Rash","Calm","Gentle","Sassy","Careful","Quirky"};
    enum natureID {Hardy,Lonely,Brave,Adamant,Naughty,Bold,Docile,
    Relaxed,Impish,Lax,Timid,Hasty,Serious,Jolly,Naive,Modest,Mild,
    Quiet,Bashful,Rash,Calm,Gentle,Sassy,Careful,Quirky};

    const int namingValue = 2;
    const int rumbleValue = 40; //note in colo this is 20 calls instead.
    const int memcardValue = 1009;
    std::vector<int> m_criteria = {-1, -1, -1, -1, -1, -1};
    u32 userInputRerollSeed = 0x10EA571D;

    u32 userInputTargetSeed = 0x0; //eventually, once I locally implement rerolling, will Only need this input.
    u32 seed = userInputRerollSeed;
    u32 listingSeed = seed;
    u32 titleSeed = 0x0;
    u32 debugSeed = 0;
    int instructions[4] = {0,0,0,0};
    const int eeveeGenderRatio = 0x1F;
    std::vector<u32> rerollSeeds;

    PokemonProperties eevee;
    PokemonRequirements requirements;
    requirements.validHPTypes.fill(0);
    requirements.validNatures.fill(0);

    requirements.spAtkIV = 31;
    requirements.speedIV = 22;
    requirements.hiddenPowerPower = 68;
    requirements.validHPTypes[Psychic] = true;
    requirements.validHPTypes[Electric] = true;
    requirements.validNatures[Mild] = true;
    requirements.validNatures[Modest] = true;
    requirements.validNatures[Rash] = true;


    //search for runnable eevee.
    while(!foundRunnable(eevee,requirements)){
        listingSeed = LCG(seed);
        condensedGenerateMon(eevee,seed,eeveeGenderRatio);
        seed = listingSeed;
    }
    std::cout << "Reached: " << std::hex << listingSeed << std::endl;
    LCGn_BACK(seed,1002);
    std::cout << "Target (seed at title screen): " << seed << std::endl;
    titleSeed = seed;

    //find calls to target.
    u32 searchSeed = userInputRerollSeed;
    int callsToTarget = 0;
    callsToTarget = findGap(searchSeed,seed,1);
    
    std::cout << "First valid eevee found at: " << std::hex << searchSeed << std::dec<< " which is " << callsToTarget << " calls away\n";
    std::cout << "TID: " << eevee.trainerId 
    <<  "\n" << eevee.hpIV
    << " / " << eevee.atkIV
    << " / " << eevee.defIV
    << " / " << eevee.spAtkIV
    << " / " << eevee.spDefIV
    << " / " << eevee.speedIV
    << " \nHidden Power: " << hpTypes[eevee.hiddenPowerTypeIndex]
    << ": " << eevee.hiddenPowerPower
    << "\nNature: " << naturesList[eevee.natureIndex];
    if (!eevee.genderIndex){
        std::cout << "\nFemale";
    } else {
        std::cout << "\nMale";
    }
    std::cout << "\n";

//~~~~~~~~~~~~~~~~~~Target seeking is complete!~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~Now find a path to target.~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//Assume starting in reroll menu
//Starting off with a reroll focused system, may eventually move to memcard focused.
//lets just see what happens.

//0.73s vs 3.73, memcard is worth 5x rerolls, ish in time, but less than that in calls, typically.
//plus no benefit of seeing where you are exactly.


//goal, bring findGap(seed,titleSeed,1) to zero.
while (true)
{
  if (findGap(seed,titleSeed,1) > 0){
    rerollSeeds.push_back(generateBattleTeam(seed,m_criteria,m_criteria));
    m_criteria = {-1, -1, -1, -1, -1, -1};
  }
}


//before applying to seed, simulate the next roll to ensure we don't overshoot.


// int lastViableDistance;
// while (true)
// {
//   u32 preSeed = seed;
//   rerollSeeds.push_back(generateBattleTeam(seed,m_criteria,m_criteria));
//   m_criteria = {-1, -1, -1, -1, -1, -1};
//   int distance = findGap(preSeed,seed,1);
//   if (distance <= callsToTarget){
//     callsToTarget-= distance;
//     instructions[3]++;
//     lastViableDistance = distance;
//   } else {
//     std::cout << "Roll would have caused: " << distance << " calls to occur, putting the count at " << callsToTarget - distance << std::endl;
//     while(callsToTarget % 2 == 1 && callsToTarget < 1009){
//       rerollSeeds.pop_back();

//     }
//     rerollSeeds.pop_back();
//     seed = rerollSeeds.back();
//     break;
//   }
// }

// callsToTarget = findGap(seed,target,1);










// //guaranteed calls
// while(callsToTarget > 2269){
//   rerollSeeds.push_back(generateBattleTeam(seed,m_criteria,m_criteria));
//   m_criteria = {-1, -1, -1, -1, -1, -1};
// } //highest observed reroll amount, should be safe.

// u32 testSeed = seed;
// while(callsToTarget > 0){
//   rerollSeeds.push_back(generateBattleTeam(testSeed,m_criteria,m_criteria));
//   if(findGap(testSeed,seed,1) < callsToTarget){
//     instructions[4]++;
//   }
// }


// //shoot first and ask questions later method:
// while(callsToTarget > 0){
//   u32 preSeed = seed;
//   rerollSeeds.push_back(generateBattleTeam(seed,m_criteria,m_criteria));
//   callsToTarget -= findGap(preSeed,seed,1);
// }
// if (callsToTarget < 0){
//   rerollSeeds.pop_back();
//   callsToTarget+= (rerollSeeds.back(),seed);
//   seed = rerollSeeds.back();
// }



//This is an area to target for a performance boost.
//I know, I know, I took out the counter function from LCG() because I thought it was superfluous but now
//it actually matters. This manip works a bit differently than NTSC, since ntsc simply scans until it gets lucky, so the
//specific counter isn't that important, except for debugging.
//This manip identifies a target in the vast swirling ocean of RNG and hunts it down directly.
  //std::cout << "Debug seed: " << debugSeed << " at target: " << std::dec << target + 115 << std::endl << std::endl;
    std::cout <<"REM1: " << callsToTarget << std::endl;
    if (callsToTarget >= 0){
      determineInstructions(instructions,callsToTarget-2);
    } else {
      std::cout << "ERROR! OVERSHOT!";
    }
    
    std::cout << instructions[3] << "/" << instructions[0] 
    << "/" << instructions[1] << "/" << instructions[2] << std::endl;
    


    std::cout << "To reach target, peform: \n"
    << instructions[3] << " reroll(s),\n"
    << instructions[0] << " memory card reload(s),\n"
    << instructions[1] << " rumble switch(es),\n"
    << instructions[2] << " naming screen back-outs.\n";

    debugSeed = userInputRerollSeed;
    std::cout << std::hex << LCGn(debugSeed,memcardValue*instructions[0]);
    std::cout << "\n" << LCGn(debugSeed,rumbleValue*instructions[1]);
    std::cout << "\n" << LCGn(debugSeed,namingValue*instructions[2]);

    return 0;
}

    /*

    METHOD:
    Get the seed of the NTSC REROLL at the bottom, on the final reroll's page. Unfortunately can't copy/paste.

    Get Seed of Target espeon.

    Reroll up to the target like you normally would.

    Program calc's distance from reroll seed to target, and then gives you instructions to get to a value 2 frames before
    the target seed, which is 1000 frames before the seed you see in the program.

    From there, follow the program's instructions: 
    [0] = memcard reload. take memcard out and continue without saving. then reinsert memcard, back to main menu and re enter. 1009 calls.
    [1] = Rumble switch. Go into options menu, flip it from on to off or vice versa, and accept the save prompt. 40 calls.
    [2] = Naming screen back outs. Enter name screen and press b to return to main menu. 2 calls.
    [3] = Traditional re-roll. WIP. Unknown call amount. Will know once implemented.


    example: target is ntsc 268 rerolls out. 
    351C941E initial.
    8A55887A 10 rerolls to ntsc target.
    Frame 562.

    F9FD38FF on title screen.

    
*/  
  
/*
may need to locally implement rerolling function T_T

    // LCGn(seed,2);
    // std::cout << "Initial 2 calls, result: " << std::hex << seed << std::endl;
    // for (int i = 0; i < 724; i++)
    // {
    //     rollRNGwithHiSeed(seed);
    // }


    // for (int i = 0; i < target; i++)
    // {
    // rollRNGwithHiSeed(seed);
    // debugSeed = seed;
    // if (i == target -1){
    //     std::cout <<"AIM FOR THIS: " << std::hex << debugSeed << std::endl;
    // }
    // LCGn(debugSeed,1000);
    // }
    

    // for (int i = 0; i < 116; i++){
    //     rollRNGwithHiSeed(seed);
    // }
    // std::cout << "NameScreen Seed: " << std::hex << seed << std::endl;
    */