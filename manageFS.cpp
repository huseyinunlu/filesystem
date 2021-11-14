#include<iostream>
#include<cstring>
#include "file.h"
#include<vector>
#include <sstream>

using namespace std;

int main(int argc, char* argv[]){
    FileSystem fs;
    string filename;
    if(argc != 2){
        cout << "invalid args.\n";
        return 0;
    }    
    else{
        filename = argv[1];
        fs.DREAD(filename);
    }
    
    string cmd;
    vector<string> args;
    string token;
    cout << "/->";
    while (getline(cin, cmd)) {
        
        args.clear();
        istringstream iss(cmd);
        while (iss >> token) { args.push_back(token); }
        if (args.size() == 0) {
            continue;
        }
        else if(args[0] == "exit") exit(1);
        else if (fs.isFileOpened == false) {
            if (args[0] == "create") {
                string temp = args[1];
                fs.createDir(temp[0], (char*) args[2].c_str());
            }
            else if (args[0] == "ls") {
                fs.printChildDirs();
            }
            else if (args[0] == "tree") {
                fs.printFileHierarchy(0, "");
            }
            else if (args[0] == "cd") {
                fs.openFolder((char*)args[1].c_str());
            }
            else if (args[0] == "open") {
                string temp = args[1];
                fs.openDataFile(temp[0],(char*) args[2].c_str());

            }
            else if (args[0] == "delete") {
                fs.deleteFile((char*) args[1].c_str());
            }
            else {
                cout << "invalid command" << endl;
            }
        }
        else {
            if (args[0] == "close") {
                fs.closeDataFile();
            }
            else if (args[0] == "read" && (fs.openMode == 'I' || fs.openMode == 'U')) {
                fs.readData(stoi(args[1]));
            }
            else if (args[0] == "write"&& (fs.openMode == 'O' || fs.openMode == 'U')) {
                size_t pos1 = cmd.find("'");
                string arg2 = cmd.substr(pos1+1);
                size_t pos2 = arg2.find("'");
                arg2 = arg2.substr(0, pos2);
                fs.writeData(stoi(args[1]), (char*) arg2.c_str());

            }
            else if (args[0] == "seek" && (fs.openMode == 'I' || fs.openMode == 'U')) {
                fs.seekData(stoi(args[1]), stoi(args[2]) );
            }
            else {
                cout << "invalid command" << endl;
            }
            
        }
        cout << fs.getPath();
        if (fs.isFileOpened) {
            cout << "(" << fs.openedFileName << ")";
        }
        cout << "->";
        
        fs.DWRITE(filename);
    }
}
