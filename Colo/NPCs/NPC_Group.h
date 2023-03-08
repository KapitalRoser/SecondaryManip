#include "NPC.h"

class NPC_Group{
public:
NPC_Group(std::vector<NPC> in, u32 &inSeed);
private:
std::vector<NPC>set;
std::string status;
};