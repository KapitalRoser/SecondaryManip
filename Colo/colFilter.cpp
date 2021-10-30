#include "../processCore.h"

int main(){
    u32 seed = 0; //INITIAL SEED IS THE SEED AFTER THE 6 NPC CALLS. THE ONE THAT REPEATS TWICE.
    region gameRegion = USA;
    
    //File IN/OUT
    const std::string FILE_EXTENSION = ".txt";
    std::string FILE_NAME = "";
    std::cout << "Enter filename: ";
    getline(std::cin, FILE_NAME);
    std::vector<int> inputValues = decimalReadNumbersFromFile(FILE_NAME + FILE_EXTENSION); //loads all seeds from file into RAM.
    std::ofstream refinedPattern("out" + FILE_NAME + FILE_EXTENSION); 
    std::cout << inputValues.size() << std::endl;
    std::ofstream outputData ("out" + FILE_NAME + FILE_EXTENSION);

    //CHANGE THIS AS NEEDED.
    std::string comment = "";
    std::string frameLabel = "";
    int filterAmt = 0; //OBVIOUSLY CHANGE THIS AS NEEDED.
    int initialOffset = 3;
    int stepsOffset = -1; //THIS ISN'T A REAL VARIABLE -- temporary adjustment to avoid re-typing all the step frames again.
    //Fix step frames at some point.
    int frames = 0;

    bool trackNPCs = false;
    bool trackSteps = true;
    bool trackBG = true;

    //data
    const int loFrame = 76;
    const int hiFrame = 116;

    const int xframe = 114;
    const int yframe = 154;

    std::vector<int>quilavaSteps{5,10,15,20,25,30,35,39,44,49,54,67,88,100,111,119,
    126,132,138,143,148,153,158,163,168,173,177,182,187,192,197,202,210,217,
    227,237,252,268,284,292,297,302,307,312,316,321,326,331};

    std::vector<int>fixedSteps = quilavaSteps;
    for (unsigned int i = 0; i < quilavaSteps.size(); i++)
    {
        quilavaSteps.at(i) = fixedSteps.at(i) + stepsOffset;
    }
    

    for (unsigned int i = 0; i < inputValues.size(); i++)
    {
        frames = inputValues.at(i);
        //Visual Labelling
        if (trackBG){
            if (inputValues.at(i) == hiFrame){
                frameLabel = "HIGH";
            } else if (inputValues.at(i)==loFrame){
                frameLabel = "LOW";
            } else if (inputValues.at(i)==xframe){
                frameLabel = "X --";
            } else if (inputValues.at(i)==yframe){
                frameLabel = "Y --";
            } else {
                frameLabel = "OTHER --- ";
            }
        }
        //Actual Filtering
        if (gameRegion == USA){
            //PRE-PATTERN
            if (i<=2 && i % 3 == 2){
                filterAmt = loFrame;
               //comment = "xLow";
            } else {
                filterAmt = hiFrame;
               //comment = "xHigh";
            }
            //MAIN NOISE PATTERN
            if (i>2){
                if (i % 5 == 0 || i % 5 == 2){
                    filterAmt = loFrame;
                    //comment = "xLow";
                } else {
                    filterAmt = hiFrame;
                    //comment = "xHigh";
                }
            }
            if (std::binary_search(quilavaSteps.begin(),quilavaSteps.end(),i)){
                filterAmt += 2;
            }
        }
        frames -= filterAmt;
        if (frames == 0){
            frameLabel = "";
        }
    outputData << std::left << std::setw(10) << frameLabel << " - "<<std::setw(4) << frames << " - " << comment << std::endl;
    }


    return 0;
}