#include<iostream>
#include<stdlib.h>
#include<fstream>
#include<cstring>
#include "file.h"


using namespace std; 

//creates a new virtual disk
void FileSystem::DCREATE(int num_datab, int num_dirb, int size_blocks){
    this->sb.num_datab = num_datab;
    this->sb.num_dirb = num_dirb;
    this->sb.size_blocks = size_blocks;
    this->dib = (dir_block*)malloc(sizeof(dir_block) * this->sb.num_dirb);
    this->dab = (data_block*)malloc(sizeof(data_block) * this->sb.num_datab);
    for(int i=0;i<this->sb.num_dirb;i++){
        this->dib[i].BACK = 0;
        this->dib[i].FRWD = 0;
        this->dib[i].FREE = 0;
        this->dib[i].dir = 0;
        this->dib[i].type = 'F';
        this->dib[i].link = 0;
    }
    for(int i=0;i<this->sb.num_datab;i++){
        this->dab[i].BACK = 0;
        this->dab[i].FRWD = 0;
    }
    // root folder
    this->dib[0].type = 'D';
    strcpy(this->dib[0].name,"");
    strcpy(this->head->dirName, "");
    this->dib[0].link = 0;
    this->head->index = 0;
    this->head->next = NULL;
    this->tail = this->head;
}

//writes virtual disk to a file
void FileSystem::DWRITE(string filename){
    ofstream myfile;
    myfile.open(filename);
    myfile.write((char *) &this->sb, sizeof(super_block));

    for(int i =0; i<this->sb.num_dirb;i++){
        myfile.write((char *) &this->dib[i], sizeof(dir_block));
    }
    for(int i =0; i< this->sb.num_datab;i++){
        myfile.write((char *) &this->dab[i], sizeof(data_block));
    }
    myfile.close();
}

//reads virtual disk from file
void FileSystem::DREAD(string filename){
    ifstream myfile;
    myfile.open(filename);
    myfile.read((char *)&this->sb, sizeof(super_block));
    this->dib = (dir_block*)malloc(sizeof(dir_block) * this->sb.num_dirb);
    this->dab = (data_block*)malloc(sizeof(data_block) * this->sb.num_datab);
    for(int i =0; i<this->sb.num_dirb;i++){
        myfile.read((char *)&this->dib[i], sizeof(dir_block));
    }
    for(int i =0; i< this->sb.num_datab;i++){
        myfile.read((char *) &this->dab[i], sizeof(data_block));
    }
    myfile.close();
    strcpy(this->head->dirName, "");
    this->head->index = 0;
    this->head->next = NULL;
    this->tail = this->head;

}

//creates  directories and files
void FileSystem::createDir(char type, char* dirname){
    if (strlen(dirname) > 9) {
        cout << "file name size cannot be more than 9" << endl;
    }
    else {
        int i = this->findFreeDirBlock();
        this->dib[i].type = type;
        strcpy(this->dib[i].name, dirname);
        this->dib[this->getCurrentFileIndex()].dir++;
        this->dib[i].link = 0;
        this->dib[i].size = 0;
        this->dib[i].dir = 0;
        this->appendAsSubDir(i, this->getCurrentFileIndex());
        if (type == 'U') {
            this->openDataFile('O', dirname);
        }
    }
}

//Done
void FileSystem::deleteFile(char* dirname){
    dir_block* dirblck = &this->dib[this->dib[this->getCurrentFileIndex()].link];
    if (this->dib[this->getCurrentFileIndex()].link != 0) {
        while (strcmp(dirblck->name, dirname)!=0) {
            if (dirblck->FRWD == 0) break;

            dirblck = &this->dib[dirblck->FRWD];
        }
        
        if (strcmp(dirblck->name, dirname) == 0) {
            if (dirblck->type == 'D' && dirblck->link != 0) {
                cout << "this folder is not empty" << endl;
            }
            else if (dirblck->type == 'U' && dirblck->link != 0) {
                data_block* datablck = &this->dab[dirblck->link];
                while (datablck->FRWD != 0) {
                    int t = datablck->FRWD;
                    datablck->data[0] == '\0';
                    datablck->FRWD = 0;
                    datablck->BACK = 0;
                    datablck = &this->dab[t];
                }
                dirblck->link = 0;
            }

            if (dirblck->BACK != 0 && dirblck->FRWD != 0) {
                this->dib[dirblck->BACK].FRWD = dirblck->FRWD;
                this->dib[dirblck->FRWD].BACK = dirblck->BACK;
            }
            else if (dirblck->BACK == 0 && dirblck->FRWD!=0) {
                this->dib[this->getCurrentFileIndex()].link = dirblck->FRWD;
                this->dib[dirblck->FRWD].BACK = 0;
            }
            else if (dirblck->FRWD == 0 && dirblck->BACK!=0) {
                this->dib[dirblck->BACK].FRWD = 0;
            }
            
            dirblck->name[0] = '\0';
            dirblck->size = 0;
            dirblck->type = 'F';
            cout << "successfully deleted" << endl;
        }
        else {
            cout << "file not found" << endl;
        }
        
    }


}

//done
void FileSystem::addNewDataBlock(data_block* newdab) {

    int tempIndex = this->findFreeDataBlock();
    newdab->FRWD = tempIndex;
    this->dab[newdab->FRWD].BACK = tempIndex;
}

//not done
void FileSystem::writeData(int n, char* data){
    if (this->dib[openedFileIndex].link == 0) {
        this->dib[openedFileIndex].link = this->findFreeDataBlock();
    }
    if (n <= strlen(data)) {

        this->seekData(-1, 0);
        int temp = n;
        while (temp > 504) {
            temp -= 504;
        }
        this->dib[this->openedFileIndex].size = (short)temp;
        if (this->dib[openedFileIndex].link == 0) {
            this->dib[openedFileIndex].link == this->findFreeDataBlock();
        }


        for (int i = 0; i < n; i++) {
            *this->pointer = data[i];
            this->setPointerToPosition(1);
        }
    }
    this->seekData(-1, 0);
}

//Done
void FileSystem::setPointerToPosition(int n) {
    if ((this->position + n) >= 0) {
        this->position += n;
    }
    else {
        cout << "out of range" << endl;
    }
    data_block* datablck;
    int temp = this->position;
    if (this->isFileOpened) {
        datablck = &this->dab[this->dib[this->openedFileIndex].link];
        while (temp >= 504 || temp<=-504) {
            if (temp >= 504) {
                if (datablck->FRWD == 0)
                    this->addNewDataBlock(datablck);
                datablck = &this->dab[datablck->FRWD];
                temp -= 504;
            }
            else if (temp <= -504) {
                datablck = &this->dab[datablck->BACK];
                temp += 504;
            }
        }
        this->pointer = &datablck->data[temp];
    }
}

//not done
void FileSystem::readData(int n){
    int tempPos = this->position;
    for(int i =0; i<n;i++){
        if (*this->pointer == '\0') {
            cout << "<-EOF->";
            break;
        }
        cout << *(this->pointer);
        setPointerToPosition(+1);
    }
    cout << endl;
    this->position = tempPos;
    setPointerToPosition(+0);
}

//Done
void FileSystem::seekData(int base, int offset){
    data_block* datablck = &this->dab[this->dib[openedFileIndex].link];
    if (base == -1) {
        this->position = 0;
    }
    else if (base == 1) {
        this->position = this->getSizeofUserData(&this->dib[this->openedFileIndex]);
    }
    else if (base == 0);
    else
        cout << "invalid command" << endl;
    setPointerToPosition(offset);
}

//DONE
int FileSystem::getCurrentFileIndex(){
    return this->tail->index;
}

//DONE
char* FileSystem::getCurrentFileName(){
    return this->tail->dirName;
}

//DONE
void FileSystem::printChildDirs(){
    int parentIndex = this->getCurrentFileIndex();
    int childDirIndex = this->dib[parentIndex].link;
    if(childDirIndex != 0){
        if (this->dib[childDirIndex].type == 'U') {
                cout << "*";
            }
        cout << this->dib[childDirIndex].name << "\t";
        while(this->dib[childDirIndex].FRWD != 0){
            childDirIndex = this->dib[childDirIndex].FRWD;
            if (this->dib[childDirIndex].type == 'U') {
                cout << "*";
            }
            cout << this->dib[childDirIndex].name << "\t";
        }
    }
    cout << endl;
}

//DONE
void FileSystem::openFolder(char* name) {
    if (strcmp(name, "..") == 0) {
        this->tail = this->head;
        if (tail->next != NULL) {
            while (this->tail->next->next != NULL) {
                this->tail = this->tail->next;

            }
            this->tail->next = NULL;
        }
    }
    else {
        int parentIndex = this->getCurrentFileIndex();
        int childDirIndex = this->dib[parentIndex].link;
        if (childDirIndex != -1) {
            while (this->dib[childDirIndex].FRWD != 0) {
                if (strcmp(this->dib[childDirIndex].name, name) == 0) {
                    break;
                }
                childDirIndex = this->dib[childDirIndex].FRWD;
            }
        }
        if (strcmp(this->dib[childDirIndex].name, name) == 0) {
            if (this->dib[childDirIndex].type == 'U') {
                cout << "it is a user date file" << endl;
            }
            else {
                this->tail->next = (dirNode*)malloc(sizeof(dirNode));
                this->tail = this->tail->next;
                this->tail->next = NULL;
                strcpy(this->tail->dirName, dib[childDirIndex].name);
                this->tail->index = childDirIndex;
            }
        }
        else {
            cout << "file does not exist" << endl;
        }
    }
}

//DONE
void FileSystem::openDataFile(char openMode, char* name) {
    int childDirIndex = this->dib[this->getCurrentFileIndex()].link;
    if (childDirIndex != 0) {
        while (this->dib[childDirIndex].FRWD != 0) {
            if (strcmp(this->dib[childDirIndex].name, name) == 0) {
                break;
            }
            childDirIndex = this->dib[childDirIndex].FRWD;
        }
    }
    if (strcmp(this->dib[childDirIndex].name, name) == 0) {
        if (this->dib[childDirIndex].type == 'D') {
            cout << "it is a directory file" << endl;
        }
        else {
            this->isFileOpened = true;
            strcpy(this->openedFileName, name);
            this->openMode = openMode;
            this->openedFileIndex = childDirIndex;
            this->position = 0;
            this->pointer = &this->dab[this->dib[childDirIndex].link].data[0];
            this->openedFileSize = this->getSizeofUserData(&this->dib[childDirIndex]);
        }
    }
    else {
        cout << "file does not exist" << endl;
    }
}

//DONE
void FileSystem::closeDataFile() {
    this->isFileOpened = false;
    strcpy(this->openedFileName, "\n");
}

//DONE
string FileSystem::getPath() {
    string path = "";
    this->tail = this->head;
    string str(this->tail->dirName);
    path.append(str);
    path.append("/");
    while (this->tail->next != NULL) {
        this->tail = this->tail->next;
        string str(this->tail->dirName);
        path.append(str);
        path.append("/");
    }
    return path;
}

//Done
void FileSystem::printFileHierarchy(int index, string a) {
    dir_block* root = &this->dib[index];
    if (index == 0) {
        cout << "root" << endl;
    }
    else if(root->type == 'D')
        cout << a << "|----" << root->name << endl;
    else if (root->type == 'U') {
        cout << a << "|----" << root->name << " (Size of Data File: " << this->getSizeofUserData(&this->dib[index]) << ")" << endl;
    }
    if(root->link != 0 && root->type == 'D') {
        int child = root->link;
        a.append("    ");
        printFileHierarchy(child,a);
        while(this->dib[child].FRWD != 0){
            child = this->dib[child].FRWD;
            printFileHierarchy(child,a);
        }
    }
}

//Done
int FileSystem::getSizeofUserData(dir_block* myfile) {
    int result = 0;
    if (myfile->link != 0) {
        int tempIndex = myfile->link;
        result += (int) myfile->size;
        while (this->dab[tempIndex].FRWD !=0) {
            tempIndex = this->dab[tempIndex].FRWD;
            result += 504;
        }
    }
    return result;
}
int FileSystem::findFreeDirBlock() {
    int i = 0;
    while (this->dib[i].type != 'F') {
        i++;
        if (i > this->sb.num_dirb) {
            printf("DirBloks are full\n");
            return -1;
        }
    }
    return i;
}

//Done
int FileSystem::findFreeDataBlock() {
    int i = 1;
    while (this->dab[i].data[0] != '\0') {
        i++;
        if (i > this->sb.num_datab) {
            printf("DataBlocks are full\n");

            return -1;
        }
    }
    return i;
}

//DONE
void FileSystem::appendAsSubDir(int dirIndex, int parentIndex) {
    int tempdirbIndex;
    dir_block* tempdirb;
    if (this->dib[parentIndex].link != 0) {
        tempdirbIndex = this->dib[parentIndex].link;
        tempdirb = &this->dib[tempdirbIndex];
        while (tempdirb->FRWD != 0) {
            tempdirbIndex = this->dib[tempdirbIndex].FRWD;
            tempdirb = &this->dib[tempdirbIndex];
        }
        tempdirb->FRWD = dirIndex;
        this->dib[dirIndex].BACK = tempdirbIndex;
    }
    else {
        this->dib[parentIndex].link = dirIndex;
    }
}