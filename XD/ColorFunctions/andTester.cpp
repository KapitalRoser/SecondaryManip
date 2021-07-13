#include <iostream>
#include <cmath>

using namespace std;

int main(){


    uint64_t input = 65535;
    long long modpow = pow(2,2);
    cout << modpow << " ";
    //an & is equivalen to a MOD 2^whatever is being

    cout << (input & 2) << " " << (input >> modpow) << " " << (input % 3 );

    return 0;
}