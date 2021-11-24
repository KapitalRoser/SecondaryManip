#include "../processCore.h"



float combineDistance (float distanceX, float distanceY){
    float polynomial = 0;
    float fractional = 0;
    float C = 2500; //This is the distanceX(right), where does this come from?
    float result = 0;
    distanceX *= distanceX; //square
    std::cout << "SqX: "<< distanceX << "\n";
    distanceY *= distanceY;
    C *= C;
    polynomial = distanceX + distanceY + C; // A^2 + B^2 + C
    fractional = 1 / sqrt(polynomial);

    result = -(fractional*fractional * polynomial - 3);
    result = result * fractional * polynomial;
    std::cout << "RESULT: " << result << std::endl;
    return result;

    float A = 0;c d
    float B = 0;
    result = -((1/sqrt(A^2 + B^2))^2 * ((A^2 + B^2)  - 3) * ((1/sqrt(A^2 + B^2)) / 2) *(A^2 + B^2)
}



int main(){
    float preStepDistance = 0;
    float postStepDistance = 0;
    //from standard distances, expect 15.000000953674316 for before step and 14.709676742553711 for after step.
    float distanceX = 15.811;
    float distanceY = -34;
    int i = 0;
    while(true){
        preStepDistance = combineDistance(distanceX,distanceY); 
        // should be equal to post step distance from before
        //apply step()
        distanceX = abs(distanceX) - 0.27577400207519531;
        distanceY = abs(distanceY) - 0.090751789510250092;

        postStepDistance = combineDistance(distanceX,distanceY);
 
        if (postStepDistance <= preStepDistance && i < 10){
            i++;
            std::cout << "Step reduced distance, continue!\n";
        } else {
            std::cout << "Step did not reduce distance, stop!\n";
            break;
        }

    }


   


    return 0;
}