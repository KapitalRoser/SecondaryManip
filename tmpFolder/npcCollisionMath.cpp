#include "processCoreLocal.h"
#include "NPC.h"
#include <functional>




std::vector<std::byte> readFile(const std::string& filename) {
    std::ifstream inputFile(filename, std::ios_base::binary);
    inputFile.seekg(0, std::ios_base::end);
    auto length = inputFile.tellg();
    inputFile.seekg(0, std::ios_base::beg);
    std::vector<std::byte> buffer(length);
    inputFile.read(reinterpret_cast<char*>(buffer.data()), length);
    inputFile.close();
    return buffer;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
std::vector<std::byte> getXBytesAtX(const std::vector<std::byte>& data, size_t length, size_t offset) {
    return std::vector<std::byte>(data.begin() + offset, data.begin()+offset+length);
}

float getFloatFromWord(u32 word){ //This doesn't need to exist.
    float result;
    memcpy(&result,&word,sizeof(float));
    return result;
}

u32 swapEndiannessForWord(const std::vector<std::byte>& bytes, int result_Size = 4) {
    u32 swappedValue = 0;
    for (size_t i = 0; i < result_Size; ++i) {
        swappedValue |= static_cast<u32>(bytes[i]) << (8 * (result_Size - 1 - i));
    }
    return swappedValue;
}
u32 getWord(const std::vector<std::byte>&data,int offset){
    std::vector<std::byte> slice = getXBytesAtX(data,4,offset);
    return swapEndiannessForWord(slice); 
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//SampleTri needs work, the print functions in printMeshCopy can/should be integrated
//Goal is to simply read file and organize into objects. Hopefully the sampling process can be organized as well. I hate this weird pointer shit.

enum DIMENSION {X = 0, Z = 1, Y = 2};

struct tri {
    std::vector<d_coord> points;
    std::vector<float> normals;
    int interactionFlag; //looks like we need the first short? Idk if there's a good example of the 2nd short ever being not 0
};
std::vector<double> tri_GetAllByDimension (const tri& tri, DIMENSION D){
    //X = 0, Z = 1, Y = 2
    //little inefficient with lines but honestly saves having multiple functions.
    std::vector<double> res;
    for (auto &&i : tri.points)
    {
        double resD;
        switch (D)
        {
        case X:
            resD = i.x;
            break;
        case Z:
            resD = i.z;
            break;
        case Y:
            resD = i.y;
            break;
        default:
            resD = i.x;
            break;
        }
        res.push_back(resD);
    }
    return res;
}


class sample
{ //This obj is what ptrA represents
    private:
    int idx; //offset used for ptrB. 
    int n; //number of tris as part of the sample run.
    public:
    sample(const std::vector<std::byte>&data,int addr){
        idx = getWord(data, addr);
        n = getWord(data,addr+0x4);
    }
};


class collision_obj { //refactor to "Collider"?
    private:
    int fileOffset;
    int num_tris;
    std::vector<tri>tris;
    double originX;
    double originY; //no z needed.

    //these have something to do with the point sampling system
    int end_offset; //only used in building the vectors
    int buffer_start;

    std::vector<int>sampleOrder; //The buffer_start all the way to the next object. 
    std::vector<sample> sampleStarts; //need to implement construction for these vectors.

    int xUpperLim;
    int yUpperLim;
    float scaleX;
    float scaleY;
    public:
    collision_obj(const std::vector<std::byte>&data,int addr){
        fileOffset = getWord(data,addr);
        num_tris = getWord(data,addr+0x4);
        end_offset = getWord(data,addr+0x8); //forms ptrA
        buffer_start = getWord(data,addr+0xC); //forms ptrB
        xUpperLim = swapEndiannessForWord(getXBytesAtX(data,2,addr+0x10),2);
        yUpperLim = swapEndiannessForWord(getXBytesAtX(data,2,addr+0x12),2);//Halfword
        
        scaleX = getWord(data,addr+0x14); //Scale? In both files, every object has this set to 40.
        scaleY = getWord(data,addr+0x18); //Scale? Same, 40.
        originX = getWord(data,addr+0x1C); //object_pointer[7] etc. -- OBJECT'S PERSONAL ORIGIN X
        originY = getWord(data, addr+0x20); //OBJECT ORIGIN Y. UPPER LEFT CORNER.

        for (int currentSet = end_offset; currentSet < buffer_start; currentSet+=0x8){
            sampleStarts.push_back(sample(data, currentSet));
        }
        //only way to build sample order without knowing the next offset is to run each sample in ptrA. 
        //if I build all possible samples, that could get messy and ram intensive across many many objects. 
        //Might just be more convienient if we read the data by feeding in the next offset and dealing with EOF stuff for the final object. 

    }
    //Maybe include buffer zone data too... for point ordering..
};

//assimilate this into collision object later...


typedef std::function<bool(d_coord&,const tri&,const int,d_coord)> collEvalFn;

//Fixed, good.
d_coord getCpPlanePoint(std::vector<float> tri_orientation,d_coord tri_pos_ptr,d_coord adjX_ptr_maybe){ //this assumes the params mean what I think they mean, still testing.
    float orientation_A = tri_orientation[0]; //Once again feel the need to make this a tuple of size 3 for easier manipulation, but it isn't a position so d_coord isn't proper.
    float orientation_B = tri_orientation[1];
    float orientation_C = tri_orientation[2];

    d_coord subRes = vectorSub(tri_pos_ptr,adjX_ptr_maybe);
    double factor = (orientation_A * subRes.x) + (orientation_B*subRes.z) + (orientation_C*subRes.y);
    double divisor = (pow(orientation_C,2)+ pow(orientation_A,2) + pow(orientation_B,2));  //can add a square vector for the divisor, need to keep this multiplication as is because scale uses a common term.

    double scaleFactor = factor/divisor;
    d_coord scaleResult;
    scaleResult.x = scaleFactor * tri_orientation[0];
    scaleResult.z = scaleFactor * tri_orientation[1];
    scaleResult.y = scaleFactor * tri_orientation[2];
    return vectorAdd(scaleResult,adjX_ptr_maybe); //Save first
}
double getCpLinePoint(d_coord& result_location, d_coord tri_pos_ptr,d_coord successivePoint, d_coord adjX){ //unlike the other one, this can return a separate value.
    d_coord subResult = vectorSub(successivePoint,tri_pos_ptr);
    double intermediateResult = 0.0;
    if (vectorSquareMag(subResult) == intermediateResult){
        result_location = tri_pos_ptr;
    } else {
        intermediateResult = 
        (subResult.y * (adjX.y - tri_pos_ptr.y)) + 
        (subResult.z * (adjX.z - tri_pos_ptr.z)) + 
        (subResult.x * (adjX.x - tri_pos_ptr.x));

        d_coord scaleResult = vectorScale(intermediateResult,subResult);
        result_location = vectorAdd(scaleResult,tri_pos_ptr);
    }
    return intermediateResult;
}
d_coord getPointExtensionLine(const double adjustedBallSize,d_coord position_at_collision,d_coord adjX){
    d_coord intermediate = vectorSub(adjX,position_at_collision);
    double mag = vectorMagnitude(intermediate);
    intermediate = vectorScale((adjustedBallSize / mag), intermediate);
    return vectorAdd(intermediate,position_at_collision); //return this instead of void return.
}
double getSidePlanePoint(std::vector<float>tri_orientation,d_coord tri_pos_ptr,d_coord adjX_ptr){
    return (double)(tri_orientation[0] * (adjX_ptr.x - tri_pos_ptr.x) +
                    tri_orientation[1] * (adjX_ptr.z - tri_pos_ptr.z) +
                    tri_orientation[2] * (adjX_ptr.y - tri_pos_ptr.y));
}

int myChkInTri(d_coord adjX_Data_ptr, const tri& tri_pointer){

    //most of the doubles in this function are floats. Need to convert from a d_coord to a vector of floats. 
    //This should take in a tri object. This fn picks two of the 3 values from the points (Either all 3 X's all 3 Y's or all 3 Z's).
    //Use std::signbit(-0) from math.h for the neg zero check. returns true if NEGATIVE.
    std::vector<float> logicVec = tri_pointer.normals;
    for (auto &&i : logicVec)
    {
        //if i is negative, std::signbit returns true.
        i = std::signbit(i) ? -i : i;
    }
    float a = logicVec[0];
    float b = logicVec[1];
    float c = logicVec[2];
    //luckily this means neg zero isn't a concern for the next logic block.

    //logic is done on the COPIES, ADJUST TO C IS DONE ON THE ACTUAL. Changes are not saved to tri, just local :) Picks axis of tri.
    DIMENSION flag_0_2 = X; //or 2
    DIMENSION flag_0_1 = Z; //or 1
    float reverseFlag = tri_pointer.normals[2];

    if ((b <= a) && (a > c)){
        flag_0_2 = Y;
        flag_0_1 = Z;
        reverseFlag = -tri_pointer.normals[0];
    } else if (c <= b){
        flag_0_2 = Y;
        flag_0_1 = X;
        reverseFlag = tri_pointer.normals[1];
    }

    std::vector<double> adjX_Floats = {adjX_Data_ptr.x,adjX_Data_ptr.z,adjX_Data_ptr.y}; //temp, come up with a more direct d_coord to value setup with the other.
    double adjX_val_A = adjX_Floats[flag_0_2];
    double adjX_val_B = adjX_Floats[flag_0_1];


    std::vector<double> A_Values = tri_GetAllByDimension(tri_pointer,flag_0_2);
    std::vector<double> B_Values = tri_GetAllByDimension(tri_pointer,flag_0_1);

    if (!std::signbit(reverseFlag)){ //check positive
        std::reverse(A_Values.begin(),A_Values.end());
        std::reverse(B_Values.begin(),B_Values.end());
    }

    double tri_min_bound_A = *std::min_element(A_Values.begin(),B_Values.end()); //default 1 million ingame if this min/max fails.
    double tri_max_bound_A = *std::max_element(A_Values.begin(),A_Values.end());
    double tri_min_bound_B = *std::min_element(B_Values.begin(),B_Values.end());
    double tri_max_bound_B = *std::max_element(B_Values.begin(),B_Values.end());


    if  (!(adjX_val_A > tri_min_bound_A) && (adjX_val_A < tri_max_bound_A) &&
          (adjX_val_B > tri_min_bound_B) && (adjX_val_B < tri_max_bound_B)) {
        return 0;
    }


    //could split this up into variable names or a function but may be less readable than this.
    if(    
        (   (A_Values[1] - A_Values[0]) * (adjX_val_B - B_Values[0]) -
            (B_Values[1] - B_Values[0]) * (adjX_val_A - A_Values[0]) <= 0.0
        ) 
            &&
        (
            (A_Values[2] - A_Values[1]) * (adjX_val_B - B_Values[1]) -
            (B_Values[2] - B_Values[1]) * (adjX_val_A - A_Values[1]) <= 0.0
        )
            &&
        (
            (A_Values[0] - A_Values[2]) * (adjX_val_B - B_Values[2]) -
            (B_Values[0] - B_Values[2]) * (adjX_val_A - A_Values[2]) <= 0.0
        )
    ){
        return 1;
    }

    return 0; //all other cases.
}

d_coord nudgePos(int ballSize, d_coord position_at_collision, d_coord AdjX){
    //could merge the ballSize adjustment into getPointExtensionLine?
    double ballAdjustment = ballSize + 0.00009999999747378752; // in the code: value: 0x38d1b717
    return getPointExtensionLine(ballAdjustment,position_at_collision,AdjX);
}

//my fn -- returns posAtCol and a bool which becomes didCollisionOccur
bool innerFoo0(d_coord &posAtCol, const tri &currentTri, const int ballSizeSquared, d_coord AdjX){ //no changes to adjX here.
    if (getSidePlanePoint(currentTri.normals,currentTri.points[0],AdjX) >= 0){ //shockingly simple. This signature and the next one are fine as is. Only shorten the chkInTri signature to take the whole tri. The other two fns make it clearer what data they're using by having separate parameters.
        d_coord cpPlaneResult = getCpPlanePoint(currentTri.normals,currentTri.points[0],AdjX); //CP IS INFLUENCED BY adjX -- does not return anything normally, but I may choose to alter this to be better style.
        if ((vectorSquareDistance(cpPlaneResult,AdjX) < ballSizeSquared) && myChkInTri(AdjX,currentTri) != 0) { //if the distance squared from the proposed to the CP is less than the squared ball size then begin detailed tri checking.
                posAtCol = cpPlaneResult; //save
                return true;
        }
    }
    return false;
} //Seems to check the first point in detail?

//Investigate val(triPtr+30) as well as local_d0 and such. May help with the structure of the logic.
bool innerFoo1(d_coord &posAtCol, const tri& currentTri, const int ballSizeSquared, d_coord AdjX ){
    if ((currentTri.interactionFlag & 7)!= 0){
        double planePoint_result = getSidePlanePoint(currentTri.normals,currentTri.points[0],AdjX); //oo reused from foo0
        if (planePoint_result >= 0){
            const std::vector<int> ptrC_vals = {1,2,4}; //hardcoded in the game. 
            for (int j = 0; j < 3; j++) //J IS EQUIVALENT TO tri_point_idx. However not sure if that makes sense in the way that ivar2/funkyoffset is used.
            {
                if ((currentTri.interactionFlag & ptrC_vals[j]) != 0){ //The only place that local_d0 gets used?
                    int successivePoint = j > 1 ? 0 : j+1; //I SUSPECT THAT FUNKY OFFSET REFERS TO ANOTHER POINT IN THE SAME TRI.
                    d_coord cpLineResult; //There is way too much code to try and consolidate these into one return value.
                    double cpLineIntermediate = getCpLinePoint(cpLineResult,currentTri.points[j],currentTri.points[successivePoint],AdjX);
                    if (cpLineIntermediate >= 0.0 && 
                        cpLineIntermediate <= 1.0 && //Is this in the ghidra code?? kinda weird.
                        vectorSquareDistance(cpLineResult,AdjX) < ballSizeSquared){
                        posAtCol = cpLineResult;
                        return true;
                    }
                }
            }
        }
    }
    return false;
} //seems to check all points?


//REMARKABLY SIMILAR TO INNERFOO1 WITH TINY DIFFERENCE IN THE FINAL CHECK
bool innerFoo2(d_coord& posAtCol, const tri& currentTri, const int ballSizeSquared, d_coord AdjX){
    if ((currentTri.interactionFlag & 7)!= 0){ //same condition check as innerFoo1. Could abstract out of the function. This typically returns true I think... Its the first interaction flag checked.
        double planePoint_result = getSidePlanePoint(currentTri.normals,currentTri.points[0],AdjX); //verify that it really is just the first point used here.
        if (planePoint_result>= 0){
            const std::vector<int> ptrC_vals = {1,2,4}; //hardcoded 
            for (int j_2 = 0; j_2 < 3; j_2++)
            {
                //The contents of this loop are the main diff between this and the prev function so there's potential to avoid duplicating the outside logic but cmon there's no need to be that pedantic.
                int antecedentPoint = j_2 < 1 ? 2 : j_2 - 1;  //NO CP GETTING HERE....
                if (((currentTri.interactionFlag & ptrC_vals[j_2]) != 0) && 
                    ((currentTri.interactionFlag & (ptrC_vals[j_2] + antecedentPoint * 2)) != 0) && //really trusting the decomp on this
                    (vectorSquareDistance(currentTri.points[j_2],AdjX) < ballSizeSquared)){
                    posAtCol = currentTri.points[j_2];
                    return true;
                }
            }
        }
    }
    return false;
}


bool runSample(d_coord&position_at_collision,sample smp,collision_obj objptr,const int ballSizeSquared, d_coord AdjX, collEvalFn evaluationPredicate){
    for (int i = 0; i < smp.n; i++){ //Run a given sample series of length N. This is a chain of tris in a predetermined order in sampleOrder.
        int currentTri_idx = objptr.sampleOrder[smp.idx]; //Lookup the index of a tri. Could oneline this but my brain hurts.
        tri currentTri = objptr.tris[currentTri_idx]; //Assuming that I will fill up objptr.tris in the order in which the tris are recorded to file.            
        bool didCollisionOccur = evaluationPredicate(position_at_collision,currentTri,ballSizeSquared,AdjX); //using the evaluation predicate, evaluate the tri for collision. If not, continue to the next tri in sampleOrder, until the limit of the sample is reached (as defined by N)
        if (didCollisionOccur){
            return true;
        }
        smp.idx++;
    }
    return false;
}
//This could use a llittle bit of work. It would be cool to have a better system for ptrA and ptrB than doing weird pointer math on a std::vec of bytes.
bool searchTriSample(d_coord&position_at_collision, int xMinusBall, int yMinusBall, int xPlusBall, int yPlusBall, collision_obj objptr, const int ballSizeSquared, d_coord AdjX, collEvalFn evaluationPredicate){
    //While technically this can work as a single function, it would make things cognitively easier to understand if it is broken up. Might be able to recombine things in the future to have less vectors if I'm smart.
    //ptrA refers to a idx and a size within an array of sampleOrder. The values in sampleOrder refers to an idx within the array of Tris for a given Object. The Size is the number of values to be sampled. 
    for (int ymbc = yMinusBall; ymbc < yPlusBall; ymbc++){ //for each ymbc, calculate a new start for a set of samples, each one is xUpperLim apart, offset by xMinusBall. 
        int sampleSelectionIdx = objptr.xUpperLim * ymbc + xMinusBall; //starting point for a set of samples.
        for (int xmbc = xMinusBall; xmbc < xPlusBall; xmbc++){ //using that starting point, run a series of samples. 
            sample currentSample = objptr.sampleStarts[sampleSelectionIdx];
            if (runSample(position_at_collision,currentSample,objptr,ballSizeSquared,AdjX,evaluationPredicate)){ //Run a given sample series of length N. This is a chain of tris in a predetermined order in sampleOrder.
                return true;
            }
            sampleSelectionIdx++;
        }
    }
    return false;
}


bool myCheckFixMdl(int ballSize, d_coord& AdjX, int* object_pointer, d_coord& result_storage){
    bool resultFlag;
    d_coord position_at_collision;

    //This could really be done on .ccd file read, where a vector of objects is prepared with all this data populated and organized.
    collision_obj objptr;
    objptr.fileOffset = (int)object_pointer;
    objptr.num_tris = object_pointer[1];
    objptr.end_offset = object_pointer[2]; //forms ptrA
    objptr.buffer_start = object_pointer[3]; //forms ptrB?
    objptr.xUpperLim = *(object_pointer+0x10); //Halfword
    objptr.yUpperLim = *(object_pointer+0x12);//Halfword
    
    objptr.scaleX = object_pointer[5]; //Scale? In both files, every object has this set to 40.
    objptr.scaleY = object_pointer[6]; //Scale? Same, 40.
    objptr.originX = object_pointer[7]; //object_pointer[7] etc. -- OBJECT'S PERSONAL ORIGIN X
    objptr.originY = object_pointer[8]; //OBJECT ORIGIN Y. UPPER LEFT CORNER.
    
    //PUT VECTOR BUILDS HERE. 


    //This needs to be done on the fly
    //In the game these are narrowed down intentionally into int.
    //We can probably put most of this into search tri sample and remove the extra parameters!
    int xMinusBall = (AdjX.x - ballSize - objptr.originX)/objptr.scaleX;
    int yMinusBall = (AdjX.y - ballSize - objptr.originY)/objptr.scaleY;
    int xPlusBall  = (AdjX.x + ballSize - objptr.originX)/objptr.scaleX;
    int yPlusBall  = (AdjX.y + ballSize - objptr.originY)/objptr.scaleX;

    // min and max limits.
    xMinusBall = xMinusBall < 0 ? 0 : xMinusBall; //Since scaleX and scaleY are so big (40, 50 etc.) this should always be a small number, and since its an int its pretty much always gonna be between 0, 1 or 2. 3 or higher should be exceedingly rare.
    yMinusBall = yMinusBall < 0 ? 0 : yMinusBall;

    xPlusBall = xPlusBall > (objptr.xUpperLim - 1) ? objptr.xUpperLim-1 : xPlusBall;
    yPlusBall = yPlusBall > (objptr.yUpperLim - 1) ? objptr.yUpperLim-1 : yPlusBall;


    int ballSizeSquared = pow(ballSize,2);
    
    
    std::vector<collEvalFn> evaluationFns = {innerFoo0,innerFoo1,innerFoo2};
    for (collEvalFn fn : evaluationFns){
        if (searchTriSample(position_at_collision,xMinusBall,yMinusBall,xPlusBall,yPlusBall,objptr,ballSizeSquared,AdjX,fn)){
            result_storage = nudgePos(ballSize,position_at_collision,AdjX);
            return true;
        }  
    }
    return false;
    //phuck yes
}

std::vector<collision_obj> setupMapData(std::string filename){
    //call this at the start of the application start. 

    std::vector<std::byte> data = readFile(filename); //include .ccd
    std::vector<collision_obj> res;
    int file_start = getWord(data,0x0); //returns 0x10 for our shop2f example..
    int entry_count = getWord(data,0x4);
    std::vector<int> offsetList;
    int workingSectionPtr = file_start;
    //this is a better system than us hardcoding 0x38 and counting byte by byte. Correctly skips some of the duplicate objects I think
    for (int j = 0; j < entry_count; j++)
    {
        int datum = getWord(data,workingSectionPtr + 0x28);
        if (datum != 0){
            offsetList.push_back(datum); //objects are pointed to in the order they're written. thank god. 
            collision_obj colObj = collision_obj(data,datum);
            res.push_back(colObj);
        }
        workingSectionPtr+=0x40;
    }
    
    return res;
}



bool myCheckHitCollision(int collisionBallSize, d_coord pointDiffs, d_coord &result_storage_location){
    //This ignores the 10x redundancy check on collisions. Assuming that my understanding of the loop is correct. 
    std::vector<collision_obj> mapCollisionData = setupMapData("M1_shop_2F.ccd"); //can maybe be included in a greater map object which has background noise and npc stuff in there too?
    d_coord resultPosTmp;
    for (collision_obj collider : mapCollisionData) //should be === the entry_count or less.
    {
        if (myCheckFixMdl(collisionBallSize,pointDiffs,collider,resultPosTmp)){
            result_storage_location = resultPosTmp; //save to resultStorageLocation only if collision occurs.
            return true;
        }
    }
}

bool checkHitCollision(int collisionBallSize,d_coord store_A, d_coord store_B,d_coord &result_storage_location)
{
        //if there's a collision, double check up to 10x.
    //If no collision, exit and don't save anything.
    //Honestly, why check 10x? Is that really going to change the result?
    //This func will be the first to read the collision data.
    // std::vector<std::byte> ccd_DATA; //from .ccd file.

    d_coord collision_calc_result_location_ptr;
    int *object_ptr;
    int section_ptr; //Start of the section data. 
    int objEnableResult;

                /* Where is store_A even used????? It seems like store_B is
                    the only thing that is called/passed in this function. */
    d_coord AdjX = store_B;

    int* list_start = 0; //GETS START OF CCD DATA
    //int file_start = getWord(data,0x0);

    int firstLim = 0; //num collisions?
    int i = 0;
    do {
        section_ptr = *list_start; //literal data at file_start. Ex. 0x10 Typically the beginning of the block of sections which contain the offsets to the objects.
        firstLim = 0;
        unsigned int entry_count = list_start[1]; //same as in our parser.
        for (unsigned int section_j = 0; section_j < entry_count; section_j++) {
            object_ptr = *(int **)(section_ptr + 0x28); //get an address to an object.
            if (object_ptr != (int *)0x0) { //exist? continue to check. else skip.
                if (myCheckFixMdl(collisionBallSize,AdjX,object_ptr, collision_calc_result_location_ptr) != 0) { //This picks an object for the rest of the code tosample from. This will contain a number of tris and stuff.
                    firstLim = firstLim + 1;
                    AdjX = collision_calc_result_location_ptr;
                }
            }
            section_ptr += 0x40;
        }
    } while ((0 < firstLim) && (i++, i < 10));
    if (0 < i) {
        result_storage_location = AdjX;
    }
    return 0 < i;
}


int my_Collision(int ballSize, d_coord& adjPosLoc, d_coord old){ 
    d_coord proposed = adjPosLoc;
    d_coord point_diffs = vectorSub(proposed,old); //Can compress further if needed.
    return myCheckHitCollision(ballSize,point_diffs,adjPosLoc);
}

//my ver of peopleAdjPosition
d_coord AdjustIfCollide(int ballSize, d_coord proposed, d_coord old){
    d_coord adjusted = proposed;
    if (my_Collision(ballSize,adjusted,old)){
        return adjusted; //This format plays slightly safer, only making the modification if GS_Collision returns 1.
    } //Need to see if GS_Collision modifies position if there is no collision. Otherwise just return the adjusted paramet of GS Collision.
}


int main(){

    //BUILD ALL THE COLLISION OBJECTS:

    








    d_coord old;
    d_coord proposed; //To be modified if applicable.
    int NPC_BALLSIZE = 3; //not const? I think it gets added to with TinyAdjustment later...
    proposed = AdjustIfCollide(NPC_BALLSIZE,proposed,old);
    //run thru stop check adn then apply.
    return 0;
}
