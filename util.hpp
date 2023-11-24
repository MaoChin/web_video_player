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

// 第三方库
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/value.h>
#include <jsoncpp/json/writer.h>
#include <jsoncpp/json/reader.h>

namespace web_video
{
	// 文件相关的方法类
	class UtilFile
	{
	public:
		UtilFile(const std::string filename = "")
			:filename_(filename)
		{}

		// 判断当前路径下该文件是否存在
		bool Exists() const
		{
			// access()函数判断文件是否存在的函数
			if(access(filename_.c_str(), F_OK) == -1)
				return false;
			return true;
		}

		// 获取文件大小
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

		// 从文件中读取数据
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

		// 向文件中写入数据
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

		// 在当前路径下创建 filename_ 文件夹
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

	// 进行序列化和反序列化的方法类
	class UtilJson
	{
	public:
		// 序列化：value对象 -> json串
		static bool Serialize(const Json::Value& value, std::string* str)
		{
			Json::StreamWriterBuilder jsbuild;
			std::unique_ptr<Json::StreamWriter> sw(jsbuild.newStreamWriter());
			std::stringstream ss;
			int ret = sw->write(value, &ss);
			if(ret != 0)
			{
				// 序列化失败
				std::cerr << "Serialize error!" << std::endl;
				return false;
			}
			*str = ss.str();
			return true;
		}
		// 反序列化：json串-> value对象
		static bool UnSerialize(const std::string& str, Json::Value* value)
		{
			Json::CharReaderBuilder crb;
			std::unique_ptr<Json::CharReader> cr(crb.newCharReader());
			std::string err;
			bool ret = cr->parse(str.c_str(), str.c_str() + str.size(), value, &err);
			if(!ret)
			{
				std::cerr << "UnSerialize error!" << std::endl;
				return false;
			}
			return true;
		}
	};
}
