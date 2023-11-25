#pragma once
// 业务处理模块&&网络通信模块，主要是接收客户端的网络请求，解析请求并进行相应的业务处理

#include "data.hpp"
#include "httplib.h"
#include "util.hpp"
#include <algorithm>
#include <jsoncpp/json/value.h>

namespace web_video
{
	// 静态资源根目录
#define WWW_ROOT "./www"
#define VIDEO_ROOT "/video/"
#define IMAGE_ROOT "/image/"

	// 全局的数据表(数据管理模块)
	VideoTable* videoT = nullptr;

	class Server
	{
	public:
		Server(int port = 8080)
			:port_(port)
		{}

		// 初始化VideoTable, 建立网络请求和对应处理函数的映射关系，并设置静态资源根目录
		bool Init()
		{
			// 1. 初始化数据管理模块---数据表
			videoT = new VideoTable();
			// 2. 创建相关目录
			std::string root = WWW_ROOT;
			std::string videoPath = root + VIDEO_ROOT;
			std::string imagePath = root + IMAGE_ROOT;
			// 2.1 文件相关工具
			UtilFile(WWW_ROOT).CreateDir();
			UtilFile(videoPath).CreateDir();
			UtilFile(imagePath).CreateDir();
			// 3. 设置静态资源根目录
			bool ret = server_.set_mount_point("/", WWW_ROOT);
			if(!ret)
			{
				std::cerr << "Server Init: set_mount_point error!" << std::endl;
				return false;
			}
			// 4.建立网络请求和对应处理函数之间的映射  请求URL的一部分-->处理函数
			// 4.1 插入
			server_.Post("/video" ,Insert);
			// 4.2 修改
			server_.Put("/video/(\\d+)" ,Update);
			// 4.3 删除
			server_.Delete("/video/(\\d+)" ,Delete);
			// 4.4 查询
			server_.Get("/video" ,SelectAll);
			server_.Get("/video/(\\d+)" ,SelectID);
			return true;
		}
		// 启动服务器
		bool Run()
		{
			server_.listen("0.0.0.0", port_);
			return true;
		}
	private:
		// 针对不同请求的业务处理接口
		// 设置成静态函数没有this指针，这样适配httplib库中的接口
		// 大体上都包含两部分操作，一是文件的新增删除，而是数据库中的新增删除
		static void Insert(const httplib::Request& req, httplib::Response& resp)
		{
			// 1. 获取视频相关信息
			if(req.has_file("name") == false || req.has_file("info") == false ||
					req.has_file("video") == false || req.has_file("image") == false)
			{
				resp.status = 400;
				resp.body = R"({"result":false, "reason":"上传的数据信息错误"})";
				resp.set_header("Content-Type", "application/json");
				return;
			}
			httplib::MultipartFormData nameMul = req.get_file_value("name");
			httplib::MultipartFormData infoMul = req.get_file_value("info");
			httplib::MultipartFormData videoMul = req.get_file_value("video");
			httplib::MultipartFormData imageMul = req.get_file_value("image");
			// httplib::MultipartFormData 中的数据：name, content-type, content, filename

			std::string videoName = nameMul.content;
			std::string videoInfo = infoMul.content;
			// 文件名：videoName+videoURL.filename
			std::string root = WWW_ROOT;
			std::string videoPath = root + VIDEO_ROOT + videoName + videoMul.filename;
			std::string imagePath = root + IMAGE_ROOT + videoName + imageMul.filename;
			// 2. 在文件中存储视频信息
			if(UtilFile(videoPath).WriteFile(videoMul.content) == false)
			{
				resp.status = 500;
				resp.body = R"({"result":false, "reason":"视频文件存储失败"})";
				resp.set_header("Content-Type", "application/json");
				return;
			}
			if(UtilFile(imagePath).WriteFile(imageMul.content) == false)
			{
				resp.status = 500;
				resp.body = R"({"result":false, "reason":"视频封面文件存储失败"})";
				resp.set_header("Content-Type", "application/json");
				return;
			}
			// 3. 在数据表(VideoTable)中插入视频信息,就是数据库中
			Json::Value value;
			value["name"] = videoName;
			value["info"] = videoInfo;
			value["videoURL"] = VIDEO_ROOT + videoName + videoMul.filename;
			value["imageURL"] = IMAGE_ROOT + videoName + imageMul.filename;
			if(videoT->Insert(value) == false)
			{
				resp.status = 500;
				resp.body = R"({"result":false, "reason":"数据库新增数据信息失败"})";
				resp.set_header("Content-Type", "application/json");
				return;
			}
		}
		static void Delete(const httplib::Request& req, httplib::Response& resp)
		{
			// 1. 获取删除视频的id
			int videoID = stoi(req.matches[1]);
			// 2. 分别在文件和数据库中删除视频(数据库中只是视频信息)
			Json::Value value;
			if(videoT->SelectID(videoID, &value) == false)
			{
				resp.status = 400;
				resp.body = R"({"result":false, "reason":"要删除的视频不存在"})";
				resp.set_header("Content-Type", "application/json");
				return;
			}
			std::string root = WWW_ROOT;
			std::string videoPath = root + value["videoURL"].asString();
			std::string imagePath = root + value["imageURL"].asString();
			if(remove(videoPath.c_str()) == -1)
			{
				std::cout << "remove: " << videoPath << " error!" << std::strerror(errno) << std::endl;
				return;
			}
			if(remove(imagePath.c_str()) == -1)
			{
				std::cout << "remove: " << imagePath << " error!" << std::strerror(errno) << std::endl;
				return;
			}
			if(videoT->Delete(videoID) == false)
			{
				resp.status = 500;
				resp.body = R"({"result":false, "reason":"数据库中删除视频信息失败"})";
				resp.set_header("Content-Type", "application/json");
				return;
			}
		}
		static void Update(const httplib::Request& req, httplib::Response& resp)
		{
			// 1. 获取要修改的视频id
			int videoID = stoi(req.matches[1]);
			// 2. 分别在文件和数据库中修改视频信息
			Json::Value value;
			if(UtilJson().UnSerialize(req.body, &value) == false)
			{
				resp.status = 400;
				resp.body = R"({"result":false, "reason":"视频信息解析失败"})";
				resp.set_header("Content-Type", "application/json");
				return;
			}
			if(videoT->Update(videoID, value) == false)
			{
				resp.status = 500;
				resp.body = R"({"result":false, "reason":"数据库中修改视频信息失败"})";
				resp.set_header("Content-Type", "application/json");
				return;
			}
		}
		// 模糊查询和查询所有合并
		static void SelectAll(const httplib::Request& req, httplib::Response& resp)
		{
			// 1. 判断是模糊查询还是查询所有
			bool flag = true;
			std::string searchKey;
			// 有参数 search 说明是模糊查询
			if(req.has_param("search"))
			{
				flag = false;
				searchKey = req.get_param_value("search");
			}
			// 2. 进行查询
			Json::Value values;
			if(flag)
			{
				// 查询所有
				if(videoT->SelectAll(&values) == false)
				{
				  resp.status = 400;
				  resp.body = R"({"result":false, "reason":"查询失败"})";
				  resp.set_header("Content-Type", "application/json");
				  return;
				}
			}
			else
			{
				// 模糊查询
				if(videoT->SelectLike(searchKey, &values) == false)
				{
			    resp.status = 400;
			    resp.body = R"({"result":false, "reason":"查询失败"})";
			    resp.set_header("Content-Type", "application/json");
			    return;
				}
			}
			// 3.构建响应
			if(UtilJson().Serialize(values, &(resp.body)) == false)
			{
				resp.status = 500;
				resp.body = R"({"result":false, "reason":"构建响应失败"})";
				resp.set_header("Content-Type", "application/json");
				return;
			}
			resp.set_header("Content-Type", "application/json");
		}
		static void SelectID(const httplib::Request& req, httplib::Response& resp)
		{
			// 1.获取要查询视频的ID
			int videoID = stoi(req.matches[1]);
			// 2.进行查询
			Json::Value value;
			if(videoT->SelectID(videoID, &value) == false)
			{
				resp.status = 400;
				resp.body = R"({"result":false, "reason":"要查询的视频不存在"})";
				resp.set_header("Content-Type", "application/json");
				return;
			}
			// 3.构建响应
			if(UtilJson().Serialize(value, &(resp.body)) == false)
			{
				resp.status = 500;
				resp.body = R"({"result":false, "reason":"构建响应失败"})";
				resp.set_header("Content-Type", "application/json");
				return;
			}
			resp.set_header("Content-Type", "application/json");
		}
	private:
		// 服务器监听端口
		int port_;
		// http 服务
		httplib::Server server_;
	};
}
