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
