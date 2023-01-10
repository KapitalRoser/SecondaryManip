
//RELATED TO NPCs
class duration {
    public:
    //This is super important to get right, as the numbers produced here will directly impact rng manip in the future!
    duration(float m_seconds);
    duration(int inputframes30);
    //Shouldn't ever call duration with a 60fps number, right? if so then add a /2 in the parameter.
    //both XD and Colo have 30fps overworlds and there shouldn't be a reason in XD to ever track npcs in the pause menu.
    float getFrames60fromSeconds();
    
    int getFrames30();
    int getFrames60();
    float getSeconds();
    void setFrames30(int input);
    void setSeconds(float input);
    private:
    float m_seconds = 0;
    int m_frames30 = 0;
};
