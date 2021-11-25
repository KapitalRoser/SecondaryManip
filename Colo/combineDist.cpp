#include "../processCore.h"



float combineDistance (float distanceX, float distanceY){

    //Decent function, can occasionally round a bit off, has something to do with the precision of 1/sqrt(sumSq),
    //but not sure how to fix. Can result in exactly 1 byte being off
    //ex: 41700001 vs 41700000
    //does double vs float make things more or less accurate?
    double sumSq = (distanceX*distanceX) + (distanceY*distanceY);
    double fractional = 1/sqrt(sumSq);
    double fractionalSq = fractional*fractional;
    double result = -(fractionalSq*sumSq-3) * (fractional/2) * sumSq;
    std::cout << "RESULT: " << std::setprecision(16) << result << std::endl;
    return result;

    //result = -((1/sqrt(A^2 + B^2))^2 * ((A^2 + B^2)  - 3) * ((1/sqrt(A^2 + B^2)) / 2) *(A^2 + B^2)
    /*
    
    */
}



int main(){
    float preStepDistance = 0;
    float postStepDistance = 0;
    float destinationX = -10.248325347900391;
    float destinationY = 28.688840866088867;
    float currentPosX = 4;
    float currentPosY = 24;
    //from standard distances, expect 15.000000953674316 for before step and 14.709676742553711 for after step.
    float distanceX = -14.248325347900391;
    float distanceY = 4.688840866088867;
    int i = 0;
        do
        {
        i++;
        
        preStepDistance = combineDistance(distanceX,distanceY); 
        distanceX = abs(distanceX) - 0.27577400207519531; //factor x2 gets correct number of frames.
        distanceY = abs(distanceY) - 0.090751789510250092; //occasionally, step gets applied twice?
        // if (i == 1){//Why are some steps skipped?
        //     distanceX = abs(distanceX) - 0.27577400207519531;
        //     distanceY = abs(distanceY) - 0.090751789510250092; 
        // }
        postStepDistance = combineDistance(distanceX,distanceY);
        
        std::cout << "Step reduced distance, Continue!\n";
        } while (postStepDistance <= preStepDistance);
        std::cout << "Step did not reduce distance, Stop!\nSteps taken: " << i;
        


   


    return 0;
}