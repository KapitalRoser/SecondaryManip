#include "../../processCore.h"
#include "duration.h"
#include "refactoredNPC.h"
NPC::NPC(d_coord anchor){
    setAnchor(anchor);
    f_coord temp;
    temp.x = anchor.x;
    temp.y = anchor.y;
    setNextPos(temp);
    setSpeed(walkingSpeed[STANDARD]);
}
    NPC::NPC(d_coord anchor, commonSpeed speed){
    setAnchor(anchor);
    f_coord temp;
    temp.x = anchor.x;
    temp.y = anchor.y;
    setNextPos(temp);
    setSpeed(walkingSpeed[speed]);
}
NPC::NPC(d_coord anchor, commonSpeed speed, int ID){
    setAnchor(anchor);
    f_coord temp;
    temp.x = anchor.x;
    temp.y = anchor.y;
    setNextPos(temp);
    setID(ID);
    setSpeed(walkingSpeed[speed]);
}
NPC::NPC(d_coord anchor, commonSpeed speed, std::string name){
    setAnchor(anchor);
    f_coord temp;
    temp.x = anchor.x;
    temp.y = anchor.y;
    setNextPos(temp);
    setName(name);
    setSpeed(walkingSpeed[speed]);
}

NPC::NPC(d_coord anchor, commonSpeed speed = STANDARD, int ID = 0, std::string name = "NAME_NOT_SET") :
    m_anchor(anchor),
    m_nextPos({float(anchor.x),float(anchor.y)}), //returns valid f_coord?
    m_ID(ID),
    m_name(name),
    m_speedFactor(walkingSpeed[speed])
    {} //empty body! nice!
    //Only anchor should be mandatory, everything else is set to default values if not specified,
    //and all initialization is done in the list.
    //Once again, does the optional parameter need to be specified in the cpp or in the .h?
    //Qt makes me think .h but Im not positive.