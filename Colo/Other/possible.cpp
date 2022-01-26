#include "../../processCore.h"
 
// Driver code
int main()
{   
    std::string yee = "inNums.txt";
    char digit = '0';
    std::vector<std::string> numListStr = readStringFromFile(yee);
    std::vector<int> sums;
    std::vector<float> avgs;
    float finalAvg = 0;
    int value = 0;
    for (unsigned int i = 0; i < numListStr.size(); i++) //for a string
    {
        for (unsigned int j = 0; j < numListStr[i].length(); j++) //for a char
        {
            switch (numListStr[i][j])
            {
            case '0':
                value += 56;
                break;
            case '1':
                value += 59;
                break;
            case '2':
                value += 115;
                break;
            default:
                break;
            }
        }
        sums.push_back(value);
        float lineAvg = static_cast<float>(value)/8.0;
        avgs.push_back(lineAvg);
        finalAvg += lineAvg; //currently a sum.

        value = 0;
    }
    finalAvg = finalAvg / numListStr.size();
    std::cout << "FINAL: " << finalAvg << "\n";
    for (unsigned int i = 0; i < avgs.size();i++){
        std::cout << avgs.at(i) << ", ";
        if (i % 20 == 19){
            std::cout << std::endl;
        }
    }

    return 0;
}