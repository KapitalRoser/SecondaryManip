#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
using namespace std;

vector<int> decimalReadNumbersFromFile(string fileName)
{
    uint32_t value;
    int lineRead = 0;
    vector<int> data; //Setup
    ifstream file(fileName);
    if (file.fail())
    {
        cout << "File inaccessible";
        exit(EXIT_FAILURE);
    }
    while (!(file.fail()))
    {   /*Keeping the pattern in raw ints of rolls is a bigger filesize than
          encoding everything as individual chars. However this makes it easy to import
          patterns with odd or new values */
        file >> lineRead; 
        data.push_back(lineRead);
    }
    file.close();
    return data;
}
vector<string> readStringFromFile(string fileName)
{
    vector<string> data; //Setup
    string lineRead = "";
    ifstream file(fileName);
    if (file.fail())
    {
        cout << "File inaccessible";
        exit(EXIT_FAILURE);
    }
    while (!(file.fail()))
    {
        getline(file,lineRead);
        data.push_back(lineRead);
    }
    file.close();
    cout << "lines from " + fileName + " read: " << data.size() << endl;
    return data;
}


void subtractOneFromItem(){
    vector<int> camFrames = decimalReadNumbersFromFile("outCompleteCombine.txt");
    ofstream result("outReducedCamFrames.txt");
    for (unsigned int i = 0; i < camFrames.size(); i++)
    {
        camFrames.at(i)--;
        result << camFrames.at(i) << endl;
    }
    
    cout << "FINISHED!";
}
void labelUniquePairs(){
    ofstream result("outBd3LTV.txt");
    vector<string> allLines = readStringFromFile("bd3lasttwovaluesonly.txt");
    
    //value definitions:
    //decode block
    for (unsigned int i = 0; i < allLines.size(); i++)
    { //could go and covert these into ints but not worth it imo.
    int label = -1;
       if (allLines.at(i) == "7,5"){
           label = 0;
       } else if (allLines.at(i) == "7,6"){
           label = 1;
       } else if (allLines.at(i) == "8,5"){
           label = 2;
       } else if (allLines.at(i) == "8,6"){
           label = 3;
       }
        result << label << endl;
    }
}

int main(){
    // subtractOneFromItem();
    labelUniquePairs();

    return 0;
}