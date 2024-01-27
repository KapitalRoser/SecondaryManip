// #include <iostream>
// #include <vector>
// #include <fstream>


// float convertToFloat(uint32_t val){
//     float result;
//     memcpy(&result,&val,sizeof(float));
//     return result;
// }
// float swapEndiannessAndConvertToFloat(const std::vector<unsigned char>& bytes) {
//     // Assuming the size of a float is 4 bytes
//     uint32_t swappedValue = 0;
//     const int floatSize = sizeof(float);
//     static_assert(floatSize == 4, "Size of float must be 4 bytes");
//     // Swap endianness using bitwise operations
//     for (size_t i = 0; i < floatSize; ++i) { //This is because std::byte is a 8-byte type...
//         swappedValue |= static_cast<uint32_t>(bytes[i]);
//     }
//     return convertToFloat(swappedValue);
// }

// std::vector<unsigned char> readFile(const std::string& filename) {
//     std::ifstream file(filename, std::ios::binary);
//     if (!file.is_open()) {
//         throw std::runtime_error("Failed to open file: " + filename);
//     }

//     std::vector<unsigned char> data(std::istreambuf_iterator<char>(file), {});
//     return data;
// }

// std::vector<unsigned char> getXBytesAtX(const std::vector<unsigned char>& data, size_t length, size_t offset) {
//     return std::vector<unsigned char>(data.begin() + data.size() - offset, data.end());
// }

// uint32_t xByte(const std::vector<unsigned char>& data, size_t offset) {
//     uint32_t value = 0;
//     float f = swapEndiannessAndConvertToFloat(data);
//     for (size_t i = 0; i < 4; ++i) {
//         value |= static_cast<uint32_t>(data[offset]) << (8*(4-1-i));
//     }
//     return value;
// }



// void printX(const std::vector<unsigned char>& data, size_t length, size_t offset) {
//     for (size_t i = 0; i < length; ++i) {
//         std::cout << std::hex << static_cast<int>(data[offset + i]) << std::dec;
//     }
// }

// float readFloat(const std::vector<unsigned char>& data, size_t offset) {
//     float value;
//     memcpy(&value, &data[offset], sizeof(float));
//     return value;
// }

// std::vector<float> readVector(const std::vector<unsigned char>& data, size_t offset) {
//     std::vector<float> vec(3);
//     for (size_t i = 0; i < 3; ++i) {
//         vec[i] = readFloat(data, offset + i * 0x4);
//     }
//     return vec;
// }

// void printMesh(const std::vector<unsigned char>& data);

// int main() {
//     std::vector<unsigned char> data;
//     size_t s_offset = 0x0;

//     try {
//         // "E:\\Program Files\\Dolphin\\Collision\\Test Files\\M5\\M5_apart_2F\\M5_apart_2F#14.fdat"
//         data = readFile("C:\\Users\\Carter\\Documents\\ROMS\\Pokemon Colosseum (USA)\\Pokemon Colosseum (USA) CM Tool\\Game Files\\M1_shop_2F\\M1_shop_2F.ccd");
//     } catch (const std::exception& e) {
//         std::cout << e.what() << std::endl;
//         return 1;
//     }
//     std::cout << "data size: " << data.size() << "\n";
//     if (s_offset > 0) {
//         data = getXBytesAtX(data, data.size() - s_offset, s_offset);
//     }

//     printMesh(data);

//     return 0;
// }

// void printMesh(const std::vector<unsigned char>& data){
// 		uint32_t list_start = xByte(data,0x0);
// 		uint32_t entry_count = xByte(data,0x4);
// 		int entry_size = 0x40;
// 		int v_c = 1;
// 		int o_c = 0;
// 		int g_c = 1;
// 		for(int i = 0;i<entry_count;i++){
// 			std::cout << "o " << o_c << std::endl; //OBJECT? ENTRY?
// 			int offset = list_start+(entry_size*i);
// 			for(int o=0x24;o<entry_size;o+=0x4){
// 			    uint32_t a_o;
// 				if((a_o=xByte(data,o+offset))>0){
// 					//TODO 
//                     std::cout << "g " << o_c << "_" << "_" << std::hex << std::uppercase << o << std::dec << std::endl; //SECTION
// 					/*
// 					 * uint32 offset to data
// 					 * uint32 number of data entries
// 					 * uint32 offset to list1
// 					 * uint32 offset to list2
// 					 * uint16 ? some kind of dimension ?
// 					 * uint16 ? some kind of dimension ?
// 					 * float32 (whole number #40)
// 					 * float32 (whole number #40)
// 					 * float32 ( #80)
// 					 * float32
// 					 */
// 					int data_start = xByte(data,a_o);
// 					int num = xByte(data,a_o+0x4);
// 					int face_size = 0x34;
// 					std::cout << std::endl;
// 					std::cout << "#offset " << std::hex << a_o << std::dec << std::endl;
// 					for(int s=0;s<num;s++){
// 						//TODO
// 						//System.out.println("g "+Integer.toHexString(a_o)+"_"+Integer.toHexString(s));
// 						int s_o = data_start +(s*face_size);
// 						for(int c=0;c<4;c++){

// 							std::vector<float> f = readVector(data,s_o+c*0xC);
// 							if(c<3){
//                                 std::cout << "v" << f[0] << " " << f[1] << " " << f[2] <<std::endl;
// 							}else{
//                                 std::cout << "#n" << f[0] << " " << f[1] << " " << f[2] <<std::endl;
// 							}
// 						}
//                         std::cout << "l " << v_c << " " << (v_c + 1) << std::endl;
//                         std::cout << "l " << (v_c + 1) << " " << (v_c + 2) << std::endl;
//                         std::cout << "l " << (v_c + 2) << " " << v_c << std::endl;
// 						v_c+=3;

//                         std::cout << "#";
// 						printX(data,4,s_o+0x30);
// 						std::cout << "\n";
// 					}
// 				}
// 			}
// 			g_c = 1;
// 			o_c++;
// 		}
// 	}
