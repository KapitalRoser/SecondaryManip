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

    /* ~~~~~~~~~~~~~~~~~~~~
    This doc was used to remove doubles from the PidIdentify tests,
    and replace them with stars. This was done several times
    as different cam angles were tested.
    ~~~~~~~~~~~~~~~~~~~~~~~~*/
    vector<string>doc1 = readStringFromFile("outCompare4.txt");
    vector<string>doc2 = readStringFromFile("outlagtest6.txt");
    ofstream outputDoc("outCompare5.txt");

    int count = 0;

    for (unsigned int i = 0; i < doc1.size();i++){
        count++;
        if (doc1.at(i) != doc2.at(i) && doc2.at(i).length() < 5){
                outputDoc << doc2.at(i) << " * " << endl;
        } else {
            outputDoc << doc1.at(i) << endl;
        }
    }
    



    return 0;
}