#include <iostream>
#include <math.h>
bool isWithinCircle(double inputX,double inputY, double circleCentreX,double circleCentreY, double radius){
    //If point is perfectly on edge, consider valid position, since that's what we will be resetting that x or y value to anyway.
    double distX = inputX - circleCentreX;
    double distY = inputY - circleCentreY;
    double distToCentre =  sqrt(pow(distX,2) + pow(distY,2));
    if (distToCentre == radius){
        std::cout << "ON EDGE!\n";
    }
    return distToCentre < radius;
}

//Once we know that the point is within the circle, we need to decide how to deal with it. 
//for any given x (or y) there are two points that can be considered as the proper edge.

// Y minus == Left
// X minus == bottom

//To flip angle, to opposite 
double moveX(double x, double y,double circleX, double circleY, double r, int quadrantChoice = 0){
    /*sets a given x or y to its correct point on the edge of the circle.
    x = h +- sqrt(r^2 - (y-k)^2)
    
    (x-h)^2 = -(y - k) ^2 + r^2 
    x-h = sqrt(-y-k)

    */
   double mutatedX = sqrt(pow(r,2) - pow(y - circleY,2));
   double mutatedY = sqrt(pow(r,2) - pow(x - circleX,2));
   std::cout << "\nval: " << mutatedX << std::endl;
   if (quadrantChoice){ 
    mutatedX = circleX + mutatedX;
    mutatedY = circleY + mutatedY;
    std::cout << "POS: " << mutatedX;
   } else {
    mutatedX = circleX - mutatedX;
    mutatedY = circleY - mutatedY;
    std::cout << "NEG: " << mutatedX;
   }
   return 0;
}



int main(){

    double circleX = 10, circleY = 10;
    double radius = 3;
    double pX = 10, pY = 7;
    double p2X = 11,p2Y = 9;
    std::cout << "\n1:" << std::boolalpha << isWithinCircle(pX,pY,circleX,circleY,radius);
    std::cout << "\n2:" << isWithinCircle(p2X,p2Y,circleX,circleY,radius);
    std::cout << std::endl;

    moveX(p2X, p2Y,circleX,circleY,radius,0);
    moveX(p2X, p2Y,circleX,circleY,radius,1);

    return 0;
}