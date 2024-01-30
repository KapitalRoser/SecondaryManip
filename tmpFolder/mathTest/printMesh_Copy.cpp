#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <cstddef>
#include <algorithm>
typedef uint32_t u32;

std::vector<std::byte> getXBytesAtX(const std::vector<std::byte>& data, size_t length, size_t offset) {
    return std::vector<std::byte>(data.begin() + offset, data.begin()+offset+length);
}

float convertToFloat(u32 val){
    float result;
    memcpy(&result,&val,sizeof(float));
    return result;
}
u32 swapEndiannessForWord(const std::vector<std::byte>& bytes, int result_Size = 4) {
    u32 swappedValue = 0;
    for (size_t i = 0; i < result_Size; ++i) {
        swappedValue |= static_cast<u32>(bytes[i]) << (8 * (result_Size - 1 - i));
    }
    return swappedValue;
}


u32 getWord(const std::vector<std::byte>&data,int offset){
    std::vector<std::byte> slice = getXBytesAtX(data,4,offset);
    return swapEndiannessForWord(slice); 
    
}
float getFloatFromWord(u32 word){
    return convertToFloat(word);
}
std::vector<std::byte> readFile(const std::string& filename) {
    std::ifstream inputFile(filename, std::ios_base::binary);
    inputFile.seekg(0, std::ios_base::end);
    auto length = inputFile.tellg();
    inputFile.seekg(0, std::ios_base::beg);
    std::vector<std::byte> buffer(length);
    inputFile.read(reinterpret_cast<char*>(buffer.data()), length);
    inputFile.close();
    return buffer;
}

void printBytes(const std::vector<std::byte>&data, int offset, int n, int breakAfter = 0){
    for (int i = 0; i < n; i++)
    {
        std::cout << std::hex << std::setw(2) << std::to_integer<int>(data[offset+i]) << " ";
        if (breakAfter > 0 && i > 0 && (i+1) % breakAfter == 0){
            std::cout << "\n";
        }
        if (i > 0 && (i+1) % 0x40 == 0){
            std::cout << std::endl;
        }
    }
}

void printTri(const std::vector<std::byte>&data, int data_start){
    int word_size = 4;
    printBytes(data,data_start,0x24);
    std::cout << "| ";
    printBytes(data,data_start+0x24,12);
    std::cout << "| ";
    printBytes(data,data_start+0x30,4);
    std::cout << "\nConverted to: \n";
    for (int i = 0; i < 0x3; i++){
        std::cout 
    << "( " << std::setw(9)
    << getFloatFromWord(getWord(data,data_start+(i*0xC))) << ", " << std::setw(9)
    << getFloatFromWord(getWord(data,data_start+0x4+(i*0xC))) << ", " << std::setw(9)
    << getFloatFromWord(getWord(data,data_start+0x8+(i*0xC)))
    << " )\n";
    }
    //aka normals
    std::cout << "orientation data: " << getFloatFromWord(getWord(data,data_start+0x24)) << ", " 
                                      << getFloatFromWord(getWord(data,data_start+0x28)) << "," 
                                      << getFloatFromWord(getWord(data,data_start+0x2C)) << "\n";
    std::cout << "interaction flags: ";
    for (int i = 0; i < 4; i++)
    {
        std::cout << std::setw(2) << std::to_integer<int>(data[data_start+0x30+i]); //really should have a better conversion for halfwords than this.
        if (i == 1){
            std::cout << ", ";
        }
    }
    
    std::cout << std::endl;
}

int printTriSet(const std::vector<std::byte>&data, int offset, int triCount = 0xC, bool fullMode = true){
    int tri_size = 0x34;
    int num_tris = triCount; //needs to get read from somewhere I think?
    int workingOffset = offset; //updated by functions.
    for (int currentTri = 0; currentTri < num_tris; currentTri++){
        std::cout << "TRI: " << std::dec << currentTri << ". At: " << std::hex << workingOffset << ".\n";
        if (fullMode){
            printTri(data, workingOffset);
        }
        workingOffset += tri_size;
    }
    return workingOffset;
}

void printRegion(const std::vector<std::byte>&data, int custom_offset,bool fullMode = true){
    int object_offset = custom_offset;
    int tri_size = 0x34;
    int tri_offset = getWord(data,object_offset);
    int num_tris = getWord(data,object_offset+0x4); 
    int ptrA = getWord(data,object_offset+0x8);
    int next_tri = ptrA + (getWord(data,ptrA+0x4)*4) + 0x8;
    //std::cout << "NEXT TRI: " << std::hex << next_tri << "\n";
    std::cout << "\nNUM TRIS: " << num_tris << "\n"; 
    printTriSet(data,tri_offset,num_tris,fullMode);
    //return next_tri;
}
// void manualTest(const std::vector<std::byte>&data){
//     u32 custom_offset = 0x1F38;//0x2094 -- should be 1F54 but is
//     custom_offset = 0x210;
//     int offset = custom_offset;
//     for (size_t i = 0; i < 1; i++)
//     {
//         std::cout << "SET: " << i << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
//         offset = printRegion(data,offset,false);
//     } 
// }

void printAllRegions(const std::vector<std::byte>&data, const std::vector<int>&offsetList){
    for (int i = 0; i < offsetList.size(); i++)
    {
        std::cout << "REGION: " << std::dec << i << " AT: " << std::hex <<  offsetList[i] << std::dec <<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<< std::endl;
        printRegion(data,offsetList[i],false);
    }
    
}

int main() {
    std::vector<std::byte> data = readFile("C:\\Users\\Carter\\Documents\\ROMS\\Pokemon Colosseum (USA)\\Pokemon Colosseum (USA) CM Tool\\Game Files\\M1_shop_2F\\M1_shop_2F.ccd"); 
    //manualTest(data);



    // int list_start = getWord(data,0x0);
    // int entry_count = getWord(data,0x4);
    // int entry_size = 0x40;

    std::cout << "LIST OF OBJECT OFFSETS: \n";
    printBytes(data,0x10,0x200,4);
    

    //read region
    int file_start = getWord(data,0x0);
    int entry_count = getWord(data,0x4);
    int entry_size = 0x40;
    
    //note that these objects are not necessarily ordered by offset.
    int first_entry = 0x38;

    std::vector<std::vector<int>>sections;
    std::vector<int> offsetList;
    std::vector<int> offsets = {};
    for (int i = file_start; i < 0x200; i+= 0x4){
        int datum = getWord(data,i);
        if (datum != 0 && datum != 0x3F800000){ //3F800000 == 1 in float but im too lazy to do the float conversion just for this routine check.
            offsets.push_back(datum); //tmp
            offsetList.push_back(datum); //perma
        }
        if (i > 0x10 && i % 0x40 == 0x10){
            sections.push_back(offsets);
            offsets = {};
        }
    }
    sections.push_back(offsets);
    //offsets.push
    //sections.push.
    //offsets = {}
    for (int i = 0; i < sections.size(); i++){
        std::cout << "SECTION: " << i << "\n";
        for (auto &&i : sections[i])
        {
            std::cout << std::hex << "0x" << i  << std::dec << std::endl;
        }
        
    }
    //std::sort(offsetList.begin(),offsetList.end()); //make sure the num is fairly small.
    //std::cout << "NUM_OBJECTS: " << offsetList.size() << " SORTED!\n"; //Disable this and the prev line if wanting to preserve original sections
    //printAllRegions(data,offsetList);
    printRegion(data,offsetList[0],false);
    
    
    
    return 0;
}


// u32 list_start = 0x0;
    // u32 entry_count = getWord(data,0x4);
    // u32 entry_size = 0x40;
    // u32 g_something = 1;
    // for (u32 entry_o = 0; entry_o < entry_count; entry_o++){
    //     std::cout << "ENTRY: " << entry_o << "\n"; 
    //     u32 entry_offset = list_start+(entry_size*entry_o);
    //     //loop
    //     for (int offset_j = 0x24;offset_j<entry_size;offset_j+=0x4){
    //         u32 inner_entry_o = getWord(data,entry_offset); //is this supposed to represent a ptr?
    //         // std::cout << "A_O: " << std::hex << inner_entry_o << std::dec << "\n";
    //         if (inner_entry_o > 0){
    //             bool isInteractable = (inner_entry_o == 0x2c || inner_entry_o == 0x30);
                
    //             u32 face_start = getWord(data,inner_entry_o);
    //             u32 faceNum = getWord(data,inner_entry_o+0x4);
    //             u32 face_size = 0x34;
    //             std::cout << "Entry: " << entry_o << ". Group: " << std::hex << offset_j << std::dec << ". Num faces: " << faceNum << std::endl;
    //             for (u32 face_i = 0; face_i < faceNum; face_i++){
    //                 u32 face_i_offset = face_start + (face_i*face_size);
    //                 //tri time.
    //                 for(u32 corner = 0; corner < 4; corner++){
    //                     float vx = getFloatFromWord(getWord(data,face_i_offset+(corner*0xC)+0x0));
    //                     float vz = getFloatFromWord(getWord(data,face_i_offset+(corner*0xC)+0x4));
    //                     float vy = getFloatFromWord(getWord(data,face_i_offset+(corner*0xC)+0x8));
    //                 }
    //             }
    //         }
    //     }
    //     g_something = 1;
    // }
    // int object_offset = custom_offset;
    // int tri_size = 0x34;
    // int tri_offset = getWord(data,object_offset);
    // // std::cout << "TRI_OFF: " << std::hex << tri_offset;
    // int num_tris = getWord(data,object_offset+0x4); 
    // int ptrA = getWord(data,object_offset+0x8);
    // // std::cout << "PtrA: " << ptrA << std::dec;
    // int amtGap = getWord(data,ptrA+0x4);
    // // std::cout << "AMTGAP:" << amtGap;
    // // std::cout << std::hex << "TRI OFFSET: " << tri_offset << "NUM TRIS: " << num_tris << "GAP_PTR: " << ptrA << "Amt: " << amtGap;
    // amtGap *= 4;
    // ptrA += amtGap +0x8;
    // // std::cout << "adj ptrA: " << std::hex << ptrA;
    // int next_tri = getWord(data,ptrA);
    // // std::cout << "next: " << next_tri << "\n";

    // int workingOffset = tri_offset; //The word at the location specified in custom_offset contains the offset of the start of the tri.
    // //What is this in relation to the next tri? There's prob a better way of doing this lol.   
    // workingOffset = printTriSet(data,workingOffset);
    // std::cout << "LAST OFFSET: " << std::hex << workingOffset << std::dec << std::endl;
    // std::cout << "\n~~~~~~2nd SET:~~~~~~~~~~\n";
    // workingOffset+= 0x5C;
    // num_tris = std::to_integer<int>(data[workingOffset-0x1D]);
    // workingOffset = printTriSet(data,workingOffset, num_tris);
    // printTri(data, custom_offset);
    // printTri(data,custom_offset+0x34); //That tri's normal.std::cout << "NUM TRIS: " << num_tris << "\n";
       