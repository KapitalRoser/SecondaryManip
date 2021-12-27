#include "../../processCore.h"
using namespace std;
typedef std::vector<int>::iterator iter;
iter flexSearch ( iter first1, iter last1, iter first2, iter last2, bool (*pred)(int,int,int),int flexValue){
  if (first2==last2) return first1; 
  while (first1!=last1){
    iter it1 = first1;
    iter it2 = first2;
    while (pred(*it1,*it2,flexValue)) {
        ++it1; ++it2;
        if (it2==last2) return first1;
        if (it1==last1) return last1;
    }
    ++first1;
    //function courtesy of the STL search algorithm. 
  }
  return last1;
}
bool flexPredicate (int a, int b, int flexValue){
    return (a >= b-flexValue && a <= b+flexValue);
}

int main(int argc, char** argv)
{
  //better but I want to compare with a certain amount of leeway.
  int flexValue = 10;
  std::vector<int> elems = {52, 2, 6, 9, 9, 100, 6,6,9,421,13,404,6,9,1,1,1,1,6,9};
  std::vector<int> results;

  vector<int> myVector(elems.begin(), elems.end()); 
  iter it;  

  // Find the first subsequence
  vector<int>sub2 = {6,9};
  //it = flexSearch(myVector.begin(), myVector.end(), sub2.begin(), sub2.end(),flexPredicate,flexValue);
  //it = search(myVector.begin(),myVector.end(),sub2.begin(),sub2.end());
  int updateIdx = 0;
  while (it != myVector.end())
  { 
    it = flexSearch(myVector.begin()+updateIdx,myVector.end(),sub2.begin(),sub2.end(),flexPredicate,flexValue); //search algorithm
    updateIdx = it-myVector.begin(); //position of result
    std::cout << "Update Idx: " << updateIdx << std::endl; //debug
    if (it != myVector.end()){
      results.push_back(updateIdx);
      debugPrintVec(results);
      updateIdx++; //To make sure new comparison doesn't return immediately.
    }// else break
  }
  if (results.back() == myVector.size()){
    std::cout << "NOT FOUND!";
  } else {
    std::cout << "Found subsequence ";
    debugPrintVec(sub2);
    std::cout << "At position(s) ";
    debugPrintVec(results); 
  }
  
  
  

  // if (it != myVector.end()) {
  //   cout << "Found subsequence 6, 9 at position " << it - myVector.begin()
  //  << endl;
  // } else {
  //     cout << "NOT FOUND!\n";
  // }

  return (0);
}