#include "../processCoreLocal.h"









double vecMathDistance(float* ref_Old_CurrentX, float* ref_New_CurrentX){
  double dx = pow(*ref_Old_CurrentX - *ref_New_CurrentX,2);
  double dz = pow(ref_Old_CurrentX[1] - ref_New_CurrentX[1],2);
  double dy = pow(ref_Old_CurrentX[2] - ref_New_CurrentX[2],2);
  return sqrt(dx + dz + dy); //PSVECTORDISTANCE.
}


double vecMathScale(double scalingFactor,float *param_2,float *param_3){
  float *pfVar1;
  double dVar3;
  double dVar4;
  double dVar5;
  double dVar6;
  double dVar7;
  
  dVar4 = (double)*param_2;
  dVar6 = (double)param_2[1];
  pfVar1 = param_2 + 2;
  dVar5 = (double)*pfVar1;
  dVar7 = 1.0;
  dVar4 = dVar4 * scalingFactor;
  dVar3 = 1.0;
  *param_3 = (float)dVar4;
  param_3[1] = (float)dVar6;
  dVar5 = dVar5 * scalingFactor;
  dVar7 = dVar7 * scalingFactor;
  pfVar1 = param_3 + 2;
  dVar4 = 1.0;
  *pfVar1 = (float)dVar5;
  return;
}

double vecMathScale2(double scalingFactor,float *param_2,float *result_Location){
  double p20 = (double)*param_2;
  double p22 = (double)param_2[2];

  p20 = (double)*param_2;
  p22 = (double)param_2[2];
  

  result_Location[0] = (float)p20 * scalingFactor;
  result_Location[1] = (float)(double)param_2[1];
  result_Location[2] = (float)p22*scalingFactor;


  return;
}
// double vecMathSubtract(){
// return 0;
// }
double vecMathAdd(float *param_1,float *param_2,float *param_3){ //p sure this could be written as returning a tuple since in game, theyre stored to memory and this is a void
  float *pfVar1;
  double dVar3;
  double dVar4;
  double dVar5;
  double dVar6;
  
  dVar3 = (double)*param_1;
  dVar5 = (double)param_1[1];
  dVar4 = (double)*param_2;
  dVar6 = (double)param_2[1];
  dVar3 = dVar3 + dVar4;
  dVar5 = dVar5 + dVar6;
  dVar4 = 1.0;
  *param_3 = (float)dVar3;
  param_3[1] = (float)dVar5;
  pfVar1 = param_1 + 2;
  dVar3 = (double)*pfVar1;
  dVar5 = 1.0;
  pfVar1 = param_2 + 2;
  dVar4 = (double)*pfVar1;
  dVar6 = 1.0;
  dVar3 = dVar3 + dVar4;
  dVar5 = dVar5 + dVar6;
  pfVar1 = param_3 + 2;
  dVar4 = 1.0;
  *pfVar1 = (float)dVar3;
  return;
}
void vecMathAdd2(float *param_1,float *param_2,float *resultLocation){ //3rd param is result parameter? Need to add byRef or is this good enough?
  double p10 = (double)*param_1;
  double p11 = (double)param_1[1];
  double p12 = (double)param_1[2];
  double p20 = (double)*param_2;
  double p21 = (double)param_2[1];
  double p22 = (double)param_2[2];

  *resultLocation = (float)(p10+p20);
  resultLocation[1] = (float)(p11+p21);
  resultLocation[2] = (float)(p12+p22);
  return;
}

void vecMathSUB2(float *param_1,float *param_2,float *resultLocation){ //3rd param is result parameter? Need to add byRef or is this good enough?
  double p10 = (double)*param_1;
  double p11 = (double)param_1[1];
  double p12 = (double)param_1[2];
  double p20 = (double)*param_2;
  double p21 = (double)param_2[1];
  double p22 = (double)param_2[2];

  *resultLocation = (float)(p10+p20);
  resultLocation[1] = (float)(p11+p21);
  resultLocation[2] = (float)(p12+p22);
  return;
}

int main(){

  float x = -12.3776655197143;
  float y = 5.133345127;
  // float z = 20.00330924987793;
  float z = 28.50330924987793;

  //For the 2nd psvecdist we look at, it doesn't acknowledge this "distance" in Z. 

  float nx = -12.94873428;
  float ny = 5.238367081;
  float nz = 28.50330924987793; //supposedly

  float ox[3] ={x,z,y};
  float nxr[3] = {nx,nz,ny};
  double res4 = vecMathDistance(ox,nxr);
  std::cout << std::dec << "Result: " << std::setprecision(17) << res4;

  // NPC kaib    = NPC({85,-150}, "K");
  
  // d_coord xd;
  // xd.x = 100;
  // xd.y = 150;

  // d_coord nxd;
  // nxd.x = 120;
  // nxd.y = 170;

  // d_coord dist;
  // dist.x = nxd.x - xd.x;
  // dist.y = nxd.y - xd.y;


  // double res2 = kaib.pythagorasDistance(dist);
  // std::cout << "NPC RES: " << res2;

  return 0;
}
/*

5.23836612701416 r3
28.50330924987793 r3

4.99890661239624 r4
28.503307342529297 r4
--
=
0.23945951461791992
0.0000019073486328125

2nd

-12.948734283447266 r3 -- new
1 r3

-15.920820236206055 r4 -- big val
1 r4

--
=
2.972085952758789




-----------
Add:
0.24093584716320038   r3
0.000001919107944559073 r3

4.99890661239624 r4
28.503307342529297 r4
++
=
5.239842414855957 -- post Col X
28.50330924987793


2nd

2.9904096126556396 r3
1 r3

-15.920820236206055 r4
1 r4

++
=
-12.930410385131836 -- post col Y
2 r4








*/
