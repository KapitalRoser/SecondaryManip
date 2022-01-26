#include <iostream>
#include <map>

using namespace std;

int main (){
  map<int,int> first;
  
  //initializing
  first[1]=1;
  first[98]=1;
  first[201]=0;
  
   map<int, int>::iterator it;
   for(it=first.begin(); it!=first.end(); ++it){
      cout << it->first << " => " << it->second << '\n';
   }
   
  return 0;
}