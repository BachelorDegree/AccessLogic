#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <libgen.h>
//#include "atlstr.h"
#include <sys/io.h>
#include <sys/stat.h>
#include <dirent.h>
#include <memory>
#include <sstream>
#include <iomanip>

#include "fileutils.hpp"
#include "md5global.hpp"
#include "md5.hpp"

using namespace std;

namespace accesslogic{

//F_OK (或0): 判断该文件/文件夹是否存在；
//R_OK (或2): 判断该文件/文件夹是否有读权限；
//W_OK (或4): 判断该文件/文件夹是否有写入权限；
//X_OK (或6): 判断该文件/文件夹是否有执行权限；
//access(filename.c_str(), F_OK) == 0;

bool isExit(const string &pathname)
{
    struct stat buffer;
	return (stat(pathname.c_str(), &buffer) != 0);
}

bool isFile(const string &filename)
{
    struct stat buffer;
    int ret = stat(filename.c_str(), &buffer);
    if (ret != 0)
    {
        printf("%s not exit.\n", filename.c_str());
        return false;
    }

    return (S_IFREG & buffer.st_mode);
}

bool isDir(const string &dirname)
{
    struct stat buffer;
    int ret = stat(dirname.c_str(), &buffer);
    if (ret != 0)
    {
        printf("%s not exit.\n", dirname.c_str());
        return false;
    }

    return (S_IFDIR & buffer.st_mode);
}

int checkAndCreateDir(const std::string &dirpath)
{
	int len = dirpath.length();
	char tmpDirPath[256] = { 0 };
	for (int i = 0; i < len; i++)
	{
		tmpDirPath[i] = dirpath[i];
		if (tmpDirPath[i] == '\\' || tmpDirPath[i] == '/')
		{
			if (!isExit(tmpDirPath) || !isDir(tmpDirPath))
			{
				int ret = mkdir(tmpDirPath, S_IRWXG);
                cout << ret << endl;
				if (ret == -1) return ret;
			}
		}
	}

	return 0;
}

int getFileName(const string &oriPath, string &filename)
{
    char path[128];
    snprintf(path, 128, "%s", oriPath.c_str());
    filename = basename(path);
    return 0;
}

int getAbsFilePath(const string &oriPath, string &absFilePath)
{
    char buffer[1024];

    if (!isFile(oriPath)) return -1;

    if(realpath(oriPath.c_str(), buffer) == NULL)
    {
        printf("the file '%s' is not exist\n", oriPath.c_str());
    }

    absFilePath = buffer;
    return 0;
}

int getAbsDirPath(const string &oriPath, string &absDirPath)
{
    char buffer[1024];

    if (!isFile(oriPath)) return -1;

    if(realpath(oriPath.c_str(), buffer) == NULL)
    {
        printf("the file '%s' is not exist\n", oriPath.c_str());
    }

    absDirPath = dirname(buffer);
    return 0;
}

int fileSize(const string &filename, size_t &filesize)
{
    struct stat buffer;
    if (stat(filename.c_str(), &buffer) != 0)
        return -1;
    filesize = buffer.st_size;
    return 0;
}

int readFile(char* data, std::ifstream &fin, int size)
{
    fin.read(data, size); 
    return 0;
}

int writeFile(char* data, std::ofstream &fout, int size)
{
    fout.write(data, size);
    return 0;
}

int copyFile(ifstream &fin, ofstream &fout, size_t inputsize)
{
    char* data = new char[BLOCKSIZE];
    size_t blockcount = inputsize / BLOCKSIZE;
    for (size_t i = 0; i < blockcount; ++i)
    {
        readFile(data, fin, BLOCKSIZE);
        writeFile(data, fout, BLOCKSIZE);
    }

    size_t leftsize = inputsize % BLOCKSIZE;
    if (leftsize != 0)
    {
        readFile(data, fin, leftsize);
        writeFile(data, fout, leftsize);
    }

    delete [] data;
    data = nullptr;

    return 0;
}

int fileSegment(const string &filepath,
        vector<size_t> &vSegmentSize,vector<string> &vSegmentPath)
{
    string filename;
    string dirpath;
    int ret = getFileName(filepath, filename);
    ret = getAbsDirPath(filepath, dirpath);
    string tmpdirpath = dirpath + "/";

    ifstream fin(filepath);
    for (size_t i = 0; i < vSegmentSize.size(); ++i)
    {
        size_t segmentsize = vSegmentSize[i];
        string segmentpath = tmpdirpath + filename + "_" + to_string(i) + ".tmp";
        vSegmentPath.push_back(segmentpath);

        ofstream fout(segmentpath);
        copyFile(fin, fout, segmentsize);
        fout.close();
    }
    fin.close();

    return 0;
}

int fileMerge(const string &filepath, 
        vector<size_t> &vSegmentSize, vector<string> &vSegmentPath)
{
    ofstream fout(filepath);
    for (size_t i = 0; i < vSegmentSize.size(); ++i)
    {
        size_t segmentsize = vSegmentSize[i];
        ifstream fin(vSegmentPath[i]);
        copyFile(fin, fout, segmentsize);
        fin.close();
        int ret = remove(vSegmentPath[i].c_str());
    }
    fout.close();

    return 0;
}

int getFileMD5(const string &filename, string &md5Str)
{
    ifstream fin(filename.c_str(), ifstream::in |ifstream::binary);
    if (!fin) return -1;
    MD5_CTX context;
    MD5Init(&context);

    size_t filesize;
    int ret = fileSize(filename, filesize);
    
    size_t totalCount = 0;
    unique_ptr<unsigned char[]> buffer{new unsigned char[BUFFERSIZE] {}};
    decltype(fin.gcount()) readCount = 0; //范型编程，自动类型推导

    while (fin.read(reinterpret_cast<char*>(buffer.get()), BUFFERSIZE))
    {
        readCount = fin.gcount();
        totalCount += readCount;
        MD5Update(&context, buffer.get(), static_cast<unsigned int>(readCount));
    }

    readCount = fin.gcount();
    if (readCount > 0)
    {
        totalCount += readCount;
        MD5Update(&context, buffer.get(), static_cast<unsigned int>(readCount));
    }
    fin.close();

    if(totalCount != filesize) return -1;

    unsigned char digest[16];
    MD5Final(digest, &context);

    ostringstream oss;
    for (int i = 0; i < 16; ++i)
    {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(digest[i]);
    }
    oss << std::ends;
    md5Str = oss.str();

    return 0;
}

} //end namespace
