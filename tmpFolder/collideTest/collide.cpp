#include "../processCoreLocal.h"
#include "../NPC.h"
#include <functional>



std::vector<std::byte> readFile(const std::string& filename) {
    std::ifstream inputFile(filename, std::ios_base::binary);
    if (inputFile.fail())
    {
        std::cout << "File inaccessible";
        exit(EXIT_FAILURE);
    }
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

class tri {
    public:
    std::vector<d_coord> points;
    std::vector<float> normals;
    int interactionFlag; //looks like we need the first short? Idk if there's a good example of the 2nd short ever being not 0

    tri(const std::vector<std::byte>&data, int tri_offset){
            for (int i = 0; i < 3; i++)
            {
                d_coord pt;
                pt.x = getFloatFromWord(getWord(data,tri_offset+(i*0xC)));
                pt.z = getFloatFromWord(getWord(data,tri_offset+0x4+(i*0xC)));
                pt.y = getFloatFromWord(getWord(data,tri_offset+0x8+(i*0xC)));
                points.push_back(pt);
            }
            //now for normals
            normals = {
                getFloatFromWord(getWord(data,tri_offset+0x24)),
                getFloatFromWord(getWord(data,tri_offset+0x28)),
                getFloatFromWord(getWord(data,tri_offset+0x2C))};

            interactionFlag = swapEndiannessForWord(getXBytesAtX(data,2,tri_offset+0x30),2);//I call this an interaction_flag because that's what Stars has these labeled as. 
            //t.interactionFlag_2 = swapEndiannessForWord(getXBytesAtX(data,2,tri_offset+0x32),2); //not sure if this is needed. Doesn't show up in my collision stuff.
    }

};

class sample
{ //This obj is what ptrA represents
    private:
    int offset; //offset used for ptrB. 
    int n; //number of tris as part of the sample run.
    
    public:
    std::vector<int> sampleTris;
    sample(const std::vector<std::byte>&data,int offsetSelf, int buffer_start){
        offset = getWord(data, offsetSelf);
        n = getWord(data,offsetSelf+0x4); //technically don't need offset or N after the loop, since this sample object is just a glorified std::vector<int>
        for (int i = 0; i < n; i++){
            int tri = getWord(data,buffer_start+(offset*0x4)+(i*0x4)); //contains an index for the tri in the .tris vector of collision obj. meaningless without that. 
            sampleTris.push_back(tri); 
        }
    }
    int getN(){return n;} //Surely we can make the vars public, ik ik style yes yes, but cmon if someone is working with interface surely they know what they're doing right?
    int getOffset(){return offset;}
    
};

class collider { //refactor to "Collider"?
    private:
    
    int firstTri;
    int num_tris;
    std::vector<tri>tris; //only used once, in runSample()
    double originX;
    double originY; //no z needed.

    //these have something to do with the point sampling system
    int end_offset; //only used in building the vectors
    int buffer_start;

    std::vector<sample> samples; //need to implement construction for these vectors.

    int xUpperLim; //Clearly Num samples per sample group.
    int yUpperLim; //my first thought is this is number of sample groups but I could be wrong.
    float scaleX;
    float scaleY;
    public:
    int offsetSelf;
    collider(const std::vector<std::byte>&data,int addr){
        offsetSelf = addr;
        firstTri = getWord(data,addr);
        num_tris = getWord(data,addr+0x4);
        end_offset = getWord(data,addr+0x8); //forms ptrA
        buffer_start = getWord(data,addr+0xC); //forms ptrB
        xUpperLim = swapEndiannessForWord(getXBytesAtX(data,2,addr+0x10),2);//Halfwords
        yUpperLim = swapEndiannessForWord(getXBytesAtX(data,2,addr+0x12),2);
        
        scaleX = getFloatFromWord(getWord(data,addr+0x14)); //Scale? In both files, every object has this set to 40.
        scaleY = getFloatFromWord(getWord(data,addr+0x18)); //Scale? Same, 40.
        originX = getFloatFromWord(getWord(data,addr+0x1C)); //object_pointer[7] etc. -- OBJECT'S PERSONAL ORIGIN X
        originY = getFloatFromWord(getWord(data, addr+0x20)); //OBJECT ORIGIN Y. UPPER LEFT CORNER.

        const int triSize = 0x34;
        for (int i = 0; i < num_tris; i++)
        {
            int tri_offset = firstTri+(i*triSize);
            tris.push_back(tri(data,tri_offset));
        }

        for (int currentSet = end_offset; currentSet < buffer_start; currentSet+=0x8){
            samples.push_back(sample(data,currentSet,buffer_start));
        }
    }

    double getOriginX(){return originX;}
    double getOriginY(){return originY;}
    int getXUpperLim(){return xUpperLim;}
    int getYUpperLim(){return yUpperLim;}
    float getScaleX(){return scaleX;}
    float getScaleY(){return scaleY;}
    std::vector<tri> getTris(){return tris;}
    std::vector<sample> getSamples(){return samples;}
    
};

//assimilate this into collision object later...
std::vector<collider> setupMapData(std::string filename){ //call this at the application start. 
    std::vector<std::byte> data = readFile(filename); //include .ccd
    std::cout << "READ INTO BYTE DATA";
    std::vector<collider> res;
    int file_start = getWord(data,0x0); //returns 0x10 for our shop2f example..
    int entry_count = getWord(data,0x4);
    int sectionPtr = file_start;//this is a better system than us hardcoding 0x38 and counting byte by byte. Correctly skips some of the duplicate objects I think
    for (int j = 0; j < entry_count; j++)
    {
        int datum = getWord(data,sectionPtr + 0x28);
        if (datum != 0){
            res.push_back(collider(data,datum));
        }
        sectionPtr+=0x40;
    }
    return res;
}

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
double getCpLinePoint(d_coord& result_location, d_coord triPoint,d_coord successivePoint, d_coord adjX){ //unlike the other one, this can return a separate value.
    d_coord subResult = vectorSub(successivePoint,triPoint);
    subResult.x = static_cast<float>(subResult.x);
    subResult.z = static_cast<float>(subResult.z);
    subResult.y = static_cast<float>(subResult.y); //not sure if this correction needs to happen to vectorSub, but the perfectionist in me likes this workaround to get the correct value. Can also try removing this since it may not be necessary to solve my problems.

    double intermediateResult = 0.0;
    double subMag = vectorSquareMag(subResult);
    if (subMag == intermediateResult){
        result_location = triPoint;
    } else { //While it seems like it comes from sub result, not sure that it actually is???????
    //here comes perfectionism:
    double b4 = (subResult.y * (adjX.y - triPoint.y)) + (subResult.z * (adjX.z - triPoint.z)) + (subResult.x * (adjX.x - triPoint.x));
    intermediateResult = b4/subMag;
        // intermediateResult = 
        // ((subResult.y * (adjX.y - triPoint.y)) + 
        // (subResult.z * (adjX.z - triPoint.z)) + 
        // (subResult.x * (adjX.x - triPoint.x))) / subMag;

        d_coord scaleResult = vectorScale(intermediateResult,subResult);
        result_location = vectorAdd(scaleResult,triPoint);
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

bool myChkInTri(d_coord adjX_Data_ptr, const tri& tri_pointer){
    //Use std::signbit(-0) from math.h for the neg zero check. returns true if NEGATIVE.
    
    std::vector<float> logicVec = tri_pointer.normals;
    for (auto &&i : logicVec){  
        i = std::signbit(i) ? -i : i; //if i is negative, std::signbit returns true.
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


    // double tri_min_bound_A;
    // double tri_max_bound_A;
    // double tri_min_bound_B;
    // double tri_max_bound_B;
    //HERE IT IS.
    // double tri_min_bound_A = *std::min_element(A_Values.begin(),B_Values.end()); //default 1 million ingame if this min/max fails.
    // double tri_max_bound_A = *std::max_element(A_Values.begin(),A_Values.end());
    // double tri_min_bound_B = *std::min_element(B_Values.begin(),B_Values.end());
    // double tri_max_bound_B = *std::max_element(B_Values.begin(),B_Values.end());
    
    double tri_min_bound_A;
    double tri_max_bound_A;
    double tri_min_bound_B;
    double tri_max_bound_B;

    try {
        tri_min_bound_A = *std::min_element(A_Values.begin(), A_Values.end());
        tri_max_bound_A = *std::max_element(A_Values.begin(), A_Values.end());
        tri_min_bound_B = *std::min_element(B_Values.begin(), B_Values.end());
        tri_max_bound_B = *std::max_element(B_Values.begin(), B_Values.end());
    } catch (const std::exception& e) {
        std::cerr << "tri_max_bound exception (ChkInTri) caught: " << e.what() << std::endl << "Congratulations, you got the weird error to show up. Let Kapital know.\n";
        // Handle the exception here, for example set default values
        tri_min_bound_A = -1000000.0; // Default value as mentioned in the comment
        tri_max_bound_A = 1000000.0; // Default value as mentioned in the comment
        tri_min_bound_B = -1000000.0; // Default value as mentioned in the comment
        tri_max_bound_B = 1000000.0; // Default value as mentioned in the comment
    }

    if  (!(adjX_val_A > tri_min_bound_A) && (adjX_val_A < tri_max_bound_A) &&
          (adjX_val_B > tri_min_bound_B) && (adjX_val_B < tri_max_bound_B)) {
        return false;
    }

    //condition check for return
    return (    
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
    );
    
}

d_coord nudgePos(int ballSize, d_coord position_at_collision, d_coord AdjX){
    //could merge the ballSize adjustment into getPointExtensionLine?
    double ballAdjustment = ballSize + 0.00009999999747378752; // in the code: value: 0x38d1b717
    return getPointExtensionLine(ballAdjustment,position_at_collision,AdjX);
}

typedef std::function<bool(d_coord&,const tri&,const int,d_coord)> collEvalFn;
//my fn -- returns posAtCol and a bool which becomes didCollisionOccur
bool innerFoo0(d_coord &posAtCol, const tri &currentTri, const int ballSizeSquared, d_coord AdjX){ //no changes to adjX here.
    if (getSidePlanePoint(currentTri.normals,currentTri.points[0],AdjX) >= 0){ //shockingly simple. This signature and the next one are fine as is. Only shorten the chkInTri signature to take the whole tri. The other two fns make it clearer what data they're using by having separate parameters.
        d_coord cpPlaneResult = getCpPlanePoint(currentTri.normals,currentTri.points[0],AdjX); //CP IS INFLUENCED BY adjX -- does not return anything normally, but I may choose to alter this to be better style.
        if ((vectorSquareDistance(cpPlaneResult,AdjX) < ballSizeSquared) && myChkInTri(AdjX,currentTri) != 0) { //if the distance squared from the proposed to the CP is less than the squared ball size then begin detailed tri checking.
                posAtCol = cpPlaneResult; //save
                std::cout << "!!0\n";
                return true;
        }
    }
    std::cout << "-0\n";
    return false;
} //Seems to check the first point in detail?

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
                    double cpLineIntermediate = getCpLinePoint(cpLineResult,currentTri.points[j],currentTri.points[successivePoint],AdjX); ///LOOKS LIKE WE NEED YET ANOTHER SIGNED BIT CHECK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                    if ((cpLineIntermediate >= 0.0) && (cpLineIntermediate <= 1.0) && (vectorSquareDistance(cpLineResult,AdjX) < ballSizeSquared)){
                        posAtCol = cpLineResult;
                        std::cout << "!!1\n";
                        return true;
                    }
                }
            }
        }
    }
    std::cout << "-1\n";
    return false;
} //seems to check all points?

bool innerFoo2(d_coord& posAtCol, const tri& currentTri, const int ballSizeSquared, d_coord AdjX){ //REMARKABLY SIMILAR TO INNERFOO1 WITH TINY DIFFERENCE IN THE FINAL CHECK
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
                    std::cout << "!!2\n";
                    return true;
                }
            }
        }
    }
    std::cout << "-2\n";
    return false;
}


bool runSample(d_coord&posAtCol,sample smp,const std::vector<tri> &tris,const int ballSizeSquared, d_coord AdjX, collEvalFn evaluationPredicate){
    for (auto &&i : smp.sampleTris){
        tri currentTri = tris[i]; //comes from the collision object in question
        if (evaluationPredicate(posAtCol,currentTri,ballSizeSquared,AdjX)){
            return true; //collision occurred
        }
    } 
    return false;
}

bool samplePointCloud(d_coord&position_at_collision, int xMinusBall, int yMinusBall, int xPlusBall, int yPlusBall, collider objptr, const int ballSizeSquared, d_coord AdjX, collEvalFn evaluationPredicate){
    for (int ymbc = yMinusBall; ymbc <= yPlusBall; ymbc++){ //each iteration is a sample group, each sample group being an amount xUpperLim samples apart, offset by xmb. Ypb-ymb number of groups are examined. 
        int sampleSelectionIdx = objptr.getXUpperLim() * ymbc + xMinusBall; //starting point for a group of samples.
        for (int xmbc = xMinusBall; xmbc <= xPlusBall; xmbc++){ //Run some of (but not necessarily all) samples within the sample group. Xpb - xmb number of samples are run. 
            sample currentSample = objptr.getSamples()[sampleSelectionIdx]; //lookup sample from the whole sample data (rn this is all in one array, could simplify with codified sample groups)
            if (runSample(position_at_collision,currentSample,objptr.getTris(),ballSizeSquared,AdjX,evaluationPredicate)){ //Run a given sample series of length N. This is a chain of tris in a predetermined order in sampleOrder.
                return true;
            }
            sampleSelectionIdx++; //onto the next sample within the sample group
        }
    }
    return false;
}

bool checkHit(int ballSize, d_coord AdjX, collider objptr, d_coord& result_storage){
    int xMinusBall = (AdjX.x - ballSize - objptr.getOriginX())/objptr.getScaleX(); //these have many roles. For xmb ic is the offset for the sampling section, and it is also the limit of how many of the samples to run within a section of possible samples. 
    int yMinusBall = (AdjX.y - ballSize - objptr.getOriginY())/objptr.getScaleY(); //Intentional data loss with truncation to int.
    int xPlusBall  = (AdjX.x + ballSize - objptr.getOriginX())/objptr.getScaleX();
    int yPlusBall  = (AdjX.y + ballSize - objptr.getOriginY())/objptr.getScaleY();

    // min and max limits.
    xMinusBall = xMinusBall < 0 ? 0 : xMinusBall; //Since scaleX and scaleY are so big (40, 50 etc.) this should always be a small number, and since its an int its pretty much always gonna be between 0, 1 or 2. 3 or higher should be exceedingly rare.
    yMinusBall = yMinusBall < 0 ? 0 : yMinusBall;

    xPlusBall = xPlusBall > (objptr.getXUpperLim() - 1) ? objptr.getXUpperLim()-1 : xPlusBall; //Do not consolidate this into the lim itself, as this value is used raw later on.
    yPlusBall = yPlusBall > (objptr.getYUpperLim() - 1) ? objptr.getXUpperLim()-1 : yPlusBall;

    int ballSizeSquared = pow(ballSize,2); //could delete this line and send this squaring thing down to the runSample function, as it only is used in the three predicate functions. HOWEVER still need to send ballsize down then...
    
    d_coord position_at_collision;
    std::vector<collEvalFn> evaluationFns = {innerFoo0,innerFoo1,innerFoo2}; //Can skip the last two functions if the interaction flag & 7 == 0...
    for (collEvalFn fn : evaluationFns){
        std::cout << "EVAL FUNCTION\n";
        if (samplePointCloud(position_at_collision,xMinusBall,yMinusBall,xPlusBall,yPlusBall,objptr,ballSizeSquared,AdjX,fn)){ //TLDR: If there is a hit, then this records the position that the hit was detected. Then a static Nudge is applied to put the actor back inbounds.
            result_storage = nudgePos(ballSize,position_at_collision,AdjX);//could pull this back out to adjustIfCollide, and then just return true; OR could keep here.
            return true;
        }  
    }
    return false;
}

d_coord adjustIfCollide (int ballSize, d_coord proposed, d_coord old, const std::vector<collider>&mapColDat){
    d_coord adjusted = proposed;
    const double PLAYER_HEIGHT = 8.5;
    proposed.z += PLAYER_HEIGHT;
    // d_coord pointDiffs = vectorSub(proposed,old);
    // d_coord zeros;
    // zeros.x = 0;
    // zeros.z = 0;
    // zeros.y = 0;
    //std::cout << "POINT DIFFS: (" << pointDiffs.x << "," << pointDiffs.z << "," << pointDiffs.y << ")";
    for (collider colliderObj : mapColDat){
        if (colliderObj.offsetSelf == 7992){
            int a = 0; //debug
        }
        std::cout << "CHECKED COLLIDER\n";
        if (checkHit(ballSize,proposed,colliderObj,adjusted)){ 
            std::cout << "HIT FOUND\n";
            adjusted.z -= 8.5;
            return adjusted;
        }
    }
    std::cout << "NO HITS FOUND\n";
    proposed.z -= 8.5;
    return proposed;
}


int main(){
    const std::vector<collider> mapCollisionData = setupMapData("C:\\Users\\Carter\\Documents\\GitHub\\SecondaryManip\\SecondaryManip\\tmpFolder\\collideTest\\M1_shop_2F.ccd"); //can maybe be included in a greater map object which has background noise and npc stuff in there too?
    std::cout << "\nLoaded Collision Data!\n";


    //FOUND A COLLISION SUCCESSFULLY!!!!!!!!


    // d_coord current; //actual, current x as written in standard ramsearch block.
    // current.x = 16.37053680419922;
    // current.z = 0;
    // current.y = -18.424144744873047;
    // d_coord suppose; //To be modified if applicable.
    // suppose.x = 16.772022247314453;
    // suppose.z = 0;
    // suppose.y = -18.004671096801758;

    d_coord current; //actual, current x as written in standard ramsearch block.
    current.x = 21.9913330078125;
    current.z = 0;
    current.y = -12.551513671875;
    d_coord suppose; //To be modified if applicable.
    suppose.x = 22.392818450927734;
    suppose.z = 0;
    suppose.y = -12.132040023803711;
    
    int NPC_BALLSIZE = 3; //not const? I think it gets added to with TinyAdjustment later...

    std::cout << std::setprecision(16) << "OLD POS: (" << current.x << "," << current.z << "," << current.y << ")\n";
    d_coord proposed = adjustIfCollide(NPC_BALLSIZE,suppose,current,mapCollisionData); //If collision occurs, returns corrected position.
    std::cout << "NEW POS: (" << proposed.x << "," << proposed.z << "," << proposed.y << ")\n";
    if (suppose.x == proposed.x && suppose.z == proposed.z && suppose.y == proposed.y){
        std::cout << "UNCHANGED!";
    } else {
        std::cout << "ADJUSTED!\n";
    }
    return 0;
}