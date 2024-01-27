
// /* __stdcall _peopleMoveTypeRandomWalk(tagPeopleWork *) */

// #include <iostream>
// #include <cmath>

// //This is very similar to the Colo one for obvious reasons. However there are some differences, 
// //and massaging this doc is an attempt to highlight the differences as well as lines that I may have missed when I did Colo.

// void zz_character_data_get_movement_vector_b (int* a, int* b) {

// }
// double HSD_Randf(){
//   //LCGGetPercentage
// }
// unsigned int _peopleMoveSub(int* ptr){
//   //Aka NPC STEP LOGIC
// }
// double GScolsys2HumanGetWalkHeight(int* ptr){

// }
// bool GSmodelHasAnimationEnded (unsigned int* ptr){
//   return (*ptr >> 0xe) & 1;
// }
// void _peopleSetMoveAnimation(int* ptr){

// }
// void peopleViewerDisplayRandWalk(){
  
// }
// void peopleRotateToAngle(double param_1, double param_2,int a, int b){
//   //This seems to be exclusive to XD. A and B are apparently passed to it but the func signature doesn't match.
// }
// int zz_character_data_get_coordinates(int* ptr){
//   //gets a pointer to coordinates, hard coded distance from ptr.
// }
// void _peopleMoveTypeRandomWalk(int* npc_ArrPtr)

// {
//   bool b_AnimEnded;
//   float *pfVar1;
//   int walkFlagLocal;
//   int iteratorVar;
//   int iVar3;
//   double waitTimerDecrement;
//   double dVar3;
//   double cycleVariance;
//   double __x;
//   double dVar4;
//   double double_6_var;
//   double double_Float_1_or_2;
//   float float_0_var;
  
//   unsigned int stateFlag;

//   const float FLOAT_1 = 1.0;
//   const float FLOAT_2X_PI = 6.2831855;
//   const float FLOAT_2 = 2.0;
//   const float FLOAT_0 = 0;
//   const float FLOAT_PI_APPROX = 3.1415927;
//   const float FLOAT_0_1 = 0.1;


  

//   double_Float_1_or_2 = (double)FLOAT_1;
//   stateFlag = *(unsigned int *)(npc_ArrPtr + 0x55);
//   if (stateFlag == 1) {
// BeginStepFind:
//     zz_character_data_get_movement_vector_b(npc_ArrPtr,npc_ArrPtr + 0x5c);
//     zz_character_data_get_movement_vector_b(npc_ArrPtr,npc_ArrPtr + 0x68);

//     dVar4 = (double)FLOAT_2X_PI;
//     iteratorVar = 0;
//     double_6_var = (double)(float)6.0;

//     do {
//       dVar3 = HSD_Randf();
//       //find X destination
//       __x = (double)(float)(dVar4 * dVar3); //2 * pi * rand.
//       *(float *)(npc_ArrPtr + 0x68) =
//            *(float *)(npc_ArrPtr + 0x8c) * (float)sin(__x); + *(float *)(npc_ArrPtr + 0x80); //+68 is x dest. Not calculated is the z value in the middle.
//       *(float *)(npc_ArrPtr + 0x70) =
//            *(float *)(npc_ArrPtr + 0x8c) * (float)cos(__x) + *(float *)(npc_ArrPtr + 0x88); //+70 is y dest.

//       dVar3 = GScolsys2HumanGetWalkHeight(npc_ArrPtr + 0x68);
//       *(float *)(npc_ArrPtr + 0x6c) = (float)dVar3;

//       dVar3 = (double)zz_0297a30_(npc_ArrPtr + 0x68,npc_ArrPtr + 0x5c);
//       if ((double)(float)(double_6_var * (double)FLOAT_1) < dVar3) break;

//       iteratorVar = iteratorVar + 1;
//     } while (iteratorVar < 10);

//     iVar3 = zz_character_data_get_coordinates(npc_ArrPtr);
//     double YDistance = (double)(float)((double)*(float *)(npc_ArrPtr + 0x70) - (double)*(float *)(iVar3 + 8));
//     pfVar1 = (float *)zz_character_data_get_coordinates((int *)(iVar3 + 8));
//     double XDistance = (double)(*(float *)(npc_ArrPtr + 0x68) - *pfVar1);
//     double Atan2Result = atan2(XDistance,YDistance); //ANGLE.

//     if (FLOAT_0_1 + *(float *)(npc_ArrPtr + 0x20) < *(float *)(npc_ArrPtr + 0x58)) {
//       double_Float_1_or_2 = (double)FLOAT_2;
//     }

//     peopleRotateToAngle((double)(float)Atan2Result,
//                         double_Float_1_or_2,
//                         *(int *)(npc_ArrPtr + 0x14),
//                         *(int *)(npc_ArrPtr + 0x18));
                        
//     _peopleSetMoveAnimation(npc_ArrPtr);
//     *(unsigned int *)(npc_ArrPtr + 0x55) = 2;
//   }
//   else {
//     if (stateFlag == 0) {
//       if (false) goto RETURN;
//       peopleViewerDisplayRandWalk();
//                     /* Evaluate TIMER for next cycle */
//       if (FLOAT_0 < *(float *)(npc_ArrPtr + 0x90)) {
//         waitTimerDecrement = (double)zz_02a04d8_();
//         float_0_var = FLOAT_0;
//                     /* Decrement WaitTimer */
//         *(float *)(npc_ArrPtr + 0x90) =
//              (float)((double)*(float *)(npc_ArrPtr + 0x90) - waitTimerDecrement);
//         if (float_0_var < *(float *)(npc_ArrPtr + 0x90)) goto RETURN;
//                     /* If waitTimer <= 0, snap to 0 */
//         *(float *)(npc_ArrPtr + 0x90) = float_0_var;
//       }
//       b_AnimEnded = GSmodelHasAnimationEnded(*(unsigned int **)(npc_ArrPtr + 8));
//       if (!b_AnimEnded) goto RETURN;
//                     /* SET FLAG TO START WALK */
//       *(unsigned int *)(npc_ArrPtr + 0x55) = 1;
//       goto BeginStepFind;
//     }
//                     /* catch - returns if state flag above 2 for any reason */
//     if (2 < stateFlag) goto RETURN;
//   }
//   walkFlagLocal = _peopleMoveSub(npc_ArrPtr);
//   if (walkFlagLocal != 0) {
//     if ((*(bool *)(npc_ArrPtr + 0x17e) & 1) != 0) {
//       if (FLOAT_0_1 + *(float *)(npc_ArrPtr + 0x20) < *(float *)(npc_ArrPtr + 0x58)) {
//         double_Float_1_or_2 = (double)FLOAT_2;
//       }
//       peopleRotateToAngle((double)(FLOAT_PI_APPROX + *(float *)(npc_ArrPtr + 0x40)),
//                           double_Float_1_or_2,
//                           *(int *)(npc_ArrPtr + 0x14),
//                           *(int *)(npc_ArrPtr + 0x18));
//     }
//     cycleVariance = (double)HSD_Randf();
//                     /* Wait Timer calculation happens here */
//     *(float *)(npc_ArrPtr + 0x90) =
//          *(float *)(npc_ArrPtr + 0x98) * (float)((double)FLOAT_2 * cycleVariance - (double)FLOAT_1) +
//          *(float *)(npc_ArrPtr + 0x94);
//     *(unsigned int *)(npc_ArrPtr + 0x55) = 0;
//   }
// RETURN:
//   return;
// }

