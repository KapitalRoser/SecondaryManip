#include <iostream>
#include <vector>
// guide used: https://www.youtube.com/watch?v=RSXM9bgqxJM
class edge{
    //contains 2 coords, assumed by inclusion that they are connected.
    public:
    int x1; //left
    int y1; //bottom
    int x2;
    int y2;
};

// Apply x step, if rejected, place x on edge. Then apply y, if rejected, place on edge. Will create slide effect.

bool isInside(std::vector<edge>edges,int xp, int yp){
    int counter = 0;
    edge edg = edges[0];
    
    //works for straight line edges, what about the curve that happens in phenac?
    for (int i = 0; i < edges.size(); i++){
        int offset = edg.x1; //how far in the polygon lies from the left barrier of the canvas.
        int edgeHeight = edg.y2 - edg.y1;
        int edgeWidth = edg.x2 - edg.x1;
        int pointHeight = yp - edg.y1;
        int pointWidth = xp - edg.x1;
        int heightRatio = pointHeight / edgeHeight; //Ratio of height works for straight edge but what about a curved one?
        int greatestXThatCrosses = offset + heightRatio * edgeWidth;
        if (yp > edg.y1 && yp < edg.y2 && xp < greatestXThatCrosses){
            counter++;
        }
    }

    return counter % 2; //Odd is inside, even is outside.
}

int main(){







    return 0;
}