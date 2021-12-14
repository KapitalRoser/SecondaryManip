#include <iostream>


int main() {
    int count = 0;
    for ( uint64_t i = 0; i <= UINT32_MAX; i++)
    {   
        uint16_t hiSeed = i >> 16;
         if (static_cast<double>(hiSeed) / 65536.0 < 0.1){
            count++;
         }
         if (i % 1000000 == 0){
             std::cout << i << " seeds processed! " << UINT32_MAX - i << " seeds to go!\n";
         }
    }
    std::cout << "Result: " << count << ". This is " << static_cast<float>(count / UINT32_MAX) << "% of all seeds";


    return 0;
}