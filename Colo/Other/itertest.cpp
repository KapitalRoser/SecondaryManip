#include "../../processCore.h"

int main(){

    std::vector<int> vec = {100,80,70,60,50};
    for (std::vector<int>::iterator iter = vec.begin()+2; iter  < vec.end(); iter++){
        std::cout << *iter << std::endl;
    }

    return 0;
}