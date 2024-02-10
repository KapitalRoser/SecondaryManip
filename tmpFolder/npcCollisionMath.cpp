#include "processCoreLocal.h"
#include "NPC.h"
#include <functional>



//Yeah a little bit of fancy shit with the function pointers in checkFixedMdl.


struct tri {
    std::vector<d_coord> points;
    std::vector<float> normals;
    int interactionFlag; //looks like we need the first short? Idk if there's a good example of the 2nd short ever being not 0
};

struct collision_obj {
    int fileOffset;
    int num_tris;
    std::vector<tri>tris;
    double originX;
    double originY; //no z needed.

    int end_offset;
    int buffer_start;
    int xUpperLim;
    int yUpperLim;
    float scaleX;
    float scaleY;

    //Maybe include buffer zone data too... for point ordering..
};
typedef std::function<bool(d_coord&,const tri&,const int,d_coord)> collEvalFn;





//HYPOCRISY -- If I'm gonna prohibit it in SidePlanePoint then I should also prohibit it in CPPlanePoint.
//Is there another solution to this????????

//Fixed, good.
d_coord getCpPlanePoint(std::vector<float> tri_orientation,d_coord tri_pos_ptr,d_coord adjX_ptr_maybe){ //this assumes the params mean what I think they mean, still testing.

    // //Technically these are imported as an f_coord so this shouldn't be an issue. Deal with it earlier in the line? Or just use d_coords all the way thru here. 
    // float orientation_A = tri_orientation.x; //Once again feel the need to make this a tuple of size 3 for easier manipulation, but it isn't a position so d_coord isn't proper.
    // float orientation_B = tri_orientation.z;
    // float orientation_C = tri_orientation.y;

    // d_coord subRes = vectorSub(tri_pos_ptr,adjX_ptr_maybe);
    // double factor = (orientation_A * subRes.x) + (orientation_B*subRes.z) + (orientation_C*subRes.y);
    // double divisor = (pow(orientation_C,2)+ pow(orientation_A,2) + pow(orientation_B,2));  //can add a square vector for the divisor, need to keep this multiplication as is because scale uses a common term.
    // //double divisor = vectorSquareMag(orientation_Set);

    // d_coord scaleResult = vectorScale((factor / divisor), tri_orientation); //Need this double convert?
    // return vectorAdd(scaleResult,adjX_ptr_maybe); //Save first


    float orientation_A = tri_orientation[0]; //Once again feel the need to make this a tuple of size 3 for easier manipulation, but it isn't a position so d_coord isn't proper.
    float orientation_B = tri_orientation[1];
    float orientation_C = tri_orientation[2];

    d_coord subRes = vectorSub(tri_pos_ptr,adjX_ptr_maybe);
    double factor = (orientation_A * subRes.x) + (orientation_B*subRes.z) + (orientation_C*subRes.y);
    double divisor = (pow(orientation_C,2)+ pow(orientation_A,2) + pow(orientation_B,2));  //can add a square vector for the divisor, need to keep this multiplication as is because scale uses a common term.
    //double divisor = vectorSquareMag(orientation_Set);

    double scaleFactor = factor/divisor;
    d_coord scaleResult;
    scaleResult.x = scaleFactor * tri_orientation[0];
    scaleResult.z = scaleFactor * tri_orientation[1];
    scaleResult.y = scaleFactor * tri_orientation[2];
    return vectorAdd(scaleResult,adjX_ptr_maybe); //Save first





}

//Fixed, can update funkyOffset label later when I understand more.
double getCpLinePoint(d_coord& result_location, d_coord tri_pos_ptr,d_coord funkyOffset, d_coord adjX){ //unlike the other one, this can return a separate value.
    d_coord subResult = vectorSub(funkyOffset,tri_pos_ptr);
    double intermediateResult = 0.0;
    if (vectorSquareMag(subResult) == intermediateResult){
        //meaning sq result is 0
        result_location = tri_pos_ptr;
    } else {
        // working = vectorSub(adjX,tri_pos_ptr);
        // intermediateResult = (subresult.y * working.y) + ... 
        intermediateResult = 
        (subResult.y * (adjX.y - tri_pos_ptr.y)) + 
        (subResult.z * (adjX.z - tri_pos_ptr.z)) + 
        (subResult.x * (adjX.x - tri_pos_ptr.x));

        d_coord scaleResult = vectorScale(intermediateResult,subResult);
        result_location = vectorAdd(scaleResult,tri_pos_ptr);
    }
    return intermediateResult; //Do I really need to return this??? Is this useful?? Otherwise keep result_location.
    //looks like I do need this intermediate result to be returned as well as restul storing in result_location.

}

//This one is good. Nice and simple.
d_coord getPointExtensionLine(const double adjustedBallSize,d_coord position_at_collision,d_coord adjX){
    d_coord intermediate = vectorSub(adjX,position_at_collision);
    double mag = vectorMagnitude(intermediate);
    intermediate = vectorScale((adjustedBallSize / mag), intermediate);
    return vectorAdd(intermediate,position_at_collision); //return this instead of void return.
}

//This is good, would only change if I decide to allow orientation to be a d_coord. 
double getSidePlanePoint(std::vector<float>tri_orientation,d_coord tri_pos_ptr,d_coord adjX_ptr){

    return (double)(tri_orientation[0] * (adjX_ptr.x - tri_pos_ptr.x) +
                    tri_orientation[1] * (adjX_ptr.z - tri_pos_ptr.z) +
                    tri_orientation[2] * (adjX_ptr.y - tri_pos_ptr.y));
    //d_coord sub = vectorSub(adjX_ptr,tri_pos_ptr);
    //return double(tri_orientation[0] * sub.x, tri_orientation[1] * sub.z, tri_orientation[2] * sub.y)
    //So close to just vectorMultiply(tri_orientation, vectorSub(adjX,tri));...
}



int chkIntri (d_coord adjX_Data_ptr, tri tri_pointer,std::vector<float>orientationFlags){

    float adjX_val_A = 0;
    float adjX_val_B = 0;

    int returnVal = 0;
    const float FLOAT_0 = 0;
    int flag_0_2 = 0; //or 2
    int flag_0_1 = 0; //or 1
    //This should take in a tri object. This fn picks two of the 3 values from the points (Either all 3 X's all 3 Y's or all 3 Z's).
//    std::vector<float> tri_pointer = {37.1033,  -3.35513,  -11.0449, 24.9879,  -3.35513,  -11.0449, 24.9879,   24.5948,  -11.0449 };
    tri_pointer = 
    {37.1033,  -3.35513,  -11.0449, 
    24.9879,  -3.35513,  -11.0449, 
    24.9879,   24.5948,  -11.0449};
    std::vector<float> arranged_data;
//    std::vector<float> orientationFlags = {1,0,-0.0,-1}; // normals, Can be any number between 1 and -1 including 0 and -0.
    orientationFlags = {1,0,-0.0,-1}; // normals, Can be any number between 1 and -1 including 0 and -0.
    //std::vector<int> interactionFlags = {3,0}; //don't seem to have an impact on collision for our purposes.
    float a = orientationFlags[0];
    float b = orientationFlags[1];
    float c = orientationFlags[2];
    
   //Due to the many types of data possible in phenac and other maps, may not be able to abstract this out as much as i'd like.

    float a_copy = a;
    if (a <= FLOAT_0) {
        a_copy = -a;
    }
    float b_copy = b;
    if (b <= FLOAT_0) {
        b_copy = -b;
    }
    float c_copy = c;
    if (c <= FLOAT_0) {
        c_copy = -c;
    }

    //Use std::signbit(-0) from math.h for the neg zero check. returns true if NEGATIVE.

    //logic is done on the COPIES, ADJUST TO C IS DONE ON THE ACTUAL. Picks axis of tri.
    enum DIMENSION {X = 0, Z = 1, Y = 2};
    if (b_copy <= a_copy){
        if (a_copy <= c_copy){
            flag_0_2 = X; //xz
            flag_0_1 = Z;
        } else {
            flag_0_2 = Y; //yz
            flag_0_1 = Z;
            orientationFlags[2] = -orientationFlags[0]; //real!
        }
    } else if (c_copy <= b_copy){
        flag_0_2 = Y; //yx
        flag_0_1 = X;
        orientationFlags[2] = orientationFlags[1];
    } else {
        flag_0_2 = X; //x
        flag_0_1 = Z; //z
    } //Most of this if else block can be taken away by a getAllX's function on some array of 3 int coord objects like position.x .z and .y
    //GetAllY's or GetAllZ's too. As well as reversers for the negative C value where applicable. Probably easiest to use two sep vecs than this interlocking single vec format. tiny bit more memory but super readable.

    // int ptr = flag_0_2 * 4; //in the code this is inside the following if statement.
    // int ptr2 = flag_0_1 * 4;//not needed anymore


    //Convert this to d_coord stuff.
    std::vector<double> adjX_Floats = {adjX_Data_ptr.x,adjX_Data_ptr.z,adjX_Data_ptr.y}; //temp, come up with a more direct d_coord to value setup with the other.
    adjX_val_A = adjX_Floats[flag_0_2];
    adjX_val_B = adjX_Floats[flag_0_1];

    //original
    //adjX_val_A = adjX_Data_ptr + flag_0_2 * 4; //CHANGE TO FLAT INSTEAD OF *4 WHEN GET RID OF PTR AND REPLACE WITH VECTOR.
    //adjX_val_B = adjX_Data_ptr + flag_0_1 * 4; //same xyz selection as before.


//CHECK AGAINST POINTERS, MAKE SURE FLAGS ARE ACTUALLY IN THE RIGHT ORDER.
//WANT TO MAKE SURE IT IS SAFE TO REVERSE OR IF NEED TO KEEP THIS ARRANGEMENT AS IS, OR CUSTOM.
    if (0 <= orientationFlags[2]){ //i.e value is positive
        //this only makes sense on the context of pointers. 
        //values_vec().reverse()
        arranged_data[0] = tri_pointer[flag_0_2 + 6]; // or +3 
        arranged_data[1] = tri_pointer[flag_0_1 + 6];
        arranged_data[3] = tri_pointer[flag_0_2 + 3];
        arranged_data[4] = tri_pointer[flag_0_1 + 3];
        arranged_data[6] = tri_pointer[flag_0_2];
        arranged_data[7] = tri_pointer[flag_0_1];
        //2 and 5 are skipped.
    } else { // value_C is negative.
        arranged_data[0] = tri_pointer[flag_0_2];
        arranged_data[1] = tri_pointer[flag_0_1];
        arranged_data[3] = tri_pointer[flag_0_2 + 3];
        arranged_data[4] = tri_pointer[flag_0_1 + 3];
        arranged_data[6] = tri_pointer[flag_0_2 + 6]; // or +3 
        arranged_data[7] = tri_pointer[flag_0_1 + 6];
        //2 and 5 are skipped.
    }

    std::vector<float> A_Values = {arranged_data[0],arranged_data[3],arranged_data[6]};
    std::vector<float> B_Values = {arranged_data[1],arranged_data[4],arranged_data[7]};
    float tri_min_bound_A = *std::min_element(A_Values.begin(),B_Values.end()); //default 1 million ingame if this min/max fails. 
    float tri_min_bound_B = *std::min_element(B_Values.begin(),B_Values.end());
    float tri_max_bound_A = *std::max_element(A_Values.begin(),A_Values.end());
    float tri_max_bound_B = *std::max_element(B_Values.begin(),B_Values.end());


    if   ((((adjX_val_A < tri_min_bound_A)  || 
            (adjX_val_B < tri_min_bound_B)) || 
            (tri_max_bound_A < adjX_val_A)) || 
            (tri_max_bound_B < adjX_val_B)) {
        return 0; //flip and make &&'s?
    }
    
    //using 2 vecs might make this simpler and take out some of the above rearranging that we do.


    if(    
        (
            (arranged_data[3] - arranged_data[0]) * (adjX_val_B - arranged_data[1]) -
            (arranged_data[4] - arranged_data[1]) * (adjX_val_A - arranged_data[0]) <= 0.0
        ) 
            &&
        (
            (arranged_data[6] - arranged_data[3]) * (adjX_val_B - arranged_data[4]) -
            (arranged_data[7] - arranged_data[4]) * (adjX_val_A - arranged_data[3]) <= 0.0
        )
            &&
        (
            (arranged_data[0] - arranged_data[6]) * (adjX_val_B - arranged_data[7]) -
            (arranged_data[1] - arranged_data[7]) * (adjX_val_A - arranged_data[6]) <= 0.0
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




//my interpretation


//need to decide how much copying of data we want to do here. in some ways its simpler to make a structure or vector based on the file_Data vector of bytes. 
//While the majority of data in the file are words, not all of them are. There are a few halfwords, and some are ints and some are floats. 
//How long do I need to preserve the byte format of the data, vs just reading all the words and indexing from there. 
//If I keep em as bytes it makes more sense to carry an index around like a pointer. 
//Alternatively, I can use an iterator for perhaps better safety. 
//however we gotta keep this organized or I'll lose it somewhere in all the function calls.


//my fn -- returns posAtCol and a bool which becomes didCollisionOccur
bool innerFoo0(d_coord &posAtCol, const tri &currentTri, const int ballSizeSquared, d_coord AdjX){ //no changes to adjX here.
    if (getSidePlanePoint(currentTri.normals,currentTri.points[0],AdjX) >= 0){ //shockingly simple. This signature and the next one are fine as is. Only shorten the chkInTri signature to take the whole tri. The other two fns make it clearer what data they're using by having separate parameters.
        d_coord cpPlaneResult = getCpPlanePoint(currentTri.normals,currentTri.points[0],AdjX); //CP IS INFLUENCED BY adjX -- does not return anything normally, but I may choose to alter this to be better style.
        if ((vectorSquareDistance(cpPlaneResult,AdjX) < ballSizeSquared) && chkIntri(AdjX,currentTri.points,currentTri.normals) != 0) { //if the distance squared from the proposed to the CP is less than the squared ball size then begin detailed tri checking.
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

//This could use a llittle bit of work. It would be cool to have a better system for ptrA and ptrB than doing weird pointer math on a std::vec of bytes.
bool searchTriSample(d_coord&position_at_collision, int xMinusBall, int yMinusBall, int xPlusBall, int yPlusBall, collision_obj objptr, const int ballSizeSquared, d_coord AdjX, collEvalFn evaluationPredicate){
    for (int ymbc = yMinusBall; ymbc < yPlusBall; ymbc++){
        int ptrA = objptr.end_offset + (xMinusBall + ymbc * objptr.xUpperLim) * 8; //Dips into part of the "buffer" data at the end of the object data.
        for (int xmbc = xMinusBall; xmbc < xPlusBall; xmbc++){
            int ptrB = objptr.buffer_start + ptrA *4; //Really care about the value at this pointer.
            int triSampleSize = ptrA * 0x4; //max number of tris to sample. ptrA[1]
            for (int i = 0; i < triSampleSize; i++){
                
                int currentTri_idx = ptrB; //Value AT ptrB... originally ptr start + value at ptrB * 0x34 which is size of a tri.
                tri currentTri = objptr.tris[currentTri_idx]; //Assuming that I will fill up objptr.tris in the order in which the tris are recorded to file.            

                bool didCollisionOccur = evaluationPredicate(position_at_collision,currentTri,ballSizeSquared,AdjX);
                if (didCollisionOccur){
                    return true;
                }
                ptrB++;//ptr +1 or value  +1?
            }
            ptrA+=2;
        }
    }
    return false;
}


bool myCheckFixMdl(int ballSize, d_coord& AdjX, int* object_pointer, d_coord& result_storage){
    bool resultFlag;
    d_coord position_at_collision;

    /*
        I'm hypothesising that the "BUFFER" area is actually the order in which tris are to be evaluated.
        It appears that the order in which the tris are written is not the order in which they are evaluated by the loops.
        Even the "order" that is written in ptrB's section of the buffer isn't correct. The list seems to start with the 6th tri...


        Ptr A comes as 2 values. The first one is used to form Ptr B which in turn forms an offset/ptr to a tri.
        The 2nd word acts as a limit of some kind for the super inner loop. 
        When ptrA is incremented, it is incremented in 2 word amts. 


        Ex.
        First get end_offset which is objptr[2]. Then do math which in our test ended up at +0x8 which gave us:
        0x17B8 == 0x12, where [1] for that is 0x17BC == 0xA.
        Then when iterate, ptrA is incremented to 0x17C0 == 1C and 17C4 == 0E



        So what about ptrB?
        ptrB is gained from a different section of the "buffer". 
        From there we would take the 0x12 from before, multiply by the word size (4) to get the adjustment needed. 0x48 in this case.
        Add to the begin_buffer ptr (which comes from objptr[3]). Congrats, have your ptrB. This is incremented in wordsize +4, not like +8 as with ptrA.
        If begin_buffer is 0x1810, then +48 becomes 0x1858 == 0x6

        ptrA and B aren't limited by themselves as such, they're limited by xminusball and yminusball.

        the 0x6 from ptrB is multiplied by the size of a tri object (0x34) to get an offset to a specific tri.
        overall a weird system of like references with sizes to references to tris.

        unfortunately the order seems to be influenced by xminusball and yminusball which in turn are influenced by adjX. 
        Cannot do much pre-ordering that is.
        Maybe its like a pre-sampling system?
    */

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
    

    //This needs to be done on the fly
    //In the game these are narrowed down intentionally into int.
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


    // if (searchTriSample(position_at_collision,xMinusBall,yMinusBall,xPlusBall,yPlusBall,objptr,ballSizeSquared,AdjX,innerFoo0)){
    //     result_storage = nudgePos(ballSize,position_at_collision,AdjX);
    //     return true;
    // }

    // if (searchTriSample(position_at_collision,xMinusBall,yMinusBall,xPlusBall,yPlusBall,objptr,ballSizeSquared,AdjX,innerFoo1)){
    //     result_storage = nudgePos(ballSize,position_at_collision,AdjX);
    //     return true;
    // }

    // if (searchTriSample(position_at_collision,xMinusBall,yMinusBall,xPlusBall,yPlusBall,objptr,ballSizeSquared,AdjX,innerFoo2)){
    //     result_storage = nudgePos(ballSize,position_at_collision,AdjX);
    //     return true;
    // }
    // return false;
    
    // for (int ymbc = yMinusBall; (ymbc < yPlusBall) && (!didCollisionOccur); ymbc++){
    //     int ptrA = objptr.end_offset + (xMinusBall + ymbc * objptr.xUpperLim) * 8; //Dips into part of the "buffer" data at the end of the object data.
    //     for (int xmbc = xMinusBall; (xmbc < xPlusBall) && (!didCollisionOccur); xmbc++){
    //         int ptrB = objptr.buffer_start + ptrA *4; //Really care about the value at this pointer.
    //         int ptrA_bracket1 = ptrA * 0x4;
    //         for (int i = 0; (i < ptrA_bracket1) && (!didCollisionOccur); i++){
    //             int currentTri_idx = ptrB; //Value AT ptrB... originally ptr start + value at ptrB * 0x34 which is size of a tri.
    //             tri currentTri = objptr.tris[currentTri_idx]; //Assuming that I will fill up objptr.tris in the order in which the tris are recorded to file.            
    //             //Could oneline this. Would be nice to understand ptrB a bit better.
    //             //There's probably a way to abstract this out to not even need all this extra loop stuff. 
    //             didCollisionOccur = innerFoo0(position_at_collision,currentTri,ballSizeSquared,AdjX);//note if tru, then loop exits.
    //             ptrB++;//ptr +1 or value  +1?
    //         }
    //         ptrA+=2;
    //     }
    // }

    // if (didCollisionOccur){
    //     result_storage = nudgePos(ballSize,position_at_collision,AdjX);
    //     return 1;
    // }

    // //Double check that J isn't modified in ghidra.
    // for (int ymbc2 = yMinusBall; (ymbc2 < yPlusBall) && (!didCollisionOccur); ymbc2++)
    // {
    //     int ptrA = objptr.end_offset + (xMinusBall + ymbc2 * objptr.xUpperLim) * 8;
    //     for (int xmbc2 = xMinusBall; (xmbc2 < xPlusBall)  && (!didCollisionOccur); xmbc2++)
    //     {
    //         int ptrB = objptr.buffer_start + ptrA * 4;
    //         int ptrA_bracket1 = ptrA * 0x4;
    //         for (int i = 0; (i < ptrA_bracket1) && (!didCollisionOccur); i++)
    //         {
    //             int currentTri_idx = ptrB; //value AT ptrB
    //             tri currentTri = objptr.tris[currentTri_idx]; //In the game this is a pointer. I'm hoping I won't regret the structure I'm setting up here.            
                
    //             didCollisionOccur = innerFoo1(position_at_collision,currentTri,ballSizeSquared,AdjX);
    //             ptrB++;
    //         }
    //         ptrA+=2;
    //     }
        
    // }

    // if (didCollisionOccur){
    //     result_storage = nudgePos(ballSize,position_at_collision,AdjX);
    //     return 1;
    // }

    // //third and final loop.

    // for (int ymbc3 = yMinusBall; (ymbc3 < yPlusBall) && (!didCollisionOccur); ymbc3++)
    // {
    //     int ptrA = objptr.end_offset + (xMinusBall + ymbc3 * objptr.xUpperLim) * 8;
    //     for (int xmbc3 = xMinusBall; (xmbc3 < xPlusBall)  && (!didCollisionOccur); xmbc3++)
    //     {
    //         int ptrB = objptr.buffer_start + ptrA * 4; //not a ptr? the values??
    //         int ptrA_bracket1 = ptrA * 0x4;
    //         for (int i = 0; (i < ptrA_bracket1) && (!didCollisionOccur); i++)
    //         {
    //             int currentTri_idx = ptrB; //0x34 is the size of a tri, so this probably iterates through the tris. -- IS THIS JUST 1 POINT OR THE START OF THE TRI?????
    //             tri currentTri = objptr.tris[currentTri_idx]; //In the game this is a pointer. I'm hoping I won't regret the structure I'm setting up here.            
                
    //             didCollisionOccur = innerFoo2(position_at_collision,currentTri,ballSizeSquared,AdjX);
    //             ptrB++; //Could increment this after the curTri thing is sorted..
    //         }
    //         ptrA+=2;
    //     }
    // }

    // if (didCollisionOccur){
    //     result_storage = nudgePos(ballSize,position_at_collision,AdjX);
    //     return 1;
    // }
    //return 0;
}

//Apply this same process to the other two loops.

//Ideally try to understand the way the game is iterating through tris -- I'm afraid it may be using the data at the end/buffer section to iterate, when we have a perfectly good list of offsets at the start of the file to use. ptrA and ptrB in particular cause me concern...
 //I'm assuming that these 1st 2 functions use the first point of the tri in question. The ptr is definitely the start of the bigger tri.
                //Can simplify these function signatures by passing a tri object instead of two separate parameters to the same tri object.







//DO I NEED BOTH ADJX AND RESULT STORAGE MODIFIABLE?
int checkHitFixedMdl(int ballSize, d_coord& AdjX, int* object_pointer, d_coord& result_storage){ //NOTE THAT BOTH AdjX and ResultStorage may be modified and used later.......
    //MATH TIME
    //similar to the greater file, the object struct has similar elements.

    //[0] = offset to the actual tri points data.
    //[1] = num_tris in the object.
    //[2] = offset to end of list. (after the last 0300 or 0600 bit.)
    //[3] = start of the "buffer space" before the following list. shouldn't matter for us.
    //[4].A = Upper limit for xPlusBall
    //[4].B = Upper limit for yPlusBall
    //These four values are whole numbers which will m
    //[5] = Scale? Seems always set to 40
    //[6] = Scale? Seems always set to 40
    //[7] = Object Origin X
    //[8] = Object Origin Y.
    
    int resultFlag;
    d_coord position_at_collision;



    int* object_start = object_pointer;
    int num_tris = object_pointer[1];
    int* end_offset = (int*)object_pointer[2]; //possibly used?
    int* buffer_start = (int*)object_pointer[3]; //Not used
    int xUpperLim = *(object_pointer+0x10); //Halfword
    int yUpperLim = *(object_pointer+0x12);//Halfword
    float intermediateObjectPtr5 = object_pointer[5]; //Scale? In both files, every object has this set to 40.
    float intermediateObjectPtr6 = object_pointer[6]; //Scale? Same, 40.
    float objOriginX = object_pointer[7]; //object_pointer[7] etc. -- OBJECT'S PERSONAL ORIGIN X
    float objOriginY = object_pointer[8]; //OBJECT ORIGIN Y. UPPER LEFT CORNER.
    
    //In the game these are narrowed down intentionally into int.
    int xMinusBall = (AdjX.x - ballSize -  objOriginX)/intermediateObjectPtr5;
    int yMinusBall = (AdjX.y - ballSize - objOriginY)/intermediateObjectPtr6;
    int xPlusBall = (AdjX.x + ballSize - objOriginX)/intermediateObjectPtr5;
    int yPlusBall = (AdjX.y + ballSize - objOriginY)/intermediateObjectPtr6;

    // min and max limits.
    xMinusBall = xMinusBall < 0 ? 0 : xMinusBall;
    yMinusBall = yMinusBall < 0 ? 0 : yMinusBall;

    xPlusBall = xPlusBall > (xUpperLim - 1) ? xUpperLim-1 : xPlusBall;
    yPlusBall = yPlusBall > (yUpperLim - 1) ? yUpperLim-1 : yPlusBall;


    int ballSizeSquared = pow(ballSize,2);


    int yMinusBall_Copy = yMinusBall;
    bool didCollisionOccur = false; //This is probably "did collision occur"
    while ((yMinusBall_Copy <= yPlusBall) && (!didCollisionOccur)){
        int ptrA = end_offset + (xMinusBall + yMinusBall_Copy * xUpperLim) * 8;
        int xMinusBall_copy = xMinusBall;
        while (xMinusBall_copy <= xPlusBall && (!didCollisionOccur)){
            int i = 0;
            int ptrB = buffer_start + ptrA * 4; //not a ptr? the values??
            int ptrA_bracket1 = 0;
            while(i < ptrA_bracket1 && (!didCollisionOccur)){
                std::vector<d_coord>tri_ptr = object_pointer + (ptrB * 0x34); //9 d_coords total. //0x34 is the size of a tri, so this probably iterates through the tris. REFERS TO ONE SINGLE TRI WITHIN OBJECT, NOT ONE POSITION WITHIN TRI. CANNOT BE D_COORD. CONTAINS 3 D_COORDS.
                std::vector<float>tri_Normals; //comes after the tri.
                double planePoint_result = getSidePlanePoint(tri_Normals,tri_ptr,AdjX); //shockingly simple. Circumstantial parameters, need to confirm that these are what I think they are.
                if (planePoint_result >= 0){
                    d_coord cpPlaneResult = getCpPlanePoint(tri_ptr+0x24,tri_ptr,AdjX); //CP IS INFLUENCED BY adjX -- does not return anything normally, but I may choose to alter this to be better style.
                    double distanceSquared = vectorSquareDistance(cpPlaneResult,AdjX);
                    if (distanceSquared < ballSizeSquared) { //if the distance squared from the proposed to the CP is less than the squared ball size then begin detailed tri checking.
                        int ChkInTri_result = chkIntri(AdjX,tri_ptr,tri_ptr+0x24);//first param is adjX stuff I believe
                        if (ChkInTri_result == 0){
                            didCollisionOccur = false;
                        } else {
                            didCollisionOccur = true;
                            position_at_collision = cpPlaneResult;
                        }
                    } else {
                        didCollisionOccur = false;
                    }
                } else {
                    didCollisionOccur = false;
                }
                i++;
                ptrB++;
            }
            xMinusBall_copy++;
            ptrA+=2;
        }
        yMinusBall_Copy++;
    }


    
    
    














    if (didCollisionOccur){
        result_storage = nudgePos(ballSize,position_at_collision,AdjX);
        resultFlag = 1;
    } else {
        //2nd big loop:

        //all of this is identical to the loop above until the differences comment;
        didCollisionOccur = false;
        int yMinusBall_Copy_2 = yMinusBall;
        while ((yMinusBall_Copy_2 <= yPlusBall) && (!didCollisionOccur)){
            int ptrA = end_offset + (xMinusBall + yMinusBall_Copy_2 * xUpperLim) * 8;
            int xMinusBall_copy_2 = xMinusBall;
            while((xMinusBall_copy_2 <= xPlusBall) && (!didCollisionOccur)){
                int i = 0;
                int ptrB = buffer_start + ptrA * 4; //not a ptr? the values??
                int ptrA_bracket1 = 0;
                while(i < ptrA_bracket1 && (!didCollisionOccur)){
                    float tri_ptr = object_pointer + (ptrB * 0x34);
                    //NOW THE DIFFERENCES BEGIN
                    int val_at_tri_ptr_plus30 = 0;
                    if ((val_at_tri_ptr_plus30 & 7)!= 0){
                        //save some data, very weird.. investigate.
                        int local_d0 = 0; //used once?
                        int local_cc = 0; //never used?
                        double planePoint_result = getSidePlanePoint(tri_ptr+0x24,tri_ptr,AdjX);
                        if (0 <= planePoint_result){
                            int ptrC = local_d0;
                            int j = 0;
                            float tri_ptr_copy = tri_ptr;
                            do {
                                if ((val_at_tri_ptr_plus30 & ptrC) != 0){
                                    int iVar2 = j+1;
                                    if (2 < iVar2){
                                        iVar2 = 0;
                                    }
                                    d_coord cpLineResult;
                                    //I REALLY NEED FUNKY OFFSET....DAFUQ IS THAT.
                                    float funkyOffset = tri_ptr + iVar2 * 0xC;
                                    double cpLineIntermediate = getCpLinePoint(cpLineResult,tri_ptr_copy,funkyOffset,AdjX); //--------------UNFINISHED!
                                    double distanceSquared_2 = vectorSquareDistance(cpLineResult,AdjX);
                                    if (0.0 <= cpLineIntermediate && 1.0 <= cpLineIntermediate && distanceSquared_2 < ballSizeSquared){
                                        //MEANINGFUL DIFFERENCES END
                                        didCollisionOccur = true;
                                        position_at_collision = cpLineResult;
                                        //Icky gross gross gross
                                        goto badStyle;
                                    }
                                }
                                j++;
                                tri_ptr_copy+=0xC;
                                ptrC+=2;
                            }while (j < 3);
                            didCollisionOccur = false;
                        } else {
                            didCollisionOccur = false;
                        }
                        //ACTUAL DIFFERENCES END.
                    }
badStyle:
                    i++;
                    ptrB++;
                }
                xMinusBall_copy_2++;
                ptrA+=2;
            }
            yMinusBall_Copy_2++;
        }
        if (didCollisionOccur){
            result_storage = nudgePos(ballSize,position_at_collision,AdjX);
            resultFlag = 1;
        } else {
            //third and final loop.
            didCollisionOccur = false;
            //no copy done -- use straight up value.
            while ((yMinusBall <= yPlusBall) && (!didCollisionOccur))
            {
                int ptrA = end_offset + (xMinusBall + yMinusBall * xUpperLim) * 8;
                int xMinusBall_copy_3 = xMinusBall;
                while ((xMinusBall_copy_3 <= xPlusBall) && (!didCollisionOccur)){
                    int i = 0;
                    int ptrB = buffer_start + ptrA * 4; //not a ptr? the values??
                    int ptrA_bracket1 = 0;
                    //up until this loop below, all three sets use the same setup.
                    while((i < ptrA_bracket1) && (!didCollisionOccur)){
                        float tri_ptr = object_pointer + (ptrB * 0x34);
                        int val_at_tri_ptr_plus30 = 0;
                        if ((val_at_tri_ptr_plus30 & 7)!= 0){
                            int local_d8 = 0;
                            int local_d4 = 0;
                            double planePoint_result = getSidePlanePoint();
                            if (0 <= planePoint_result){
                                int j_2 = 0;
                                d_coord tri_ptr_3_copy = tri_ptr;
                                int ptrC = local_d8;
                                do
                                {
                                    int iVar8 = j_2 + 2;
                                    if (2 < iVar8){
                                        iVar8 = j_2 + -1;
                                    }
                                    //NO CP GETTING HERE....
                                    double distanceSquared_3 = vectorSquareDistance(tri_ptr_3_copy,AdjX);
                                    if (((val_at_tri_ptr_plus30 & ptrC) != 0) && ((val_at_tri_ptr_plus30 & (int)&local_d8 + iVar8 * 2) != 0) && (distanceSquared_3 < ballSizeSquared) ){
                                        didCollisionOccur = true;
                                        d_coord posAtCol_ptr = tri_ptr + j_2 * 0xC;
                                        position_at_collision = posAtCol_ptr;
                                        //ICK A GOTO.... How dare you ghidra.
                                        goto badStyle2;
                                    }
                                    j_2++;
                                    tri_ptr_3_copy.x += 0xC; //applies to ptr
                                    ptrC += 2;
                                    /* code */
                                } while (j_2 < 3);
                                didCollisionOccur = false;
                            } 
                            else {
                                didCollisionOccur = false;
                            }
                        }
//ICK ICK ICK ICK ICK REDO THIS FOR THE LOVE OF GOD                            
badStyle2:
                    i++;
                    ptrB++;
                    }
                xMinusBall_copy_3++;
                ptrA+=2;
                }
                yMinusBall++;
            }
            if (didCollisionOccur){
                result_storage = nudgePos(ballSize,position_at_collision,AdjX);
                resultFlag = 1;
            } else {
                resultFlag = 0;
            }
        }
    }
    return resultFlag;
}

int getObjEnabled(int entry_j){ //UNUSED...
    // int datC68 = 0x80404C68; //Value AT: C68: ends up containing the pointer to the .ccd data.
    int datC6C = 0x80404C6C; //Start of list of enabled objects... not part of the normal ..ccd data.
    int dat36C = 0x8040836C; //NO IDEA
    //If file_start doesn't exist (==0) or the entry_j >= num_entries, return 0;
    return (dat36C * 0xDC0) + 0x80404C6C + (entry_j * 0x28); //RETURNS VALUE AT THE POINTER WHICH RESULTS FROM THIS EQUATION.
}

//0101010111010101
//0000000000000001 if odd, == 1

bool checkHitCollision(int collisionBallSize,d_coord store_A, d_coord store_B,d_coord result_storage_location)
{
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

    int firstLim = 0;
    int i = 0;
    do {
        section_ptr = *list_start; //literal data at file_start. Ex. 0x10 Typically the beginning of the block of sections which contain the offsets to the objects.
        firstLim = 0;
        unsigned int entry_count = list_start[1]; //same as in our parser.
        for (unsigned int section_j = 0; section_j < entry_count; section_j++) {
            object_ptr = *(int **)(section_ptr + 0x28); //get an address to an object.
            if (object_ptr != (int *)0x0) { //exist? continue to check. else skip.
                if (checkHitFixedMdl(collisionBallSize,AdjX,object_ptr, collision_calc_result_location_ptr) != 0) {
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

        //FILE: 
        // REGIONS (OFTEN 1 OR 1 BIG ONE)
        // SECTIONS
        // OBJECTS
        // TRIS
        // POINTS
        // COORDS




// int GS_collision(int ballSize, d_coord& adjustedPositions_probably){
//     //Needs to not only return a 
//     //default ballSize = 3
//     d_coord old; //As in the actual current position saved in memory.
//     int returnVal = 0;
//     old.x = 0;
//     old.y = 0;

//     d_coord proposed; //Aka newCurPos, has not been saved to memory yet and char hasn't physically been moved there yet.
//     proposed.x = 0;
//     proposed.y = 0;


//     //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Actual code begins below~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//     //NPC martBoy({17,-21},"G");
//     double stepDistance = vectorDistance(proposed,old); //correct direction? -- difference to psVecDist?

//     //ball over dist
//     double divResult = stepDistance > 0 ? ballSize / stepDistance : 0; //Should always be positive.
//     divResult = divResult >= 1 ? 1 : divResult; //Forces div result to be between 0 and 1 inclusive.

//     d_coord subResult = vectorSub(proposed,old);

//     for (int i = 0; i < 1; i+= divResult){
//         double local_D = (i+divResult) > 1 ? 1 : i+divResult;
//         d_coord dataStore_A;        
//         dataStore_A = vectorScale(i,subResult);
//         dataStore_A = vectorAdd(dataStore_A,old);
        
//         d_coord dataStore_B;
//         dataStore_B = vectorScale(local_D,subResult); //What does local_d really represent here? Is there some math that I can do to simplify this?
//         dataStore_B = vectorAdd(dataStore_B,old);

       
//         bool collisionResultFlag = checkHitCollision(ballSize,dataStore_A,dataStore_B,adjustedPositions_probably,0);
//         if(collisionResultFlag != false){
//             return 1;
//         }
//         if (divResult <= 0){
//             break; //INTERESTING LINE. Sorta like a do while since this implies the above loop runs at least once even while divResult is <=0.
//         }
//     }
//     return 0;
// }

int my_Collision(int ballSize, d_coord& adjPosLoc, d_coord old){ //might not need to return a bool

    int returnVal = 0;

    d_coord proposed = adjPosLoc;

    d_coord point_diffs = vectorSub(proposed,old); //Can compress further if needed.
       
    bool collisionResultFlag = checkHitCollision(ballSize,old,point_diffs,adjPosLoc,0);
    return collisionResultFlag;
}


// int peopleAdjustPosition(int ballsize,d_coord &proposed){ //IF TRU, MODIFIES PROPOSED HERE.(?) May break up the functions and adjust hierarchy.
//     //what is different between the two scenarios it is called in?
//     //Called in MoveAlongAngle [as part of the normal step check done ever frame] and directly in MoveSub [when distance to target did not decrease]. What gives?
//     d_coord proposed;
//     d_coord collisionAdjPos;
//     //all the stuff before our collision call is ptr setup, lookup and allocation.
//     int collFlag = GS_collision(ballsize,collisionAdjPos); //should only ever return 1 or 0.
//     if (collFlag != 0){
//         d_coord intermediateSub = vectorSub(collisionAdjPos,proposed);
//         proposed = vectorAdd(proposed,intermediateSub); //Update proposed with collisionAdjPos.
//     }
//     //Do get Walk Height block. Is this needed? -- Doesn't seem so.
//     return 1; //Always returns 1, and thus is the bug in the OG code. For our purposes, cannot return anything other than 1.
// }


//my ver of peopleAdjPosition
d_coord AdjustIfCollide(int ballSize, d_coord proposed, d_coord old){
    d_coord adjusted = proposed;
    if (my_Collision(ballSize,adjusted, d_coord old)){
        return adjusted; //This format plays slightly safer, only making the modification if GS_Collision returns 1.
    } //Need to see if GS_Collision modifies position if there is no collision. Otherwise just return the adjusted paramet of GS Collision.
    //Can then redo GS_Collision to return a position instead of a bool. Can then redo this function to get rid of unnecessary call.
}

int main(){
    //pretend this is coming from the existing code.
    //MoveTypeRandom is basically NPC_Action()
    //PeopleMoveSub:
        //Get distance
        //Call MoveAlong Angle. (contains peopleAdjustPosition)
        //Always returns 1
        //Get distance after step:
        //  If distance decreased, flag == 0 and walking CONTINUES. Return to MoveTypeRandom.
        //  Else: Call peopleAdjustposition again. This time I think the position just snaps? Parameters are slightly different here, should compare these calls.
        //        Also set flag == 1 and return.
        //This function is mostly equivalent to my incrementPosition function.



    d_coord proposed; //To be modified if applicable.
    int NPC_BALLSIZE = 3; //not const? I think it gets added to with TinyAdjustment later...
    //peopleAdjustPosition(3,proposed); //Only returns 1 due to bug in Colo. XD can return 0...

    return 0;
}




