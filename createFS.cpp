#include<iostream>
#include<cstring>
#include "file.h"

/* 
    void create(int num_datab, int num_dirb, int size_blocks);
    void write(std::string filename);
    void createdir(char type,int parentIndex, char* dirname);
    void read(std::string filename);
 * */


using namespace std;

int main(){
    FileSystem fs;
    fs.FileSystem::DCREATE(100,20, 512);
    string filename = "file.dat";
    fs.FileSystem::DWRITE(filename);
}
