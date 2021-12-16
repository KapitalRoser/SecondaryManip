#include "../processCore.h"
#include "../NPC.h"
#include "coloCore.h"

//This needs serious work if it's even worth it.

class frame{
    public:
    //constructor
    frame(region r)
    :m_Region(r)
    {}
    frame(std::vector<NPC>inputNPC,region r)
    :m_Region(r){
        setNPCS(inputNPC);
    }
    frame(std::vector<int>inputStepList,region r)
    :m_Region(r){
        setSteps(inputStepList);
    }
    frame(std::vector<NPC>inputNPC,std::vector<int>inputStepList,region r)
    :m_Region(r){
        setNPCS(inputNPC);
        setSteps(inputStepList);
    }

    int updateFrame(u32 &seed, int i){
        //initialize
        setI(i);
        m_totalCalls = 0;
        if (m_i == 0 && m_npcsActive){
            for (unsigned int j = 0; j < m_npcSet.size(); j++) {
               m_npcSet[j].initializeNPC_Self(seed);
            }
        }

        //first, bg
        m_totalCalls = rollBackground(seed);

        //then, npcs
        if (m_npcsActive){
            for (unsigned int j = 0; j < m_npcSet.size(); j++){
                m_npcSet[i].npcAction_Self(seed,m_i);
            }
        }

        //finally, step.
        if (m_StepActive){
            m_totalCalls += checkStepPath(seed);
        }
        return m_totalCalls;
    }

    int checkStepPath(u32&seed){
        if (binary_search(m_stepList.begin(),m_stepList.end(),m_i+1)){
            LCGn(seed,m_callsPerStep);
            return m_callsPerStep;
        } else {
            return 0;
        }
    }
    int rollBackground(u32 &seed){
        return LCGn(seed,col_consultPattern(m_i,m_Region));
        //may be able to use a function pointer to pass consultpattern as an argument
        //without having to store all the data inside this class.
    }
    
    // void setVisFrame(int input){m_visualFrame = input;}
    void setI(int i){m_i = i;}
    void setSteps(std::vector<int>input){
        m_stepList = input;
        m_StepActive = true;
    }
    void setNPCS(std::vector<NPC>input){
        m_npcSet = input;
        m_npcsActive = true;
    }

    //others?
    // int getVisFrame(){return m_visualFrame;}
    int getIthFrame(){return m_i;}
    int getCalls(){return m_totalCalls;}


    private:
    int m_visualFrame = 0;
    int m_i = 0; //ith frame in loop, game logic.
    bool m_StepActive = 0;
    int m_callsPerStep = 0;
    bool m_npcsActive = 0;
    region m_Region = NTSCU;
    int m_totalCalls = 0;
    //do I bake NPC stuff into the frame?
    std::vector<NPC>m_npcSet;
    std::vector<int>m_stepList;
};