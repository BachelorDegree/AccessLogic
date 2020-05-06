#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include "AccessLogicServiceClient/AccessLogicServiceClient.hpp"
#include "ChunkMasterServiceClient/ChunkMasterServiceClient.hpp"
#include "ChunkServerServiceClient/ChunkServerServiceClient.hpp"
#include "Comm/fileutils.hpp"
#include <coredeps/SatelliteClient.hpp>
#include <unistd.h>

#include <fmt/format.h>
#include "coredeps/SliceId.hpp"

using namespace std;
using namespace accesslogic;
using namespace chunkmaster;
using namespace chunkserver;

string CanonicalChunkServerName(uint64_t iMachineId)
{
    Storage::SliceId oMachineId(iMachineId);
    return fmt::format(FMT_STRING("ChunkServerService.{}.{}"), oMachineId.Cluster(), oMachineId.Machine());
}

int main()
{
    /*
    SatelliteClient::GetInstance().SetServer("10.0.0.102:5553");
  	AccessLogicServiceClient alClient;
	accesslogic::HelloReq alReq;
	accesslogic::HelloResp alRsp;
	alReq.set_key("test");
	int iRet = alClient.Hello(alReq, alRsp);
	if(iRet != 0){
		std::cout<<iRet<<std::endl;
	}
	else
	{
		std::cout<<alRsp.value()<<std::endl;
	}
    */

    string filename = "../Comm/fileutils.hpp";
    string md5str;
    //fileSize(std::string &filename, size_t &filesize);
    getFileMD5(filename, md5str);
    cout <<md5str<<endl;

    return 0;

    SatelliteClient::GetInstance().SetServer("10.0.0.102:5553");
    
    string filepath = "../Comm/fileutils.hpp";
    //if (!isExist(filepath) || !isFile(filepath))
    string absfilepath;
    getAbsFilePath(filepath, absfilepath);
    filepath = absfilepath;
    if (!isFile(filepath))
    {
        printf("file not exit\n");
        return -1;
    }

    size_t filesize = 0;
    int ret = fileSize(filepath, filesize);
    if (filesize == 0)
    {
        printf("empty file\n");
        return -1;
    }

    //获取分片大小
    ChunkMasterServiceClient cmClient;
    CalculateUploadSliceLengthsReq cmReq;
    CalculateUploadSliceLengthsRsp cmRsp;
    cmReq.set_data_length(filesize);
    ret = cmClient.CalculateUploadSliceLengths(cmReq, cmRsp);
    if (ret != 0)
    {
        printf("CalculateUploadSliceLengths Err %d ret\n", ret);
        return -1;
    }

    /*
    FinishUploadSliceReq fusReq;
    FinishUploadSliceRsp fusRsp;
    fusReq.set_physical_slice_id(1048578);
    //ret = cmClient.FinishUploadSlice(fusReq, fusRsp);
    if (ret != 0)
    {
            printf("FinishUploadSlice Err %d ret\n", ret);
            return -1;
    }
    */

    //切分文件
    vector<size_t> vSegmentSize;
    vector<string> vSegmentPath;
    for (int i = 0; i < cmRsp.lengths_size(); ++ i)
    {
        vSegmentSize.push_back(cmRsp.lengths(i));
    }
    ret = fileSegment(filepath, vSegmentSize, vSegmentPath);
    if (ret != 0)
    {
        printf("split file Err %d ret\n", ret);
        return -1;
    }

    //获取逻辑分片和物理分片id
    AllocateUploadSliceReq ausReq;
    AllocateUploadSliceRsp ausRsp;
    vector<size_t> vPhySliId;
    for (size_t i = 0; i < vSegmentSize.size(); ++i)
    {
        ausReq.set_data_length(vSegmentSize[i]);
        ausReq.set_replica_policy(1);
        ret = cmClient.AllocateUploadSlice(ausReq, ausRsp);
        if (ret != 0)
        {
            printf("AllocateUploadSlice Err %d ret\n", ret);
            return -1;
        }

        //size_t logicSliId = ausRsp.logical_slice_id();
        for (int j = 0; j < ausRsp.physical_slice_id_size(); ++j)
        {
            if (j == 0) vPhySliId.push_back(ausRsp.physical_slice_id(j));
            ChunkServerServiceClient cssClient(CanonicalChunkServerName((uint64_t)ausRsp.physical_slice_id(j)));
            WriteSliceReq wsReq;
            WriteSliceRsp wsRsp;
            wsReq.set_slice_id(ausRsp.physical_slice_id(j));
            wsReq.set_offset(0);

            char *data = new char[1024 * 1024 * 16];
            ifstream fin(vSegmentPath[i]);
            fin.read(data, vSegmentSize[i]);
            fin.close();
            wsReq.set_data(data);
            delete []data;
            data = nullptr;
            cout << "data size " << wsReq.data().size() << endl;
            
            ret = cssClient.WriteSlice(wsReq, wsRsp);
            if (ret != 0)
            {
                printf("WriteSlice slice_id %lu Err %d ret\n", ausRsp.physical_slice_id(j), ret);
                return -1;
            }

            printf("write %lu success\n", ausRsp.physical_slice_id(j));

            FinishUploadSliceReq fusReq;
            FinishUploadSliceRsp fusRsp;
            fusReq.set_physical_slice_id(ausRsp.physical_slice_id(j));
            ret = cmClient.FinishUploadSlice(fusReq, fusRsp);
            if (ret != 0)
            {
                printf("FinishUploadSlice Err %d ret", ret);
                return -1;
            }

            printf("finish %lu success\n", ausRsp.physical_slice_id(j));
        }
    }

    for (size_t i = 0; i < vSegmentSize.size(); ++i)
    {
        ChunkServerServiceClient cssClient(CanonicalChunkServerName(vPhySliId[i]));
        ReadSliceReq rlReq;
        ReadSliceRsp rlRsp;
        rlReq.set_slice_id(vPhySliId[i]);
        rlReq.set_offset(0);
        rlReq.set_length_to_read(vSegmentSize[i]);
        ret = cssClient.ReadSlice(rlReq, rlRsp);
        if (ret != 0 || rlRsp.data_length() != vSegmentSize[i])
        {
            printf("ReadSlice Err %d ret, vSegmentSize=%lu,rsplen=%u\n", ret, vSegmentSize[i],rlRsp.data_length());
            return -1;
        }

        string data = rlRsp.data();
        ofstream fout(vSegmentPath[i].c_str());
        fout.write(data.c_str(), vSegmentSize[i]);
        fout.close();

        printf("read %lu success\n", vPhySliId[i]);
    }

    string newfile = "masternew.zip";
    ret = fileMerge(newfile, vSegmentSize, vSegmentPath);
    if (ret != 0)
    {
        printf("fileSegment Err %d ret\n", ret);
        return -1;
    }

    printf("merge success\n");

    /*
    string filename;
    string absfilepath;
    string absdirpath;
    int ret = getFileName(filepath, filename);
    ret = getAbsFilePath(filepath, absfilepath);
    ret = getAbsDirPath(filepath, absdirpath);
    cout << filename << endl << absfilepath << endl << absdirpath << endl;
    cout << (int)isFile(filepath) << endl << (int)isFile(absfilepath) << endl << (int)isDir(absdirpath) << endl;
	*/

/*
    string absdirpath;
    getAbsDirPath(filepath, absdirpath);
    vector<size_t> vSegmentSize;
    vSegmentSize.push_back(5783724);
    vSegmentSize.push_back(5783724);
    vector<string> vSegmentPath;
    //vSegmentPath.push_back(absdirpath + "/tmp_0");
    //vSegmentPath.push_back(absdirpath + "/tmp_1");
    fileSegment(filepath, vSegmentSize, vSegmentPath);
    //filepath = "./mastercopy.zip";
    //fileMerge(filepath, vSegmentSize, vSegmentPath);
*/
    return 0;
}
