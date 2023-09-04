#include "../../processCore.h"
#include "duration.h"

duration::duration(float m_seconds){
        setFrames30(round(m_seconds/(1.0/30))); //This only seems to matter when it comes to 60fps?
        setSeconds(m_seconds); //The 60fps number would depend on whether on an even frame or odd frame, again shouldn't matter right?
}
duration::duration(int inputframes30){
        setSeconds((1.0/30)*inputframes30);
        setFrames30(inputframes30);
}

float duration::getFrames60fromSeconds(){
    return round(m_seconds/(1.0/60)); //need to do more testing to see if this is correct.
}

int duration::getFrames30(){return m_frames30;}
int duration::getFrames60(){return m_frames30*2;}
float duration::getSeconds(){return m_seconds;}

void duration::setFrames30(int input){m_frames30 = input;}
void duration::setSeconds(float input){m_seconds = input;}


