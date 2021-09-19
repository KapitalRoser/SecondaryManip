#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;
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

int main(){
    vector<string>doc1 = readStringFromFile("completeCombine.txt");

    ofstream outputDoc("outCompleteCombine.txt");

    int count = 0;

    for (unsigned int i = 0; i < doc1.size();i++){
        count++;
        if (doc1.at(i)[2] == '*'){
            outputDoc << doc1.at(i)[0];
        } else {
            outputDoc << doc1.at(i);
        }
        outputDoc << endl;

    }
    



    return 0;
}