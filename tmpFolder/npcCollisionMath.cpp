#include "processCoreLocal.h"
#include "NPC.h"

// class tri_data
// {
// private:
//     std::vector<float> points; // 9 values, 3 data pts each.
//     int orientation_A;
//     int orientation_B;
//     int orientation_C;
//     int interactFlag_A;
//     int interactFlag_B;
// public:
//     tri_data(/* args */);
//     ~tri_data();
// };

// tri_data::tri_data(/* args */)
// {
// }

// tri_data::~tri_data()
// {
// }



//What is this tmpOrient here for?? Otherwise its pretty much good.
d_coord getCpPlanePoint(d_coord tri_orientation,d_coord tri_pos_ptr,d_coord adjX_ptr_maybe){ //this assumes the params mean what I think they mean, still testing.
    d_coord tmpOrient; //DAFUQ IS THIS DOIN HERE????

    float orientation_A = tri_orientation.x; //should these be floats or doubles? This is technically narrowing.
    float orientation_B = tri_orientation.z;
    float orientation_C = tri_orientation.y;

    d_coord subRes = vectorSub(tri_pos_ptr,adjX_ptr_maybe);
    double factor = (orientation_A * subRes.x) + (orientation_B*subRes.z) + (orientation_C*subRes.y);
    double divisor = (pow(orientation_C,2)+ pow(orientation_A,2) + pow(orientation_B,2));  //can add a square vector for the divisor, need to keep this multiplication as is because scale uses a common term.

    //Here we have the important question of is d_coord a position or is d_coord any tuple of size three. It isn't really. but it would be so clean to use here...
    d_coord scaleResult = vectorScale((double)(factor / divisor), tmpOrient); //Need this double convert?
    return vectorAdd(scaleResult,adjX_ptr_maybe); //Save first
}
//dafuq is funkyOffset? What is triPtr doing here?
double getCpLinePoint(d_coord& result_location, d_coord tri_pos_ptr,d_coord funkyOffset, d_coord adjX){ //unlike the other one, this can return a separate value.
   //HOW DOES TRI POINTER MAKE SENSE HERE???? REVIEW! Is this a particular point within the tri?
    //WHAT IS FUNKY OFFSET?!?! REVIEW
    d_coord subResult = vectorSub(funkyOffset,tri_pos_ptr);
    double intermediateResult = 0.0;
    if (vectorSquareMag(subResult) == intermediateResult){
        //meaning sq result is 0
        // result_location.x = tri_pos_ptr.x;
        // result_location.z = tri_pos_ptr.z;
        // result_location.y = tri_pos_ptr.y;
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

}

//This one is good. Nice and simple.
void getPointExtensionLine(const double adjustedBallSize,d_coord &finalResultLocation,d_coord position_at_collision,d_coord adjX){
    d_coord intermediate = vectorSub(adjX,position_at_collision);
    double mag = vectorMagnitude(intermediate);
    intermediate = vectorScale((adjustedBallSize / mag), intermediate);
    finalResultLocation = vectorAdd(intermediate,position_at_collision); //return this instead of void return.
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



int chkIntri (d_coord adjX_Data_ptr, std::vector<float> tri_pointer,std::vector<float>orientationFlags){

    float adjX_val_A = 0;
    float adjX_val_B = 0;

    int returnVal = 0;
    const float FLOAT_0 = 0;
    int flag_0_2 = 0; //or 2
    int flag_0_1 = 0; //or 1
//    std::vector<float> tri_pointer = {37.1033,  -3.35513,  -11.0449, 24.9879,  -3.35513,  -11.0449, 24.9879,   24.5948,  -11.0449 };
    tri_pointer = {37.1033,  -3.35513,  -11.0449, 24.9879,  -3.35513,  -11.0449, 24.9879,   24.5948,  -11.0449};
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

    /*
    for (float flag : orientationFlags){
        flag = flag <= 0 ? -(flag) : flag; //include way to make 0 into neg(0) or redo the whole thing with enums or from bottom up.
        //Use std::signbit(-0) from math.h for the neg zero check. returns true if NEGATIVE.
    }
    */
    //logic is done on the COPIES, ADJUST TO C IS DONE ON THE ACTUAL. Picks axis of tri.
    if (b_copy <= a_copy){
        if (a_copy <= c_copy){
            flag_0_2 = 0; //xz
            flag_0_1 = 1;
        } else {
            flag_0_2 = 2; //yz
            flag_0_1 = 1;
            orientationFlags[2] = -orientationFlags[0]; //real!
        }
    } else if (c_copy <= b_copy){
        flag_0_2 = 2; //yx
        flag_0_1 = 0;
        orientationFlags[2] = orientationFlags[1];
    } else {
        flag_0_2 = 0; //x
        flag_0_1 = 1; //z
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

    if (0 <= orientationFlags[2]){ //i.e value is positive
        //this only makes sense on the context of pointers. 
        //values_vec().reverse()
        arranged_data[0] = tri_pointer[flag_0_2 + 6]; // or +3 
        arranged_data[1] = tri_pointer[flag_0_1 + 6];
        arranged_data[3] = tri_pointer[flag_0_2 + 3];
        arranged_data[4] = tri_pointer[flag_0_1 + 3];
        arranged_data[6] = tri_pointer[flag_0_1];
        arranged_data[7] = tri_pointer[flag_0_2];
        //2 and 5 are skipped.
    } else { // value_C is negative.
        arranged_data[0] = tri_pointer[flag_0_1];
        arranged_data[1] = tri_pointer[flag_0_2];
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
    if ((arranged_data[3] - arranged_data[0]) * (adjX_val_B - arranged_data[1]) -
            (arranged_data[4] - arranged_data[1]) * (adjX_val_A - arranged_data[0]) <= 0) {
       
        flag_0_2 = 2;
        
        if ((arranged_data[flag_0_2 * 3] - arranged_data[3]) * (adjX_val_B - arranged_data[4]) -
            (arranged_data[flag_0_2 * 3 + 1] - arranged_data[4]) * (adjX_val_A - arranged_data[3]) <= 0) { //ooh some dynamic indexing with this flag_0_2 stuff here....
            
            flag_0_2 = 0;
            
            if ((arranged_data[flag_0_2 * 3] - arranged_data[6]) * (adjX_val_B - arranged_data[7]) -
                (arranged_data[flag_0_2 * 3 + 1] - arranged_data[7]) * (adjX_val_A - arranged_data[6]) <=
                FLOAT_0) {
             //THIS IS THE ONLY ONE THAT RETURNS TRUE.
                return 1;
            }
        }
    }
    return 0; //all other cases.
}


//int GetObjEnable(int j){return 1;} //don't think I actually care about this one...
int checkHitFixedMdl(int ballSize, d_coord AdjX, int roomRegion, d_coord& result_storage){
    //MATH TIME

    int resultFlag;
    d_coord position_at_collision;
    //what do these correspond to?
    float intermediateRegion7 = 0;
    float intermediateRegion8 = 0;
    float intermediateRegion5 = 0;
    float intermediateRegion6 = 0;

    int xMinusBall = ((AdjX.x - ballSize) -  intermediateRegion7)/intermediateRegion5;
    int yMinusBall = ((AdjX.y - ballSize) - intermediateRegion8)/intermediateRegion6;
    int xPlusBall = ((AdjX.x + ballSize) - intermediateRegion7)/intermediateRegion5;
    int yPlusBall = ((AdjX.y + ballSize) - intermediateRegion8)/intermediateRegion6;

    
    int regionPtr_plus4 = 0;
    int regionPtr_plus12 = 0;

    // min and max limits.
    
    xMinusBall = xMinusBall < 0 ? 0 : xMinusBall;
    yMinusBall = yMinusBall < 0 ? 0 : yMinusBall;

    xPlusBall = xPlusBall > (regionPtr_plus4 - 1) ? regionPtr_plus4-1 : xPlusBall;
    yPlusBall = yPlusBall > (regionPtr_plus12 - 1) ? regionPtr_plus12-1 : yPlusBall;

    int ballSizeSquared = pow(ballSize,2);



    int yMinusBall_Copy = yMinusBall;
    bool weirdBool = false; //This is probably "did collision occur"
    while ((yMinusBall_Copy <= yPlusBall) && (!weirdBool)){
        int roomRegion_bracket2 = 0;
        int ptrA = roomRegion_bracket2 + (xMinusBall + yMinusBall_Copy * regionPtr_plus4) * 8;
        int xMinusBall_copy = xMinusBall;
        while (xMinusBall_copy <= xPlusBall && (!weirdBool)){
            int i = 0;
            int regionPtr_bracket3 = 0;
            int ptrB = regionPtr_bracket3 + ptrA * 4; //not a ptr? the values??
            int ptrA_bracket1 = 0;
            while(i < ptrA_bracket1 && (!weirdBool)){
                float tri_ptr = roomRegion + (ptrB * 0x34); //0x34 is the size of a tri, so this probably iterates through the tris.
                std::vector<float>tri_ptr_plus_x24_floats;
                std::vector<float>tri_ptr_floats;
                std::vector<float>AdjX_floats;
                double planePoint_result = getSidePlanePoint(tri_ptr_plus_x24_floats,tri_ptr_floats,AdjX_floats); //shockingly simple. Circumstantial parameters, need to confirm that these are what I think they are.
                if (0 <= planePoint_result){
                    d_coord cpPlaneResult = getCpPlanePoint(tri_ptr+0x24,tri_ptr,AdjX); //CP IS INFLUENCED BY adjX -- does not return anything normally, but I may choose to alter this to be better style.
                    double distanceSquared = vectorSquareDistance(cpPlaneResult,AdjX);
                    if (distanceSquared < ballSizeSquared) { //if the distance squared from the proposed to the CP is less than the squared ball size then begin detailed tri checking.
                        int ChkInTri_result = chkIntri(float(0),tri_ptr,tri_ptr+0x24);//first param is adjX stuff I believe
                        if (ChkInTri_result == 0){
                            weirdBool = false;
                        } else {
                            weirdBool = true;
                            position_at_collision.x = cpPlaneResult.x;
                            //position_at_collision.z = cpPlaneResult.z;
                            position_at_collision.y = cpPlaneResult.y;
                        }
                    } else {
                        weirdBool = false;
                    }
                } else {
                    weirdBool = false;
                }
                i++;
                ptrB++;
            }
            xMinusBall_copy++;
            ptrA+=2;
        }
        yMinusBall_Copy++;
    }
    if (weirdBool){
        //if collisionLocPtr is nullptr, then return 1. Move this whole block into the one if block where weirdbool is true. then the rest of the blocks can be eliminated into a single return false statement.
        double ballAdjustment = ballSize + 0.00009999999747378752; // in the code: value: 0x38d1b717
        getPointExtensionLine(ballAdjustment,result_storage,position_at_collision,AdjX);
        resultFlag = 1;
    } else {
        //many more while loops.Some similarities with above. ICK FORGOT ABOUT THESE....
        //Two more main loops wherein there is a possibility of weirdBool becoming true. 
        //What are the differences????
        //2nd big loop:


        //all of this is identical to the loop above until the differences comment;
        weirdBool = false;
        int yMinusBall_Copy_2 = yMinusBall;
        while ((yMinusBall_Copy_2 <= yPlusBall) && (!weirdBool)){
            int roomRegion_bracket2 = 0;
            int ptrA = roomRegion_bracket2 + (xMinusBall + yMinusBall_Copy * regionPtr_plus4) * 8;
            int xMinusBall_copy_2 = xMinusBall;
            while((xMinusBall_copy_2 <= xPlusBall) && (!weirdBool)){
                int i = 0;
                int regionPtr_bracket3 = 0;
                int ptrB = regionPtr_bracket3 + ptrA * 4; //not a ptr? the values??
                int ptrA_bracket1 = 0;
                while(i < ptrA_bracket1 && (!weirdBool)){
                    float tri_ptr = roomRegion + (ptrB * 0x34);
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
                                    d_coord funkyOffset;
                                    double cpLineIntermediate = getCpLinePoint(cpLineResult,,funkyOffset,AdjX); //--------------UNFINISHED!
                                    double distanceSquared_2 = vectorSquareDistance(cpLineResult,AdjX);
                                    if (0.0 <= cpLineIntermediate && 1.0 <= cpLineIntermediate && distanceSquared_2 < ballSizeSquared){
                                        //MEANINGFUL DIFFERENCES END
                                        weirdBool = true;
                                        position_at_collision.x = cpLineResult.x;
                                        //position_at_collision.z = cpPlaneResult.z;
                                        position_at_collision.y = cpLineResult.y;
                                        //Icky gross gross gross
                                        goto badStyle;
                                    }
                                }
                                j++;
                                tri_ptr_copy+=0xC;
                                ptrC+=2;
                            }while (j < 3);
                            weirdBool = false;
                        } else {
                            weirdBool = false;
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
    if (weirdBool){
        //identical to above
        double ballAdjustment = ballSize + 0.00009999999747378752; // in the code: value: 0x38d1b717
        getPointExtensionLine(ballAdjustment,result_storage,position_at_collision,AdjX);
        resultFlag = 1;
    } else {
        //third and final loop.
        weirdBool = false;
        //no copy done -- use straight up value.
        while ((yMinusBall <= yPlusBall) && (!weirdBool))
        {
            int roomRegion_bracket2 = 0;
            int ptrA = roomRegion_bracket2 + (xMinusBall + yMinusBall_Copy * regionPtr_plus4) * 8;
            int xMinusBall_copy_3 = xMinusBall;
            while ((xMinusBall_copy_3 <= xPlusBall) && (!weirdBool)){
                int i = 0;
                int regionPtr_bracket3 = 0;
                int ptrB = regionPtr_bracket3 + ptrA * 4; //not a ptr? the values??
                int ptrA_bracket1 = 0;
                //up until this loop below, all three sets use the same setup.
                while((i < ptrA_bracket1) && (!weirdBool)){
                    float tri_ptr = roomRegion + (ptrB * 0x34);
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
                                if (((val_at_tri_ptr_plus30 & ptrC) != 0) && ((val_at_tri_ptr_plus30 & local_d8) != 0) && (distanceSquared_3 < ballSizeSquared) ){
                                    weirdBool = true;
                                    int posAtCol_ptr = tri_ptr + j_2 * 0xC;
                                    position_at_collision.x = posAtCol_ptr;
                                    //position_at_collision.z = posAtCol_ptr[1];
                                    position_at_collision.y = posAtCol_ptr+0x8; //[2]
                                    //ICK A GOTO.... How dare you ghidra.
                                    goto badStyle2;
                                }
                                j_2++;
                                tri_ptr_3_copy.x += 0xC; //applies to ptr
                                ptrC += 2;
                                /* code */
                            } while (j_2 < 3);
                            weirdBool = false;
                        } 
                        else {
                            weirdBool = false;
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
        if (weirdBool){
            //identical to above
            double ballAdjustment = ballSize + 0.00009999999747378752; // in the code: value: 0x38d1b717
            getPointExtensionLine(ballAdjustment,result_storage,position_at_collision,AdjX);
            resultFlag = 1;
        } else {
            resultFlag = 0;
        }
    }
    return resultFlag;
}
}


bool checkHitCollision(int ballSize, d_coord store_A, d_coord store_B, d_coord& result_storage,const bool hardCoded_0 = 0){
    //Lots of ptr setup and data fetching.
    //Result is the location of the room data and the region of the room?
    int room = 0;
    int region = 0;
    
    d_coord adjustedSubWithDivResult = store_B;
    d_coord localResultStorage;
    int firstLim = 0;
    //int secondLim = 8;//Wtf is this actually representing? its roomPtr[1]... maybe num regions?
    int thirdLim = 0;
    //Why have all these limits and checks?
    int i = 0;
    do
    {
        firstLim = 0;
        int room_Copy = room;
        //Also copies roomDataPtr
        for (int j = 0; j < 8; j++){
            //int ObjEnableResult = GetObjEnable(j); //Don't think I need this, reevaluate if its possible to get something else. 
            //if (ObjEnableResult != 0){
                int collisionResultFlag = checkHitFixedMdl(ballSize,adjustedSubWithDivResult,region,localResultStorage);
                if (collisionResultFlag != 0){
                    if (result_storage.x == 0){//in the code it's if this is nullptr. Should not need this in my code.
                        return true;
                    }
                    thirdLim++;
                }
            //}
            room_Copy += 0x40; //scanning rooms? regions?
        }
    } while ((firstLim > 0) && (i = i + 1, i < 10));
    if (i > 0){
        result_storage = localResultStorage; //commit results.
        return true;
    } else {
        return false; //do not commit results to result_storage.
    }
}

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

    d_coord dataStore_A = old;
    d_coord dataStore_B = vectorSub(proposed,old); //Can compress further if needed.
       
        bool collisionResultFlag = checkHitCollision(ballSize,dataStore_A,dataStore_B,adjPosLoc,0);
        if(collisionResultFlag){
            return 1;//This boils down to prepping dataStoreA and dataStoreB and returning the adjPosition again, just like last func.
        }
    
    return 0;
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




