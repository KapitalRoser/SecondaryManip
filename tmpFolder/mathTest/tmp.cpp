// #include <iostream>
// #include <cstdint>
// #include <vector>
// #include <cstring>

// // Function to swap the endianness of a sequence of bytes and convert to a float

// float convertToFloat(uint32_t val){
//     float result;
//     std::memcpy(&result,&val,sizeof(float));
//     return result;
// }
// float swapEndiannessAndConvertToFloat(const std::vector<std::byte>& bytes) {
//     // Assuming the size of a float is 4 bytes
//     uint32_t swappedValue = 0;
//     const int floatSize = sizeof(float);
//     static_assert(floatSize == 4, "Size of float must be 4 bytes");
//     // Swap endianness using bitwise operations
//     for (size_t i = 0; i < floatSize; ++i) { //This is because std::byte is a 8-byte type...
//         swappedValue |= static_cast<uint32_t>(bytes[i]) << (8 * (floatSize - 1 - i));
//     }
//     return convertToFloat(swappedValue);
// }
// int main() {
//     // Example of swapping endianness and converting to float using std::vector<std::byte>
//     std::vector<std::byte> bytes = { std::byte(0x42), std::byte(0xC8), std::byte(0x80), std::byte(0x00) };
//     float result = swapEndiannessAndConvertToFloat(bytes);

//     std::cout << "Original bytes: ";
//     for (const auto& byte : bytes) {
//         std::cout << std::hex << static_cast<int>(byte) << " ";
//     }
//     std::cout << std::endl;

//     std::cout << "Swapped and converted value: " << result << std::endl;

//     return 0;
// }


