#include "../processCore.h"
struct PokemonProperties
  {
    int hpIV = 0;
    int atkIV = 0;
    int defIV = 0;
    int spAtkIV = 0;
    int spDefIV = 0;
    int speedIV = 0;
    int hpStartingStat = 0;
    int hiddenPowerTypeIndex = 0;
    int hiddenPowerPower = 0;
    int genderIndex = 0;
    int natureIndex = 0;
    bool isShiny = false;
  };
struct Stats
  {
    int hp;
    int atk;
    int def;
    int spAtk;
    int spDef;
    int speed;
  };
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

std::vector<int> m_lastObtainedCriterias = {-1, -1, -1, -1, -1, -1};


void fillStarterGenHiddenPowerInfo(PokemonProperties& starter)
  {
    int typeSum = (starter.hpIV & 1) + 2 * (starter.atkIV & 1) + 4 * (starter.defIV & 1) +
                  8 * (starter.speedIV & 1) + 16 * (starter.spAtkIV & 1) +
                  32 * (starter.spDefIV & 1);
    starter.hiddenPowerTypeIndex = typeSum * 15 / 63;
    int powerSum = ((starter.hpIV & 2) >> 1) + 2 * ((starter.atkIV & 2) >> 1) +
                   4 * ((starter.defIV & 2) >> 1) + 8 * ((starter.speedIV & 2) >> 1) +
                   16 * ((starter.spAtkIV & 2) >> 1) + 32 * ((starter.spDefIV & 2) >> 1);
    starter.hiddenPowerPower = (powerSum * 40 / 63) + 30;
  }
void extractIVs(PokemonProperties& properties, u32& seed)
  {
    // HP, ATK, DEF IV
    LCG(seed);
    properties.hpIV = (seed >> 16) & 31;
    properties.atkIV = (seed >> 21) & 31;
    properties.defIV = (seed >> 26) & 31;
    // SPEED, SPATK, SPDEF IV
    LCG(seed);
    properties.speedIV = (seed >> 16) & 31;
    properties.spAtkIV = (seed >> 21) & 31;
    properties.spDefIV = (seed >> 26) & 31;
  }
u32 generatePokemonPID(u32& seed, const u32 hTrainerId,
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
    u32 hId = LCG(seed) >> 16;
    u32 lId = LCG(seed) >> 16;
    pid = (hId << 16) | (lId);
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

std::array<u8, 6> generateEVs(u32& seed, const bool allowUnfilledEV,
                                                            const bool endPrematurely)
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
u32 rollRNGToBattleMenu(u32 seed)
{
  // Before getting to the quick battle menu, the reason this is deterministic is because the only
  // thing that is consequential is a dummy pokemon generation, but there's no criteria on the pid
  // meaning the first pid generated will always be the one used
  seed = LCGn(seed, 1139); //Something's happening here.
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
      generatePokemonPID(seed, hTrainerId, lTrainerId, 0, WantedShininess::notShiny,-1,0,-1);
      generateEVs(seed, true, true);
    }
  }


  return seed;
}
bool generateBattleTeam(u32& seed, const std::vector<int> criteria)
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
  // modulo 6 ???
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
    generatePokemonPID(seed, hTrainerId, lTrainerId, 0, WantedShininess::notShiny,-1,0,-1);
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
    generatePokemonPID(seed, hTrainerId, lTrainerId, 0, WantedShininess::notShiny,-1,0,-1);
    std::array<u8, 6> EVs = generateEVs(seed, false, false);
    int hp = EVs[0] / 4 + hpIV + s_quickBattleTeamMaxBaseHPStat[playerTeamIndex][i];
    if (hp != criteria[2 + i] && criteria[2 + i] != -1)
      return false;
    obtainedPlayerHP.push_back(hp);
  }

#pragma omp critical
  {
    m_lastObtainedCriterias.clear();
    m_lastObtainedCriterias.push_back(playerTeamIndex);
    m_lastObtainedCriterias.push_back(enemyTeamIndex);
    m_lastObtainedCriterias.insert(m_lastObtainedCriterias.end(), obtainedPlayerHP.begin(),
                                   obtainedPlayerHP.end());
    m_lastObtainedCriterias.insert(m_lastObtainedCriterias.end(), obtainedEnemyHP.begin(),
                                   obtainedEnemyHP.end());
  }

  return true;
}
std::vector<int> obtainTeamGenerationCritera(u32& seed)
{
  std::vector<int> criteria;
  // Player trainer name generation
  LCG(seed);
  // Player team index
  criteria.push_back((LCG(seed) >> 16) % 5);
  // Enemy team index
  criteria.push_back((LCG(seed) >> 16) % 5);
  // modulo 6 ???
  LCG(seed);

  // Enemy gen

  // The player trainer ID is generated, low then high 16 bits
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
    generatePokemonPID(seed, hTrainerId, lTrainerId, 0, WantedShininess::notShiny,-1,0,-1);
    generateEVs(seed, false, false);
  }

  // modulo 6 ???
  LCG(seed);

  // Player gen

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
    generatePokemonPID(seed, hTrainerId, lTrainerId, 0, WantedShininess::notShiny,-1,0,-1);
    generateEVs(seed, false, false);
  }
  return criteria;
}


int main(){
  u32 initialSeed = 0x0;
  u16*counter = 0;
  u32 seed = initialSeed;
  std::cout << "Seed after battle menu calls: " << std::hex << rollRNGToBattleMenu(seed);
  //should be A0281F1C.
    return 0;
}