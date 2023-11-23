#pragma once
// 一些工具类

#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <cerrno>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

namespace util
{
	class UtilFile
	{
	public:
		File(const std::string filename = "")
			:filename_(filename)
		{}

		bool Exists() const
		{
			// access()函数判断文件是否存在的函数
			if(access(filename_.c_str(), F_OK) == -1)
				return false;
			return true;
		}

		size_t Size() const
		{
			if(!Exists()) return 0;

			// 文件存在
			struct stat st;
			// stat()函数获取文件大小
			if(stat(filename_.c_str(), &st) == -1)
			{
				std::cout << "Size(): stat error: " << strerror(errno) << std::endl;
				return 0;
			}
			return st.st_size;
		}

		bool ReadFile(std::string* inbuffer) const
		{
			// 读取文件内容---ifstream
			std::ifstream ifs;
			ifs.open(filename_, std::ios::binary);  // 二进制方式
			if(!ifs.is_open())
			{
				std::cout << "ReadFile: open file error!" << std::endl;
				return false;
			}
			size_t len = Size();
			inbuffer->resize(len);
			// 获取inbuffer第一个位置的指针
			ifs.read(&((*inbuffer)[0]), len);
			if(!ifs.good())
			{
				std::cout << "ReadFile: read file error!" << std::endl;
				// 关闭文件描述符
				ifs.close();
				return false;
			}
			ifs.close();
			return true;
		}

		bool WriteFile(const std::string& outbuffer)
		{
			std::ofstream ofs;
			ofs.open(filename_, std::ios::binary);  // 二进制方式
			if(!ofs.is_open())
			{
				std::cout << "WriteFile: open file error!" << std::endl;
				return false;
			}
			ofs.write(outbuffer.c_str(), outbuffer.size());
			if(!ofs.good())
			{
				std::cout << "WriteFile: write file error!" << std::endl;
				// 关闭文件描述符
				ofs.close();
				return false;
			}
			ofs.close();
			return true;
		}

		bool CreateDir() const
		{
			if(Exists()) return true;
			if(mkdir(filename_.c_str(), 0744) == -1)
			{
				std::cout << "CreateDir: mkdir error: " << strerror(errno) << std::endl;
				return false;
			}
			return true;
		}

	private:
		std::string filename_;
	};


	class UtilJson
	{
	public:

	};
}
