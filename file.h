#include<cstring>
#include<iostream>

#ifndef FILE_H
#define FILE_H

class FileSystem{
private:
    struct data_block{
        int BACK;
        char data[504];
        int FRWD;
    };

    struct dirNode{
        int index;
        char dirName[9];
        dirNode* next;
    };

    struct dir_block{
        int BACK;
        int FRWD;
        int FREE;
        char filler[4];
        int dir;
        char type;
        char name[9];
        int link;
        short size;
    };

    struct super_block{
        int num_datab;
        int size_blocks;
        int num_dirb;
    };

    dir_block* dib;
    data_block* dab; 
    super_block sb;
        
    dirNode* head = (dirNode*)malloc(sizeof(dirNode));
    dirNode* tail;

    void appendAsSubDir(int dirIndex, int parentIndex);
    int findFreeDataBlock();
    int findFreeDirBlock();
    int getSizeofUserData(dir_block* uFile);
    void addNewDataBlock(data_block* dab);
    void setPointerToPosition(int n);


public:
    void DCREATE(int num_datab, int size_blocks, int num_dirb);
    void DWRITE(std::string filename);
    void DREAD(std::string filename);
    void createDir(char type, char* dirname);
    void writeData(int n, char* data);
    void readData(int n);
    void deleteFile(char* dirname);
    void seekData(int base, int offset);
    std::string getPath();
    void printChildDirs();
    void openFolder(char* name);
    void openDataFile(char openMode, char* name);
    void closeDataFile();
    void printFileHierarchy(int index, std::string a);
    int getCurrentFileIndex();
    char* getCurrentFileName();

    
    
    //User Data Variables
    bool isFileOpened = false;
    char openedFileName[9];
    int openedFileIndex;
    int openedFileSize;
    char openMode;
    int position;
    char* pointer;
    
};
#endif
