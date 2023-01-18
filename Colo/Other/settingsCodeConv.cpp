#include "../processCore.h"

char codeConv(int sourceValue, int charOffset){
   
    return sourceValue + charOffset;
}

void runConvert(int testOffset){
PokemonRequirements inputReqs;
inputReqs.defIV = 10;
inputReqs.spAtkIV = 31;
inputReqs.speedIV = 22;
inputReqs.validNatures.fill(0);
inputReqs.validNatures[15] = true;
inputReqs.validNatures[16] = true;
inputReqs.validNatures[19] = true;
inputReqs.validHPTypes.fill(0);
inputReqs.validHPTypes[11] = true;
inputReqs.validHPTypes[12] = true;
inputReqs.hiddenPowerPower = 68;
inputReqs.genderIndex = 2;
inputReqs.isShiny = 0;
 
std::string resultCode  = "";
std::vector<int> IVSet = {inputReqs.hpIV,inputReqs.atkIV,inputReqs.defIV,inputReqs.spAtkIV,inputReqs.spDefIV,inputReqs.speedIV};

for (unsigned int i = 0; i < IVSet.size(); i++)
{
    resultCode += codeConv(IVSet.at(i),testOffset);
}
int numNat = 0;
std::string natCode = "";
for (unsigned int i = 0; i < inputReqs.validNatures.size(); i++)
{
    if (inputReqs.validNatures.at(i)){
        // resultCode += codeConv(i); 
        //std::cout << "Found!\n";
        numNat++;
        natCode+= codeConv(i,testOffset);
    }
}
resultCode += codeConv(numNat,testOffset) + natCode;

int numHPT = 0;
std::string HPTCode = "";
for (unsigned int i = 0; i < inputReqs.validHPTypes.size(); i++)
{
    if (inputReqs.validHPTypes.at(i)){
        // resultCode += codeConv(i); 
        //std::cout << "Found!\n";
        numHPT++;
        HPTCode+= codeConv(i,testOffset);
    }
}
resultCode+=codeConv(numHPT,testOffset) + HPTCode;

resultCode+=codeConv(inputReqs.hiddenPowerPower,testOffset);
resultCode+=codeConv(inputReqs.genderIndex,testOffset);
resultCode+=codeConv(inputReqs.isShiny,testOffset);




std::cout << resultCode << "\n";
}


int main(){

const int CHAR_OFFSET = 46; // "." <- arbitrary starting value, seems like the best mix of readable and non-exotic chars likely to arise.
// for (int i = 0; i < 100; i++)
// {
//         runConvert(i);
// }
runConvert(CHAR_OFFSET);



// char working = codeConv(inputReqs.hpIV); //c style way
// resultCode.append(&working);
// resultCode += working;
// std::cout << working << std::endl;




//Already noticing that the prevalence of similar char's like 0 and O make things confusing.
//Maybe pull a nintendo and use char's that aren't easily confused for each other
//Or instead use a special char for 0, OR select a certain range of the char table.
//What are the biggest numbers I'd include in this code?
//ideally want the shortest possible code.




}