#include <string>
#include <fstream>

class FileSegment {
public:
        void segment(const std::string file_name, const int segment_num, const std::string json_file="config.json");
        void merge(const std::string json_file="config.json");
private:
        bool fileExist(const string &name);
        bool dirExist(const string &name);
        size_t file_size(ifstream &file);
        void copy_file(ifstream &input, ofstream &output, size_t input_size);

        inline void read_file_in_block(char* data, ifstream &input, int size=kBlockSize) 
	    {
                input.read(data, size);
        }
        inline void write_file_in_block(char* data, ofstream &output, int size=kBlockSize) 
	    {
                output.write(data, size);
        }
private:
        // json key
        static const string kSegmentFileNum;
        static const string kSourceFileName;
        static const string kSegmentFiles;

        static const int kBlockSize;
};

//MD5计算
/* typedef a 32 bit type */
typedef unsigned long int UINT4;

/* Data structure for MD5 (Message Digest) computation */
typedef struct {
	UINT4 i[2];                   /* number of _bits_ handled mod 2^64 */
	UINT4 buf[4];                                    /* scratch buffer */
	unsigned char in[64];                              /* input buffer */
	unsigned char digest[16];     /* actual digest after MD5Final call */
} MD5_CTX;

void MD5Init(MD5_CTX *mdContext);
void MD5Update(MD5_CTX *mdContext, unsigned char *inBuf, unsigned int inLen);
void MD5Final(MD5_CTX *mdContext);
static void Transform(UINT4 *buf, UINT4 *in);
char *MD5_file(char *path, int md5_len);
