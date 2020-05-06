#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

namespace accesslogic{

//文件切分处理
#define BLOCKSIZE 1048576
#define BUFFERSIZE 10240

bool isExist(const std::string &pathname);
bool isFile(const std::string &filename);
bool isDir(const std::string &dirname);
int checkAndCreateDir(const std::string &dirpath);
int getFileName(const std::string &oriPath, std::string &filename);
int getAbsFilePath(const std::string &oriPath, std::string &absFilePath);
int getAbsDirPath(const std::string &oriPath, std::string &absDirPath);
int fileSize(const std::string &filename, size_t &filesize);
int readFile(char* data, std::ifstream &input, int size);
int writeFile(char* data, std::ofstream &fout, int size);
int copyFile(std::ifstream &fin, std::ofstream &fout, size_t inputsize);
int fileSegment(const std::string &filepath, 
        std::vector<size_t> &vSegmentSize, std::vector<std::string> &vSegmentPath);
int fileMerge(const std::string &filepath, 
        std::vector<size_t> &vSegmentSize, std::vector<std::string> &vSegmentPath);

//文件MD5计算
int getFileMD5(const std::string &filename, std::string &md5Str);
} //end namespace
