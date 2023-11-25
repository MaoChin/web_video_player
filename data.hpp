#pragma once
// 数据管理模块，主要是操作数据库进行增删改查

#include <cstdio>
#include <jsoncpp/json/value.h>
#include <mutex>

#include "util.hpp"
#include <mysql/mysql.h>

namespace web_video
{
#define HOST "127.0.0.1"
#define USER "root"
#define PASSWORD ""
#define DBNAME "web_video_info"
#define PORT 3306
#define CHARACTER "utf8"

	// 初始化句柄
	static MYSQL* MysqlInit()
	{
		MYSQL* mysql = mysql_init(nullptr);
		if(mysql == nullptr)
		{
			std::cerr << "mysql_init error: " << std::endl;
			return nullptr;
		}

		// 建立连接
		if(mysql_real_connect(mysql, HOST, USER, PASSWORD, DBNAME, PORT, nullptr, 0) == nullptr)
		{
			std::cerr << "mysql_real_connect error: " << mysql_error(mysql);
			mysql_close(mysql);
			return nullptr;
		}

		mysql_set_character_set(mysql, CHARACTER);
		mysql_select_db(mysql, DBNAME);
		return mysql;
	}
	// 销毁句柄
	static void MysqlDestory(MYSQL* mysql)
	{
		if(!mysql) mysql_close(mysql);
	}
	// 执行
	static bool MysqlQuery(MYSQL* mysql, const std::string& sql)
	{
		int ret = mysql_query(mysql, sql.c_str());
		if(ret != 0)
		{
			std::cerr << "mysql_query: " << sql << " error: " << mysql_error(mysql) << std::endl;
			return false;
		}
		return true;
	}


	// 视频数据表
	class VideoTable
	{
	public:
		VideoTable()
			:mysql_(nullptr)
		{
			mysql_ = MysqlInit();
			if(!mysql_) exit(1);
		}

		~VideoTable()
		{
			if(!mysql_) MysqlDestory(mysql_);
		}

		// 插入一条视频信息
		bool Insert(const Json::Value& video)
		{
			// 视频数据表的结构：id name info video image
			// 对 video 的数据进行校验 ...
			// char sql[2048 + video["info"].asString().size()];
			std::string sql;
			sql.resize(2048 + video["info"].asString().size());
			#define INSERT_SQL "insert into web_video values(null, '%s', '%s', '%s', '%s');"
			std::sprintf(&sql[0], INSERT_SQL, video["name"].asCString(),
					video["info"].asCString(), video["video"].asCString(),
					video["image"].asCString());
			return MysqlQuery(mysql_, sql);
		}
		// 更新一条视频信息
		bool Update(const int videoID, const Json::Value& newVideo)
		{
			// 校验...
			std::string sql;
			sql.resize(2048 + newVideo["info"].asString().size());
			#define UPDATE_SQL "update web_video set name='%s', info='%s', video='%s', image='%s' where id='%d';"
			std::sprintf(&sql[0], UPDATE_SQL, newVideo["name"].asCString(),
					newVideo["info"].asCString(), newVideo["video"].asCString(),
					newVideo["image"].asCString(), videoID);
			return MysqlQuery(mysql_, sql);
		}
		// 删除一条视频信息
		bool Delete(const int videoID)
		{
			std::string sql;
			sql.resize(1024);
			#define DELETE_SQL "delete from web_video where id='%d';"
			std::sprintf(&sql[0], DELETE_SQL, videoID);
			return MysqlQuery(mysql_, sql);
		}
		// 查询所有视频信息
		bool SelectAll(Json::Value* videos)
		{
			const char* sql = "select * from web_video;";
			// 加锁...
			mutex_.lock();
			bool ret = MysqlQuery(mysql_, sql);
			if(!ret)
			{
				mutex_.unlock();
				return false;
			}
			MYSQL_RES* res = mysql_store_result(mysql_);
			if(res == nullptr)
			{
				std::cout << "mysql_store_result error: " << mysql_error(mysql_);
				mutex_.unlock();
				return false;
			}
			mutex_.unlock();

			Json::Value video;
			int rows = mysql_num_rows(res);
			for(int i = 0; i < rows; ++i)
			{
				MYSQL_ROW rowInfo = mysql_fetch_row(res);
				// id 是 int 类型
				video["id"] = std::stoi(rowInfo[0]);
				video["name"] = rowInfo[1];
				video["info"] = rowInfo[2];
				video["video"] = rowInfo[3];
				video["image"] = rowInfo[4];
				videos->append(video);
			}
			// 释放资源
			mysql_free_result(res);
			return true;
		}
		// 查询指定ID的视频信息
		bool SelectID(const int videoID, Json::Value* video)
		{
			#define SELECTID_SQL "select * from web_video where id='%d';"
			char sql[1024];
			std::sprintf(sql, SELECTID_SQL, videoID);

			// 加锁...
			mutex_.lock();
			bool ret = MysqlQuery(mysql_, sql);
			if(!ret)
			{
				mutex_.unlock();
				return false;
			}
			MYSQL_RES* res = mysql_store_result(mysql_);
			if(res == nullptr)
			{
				std::cout << "mysql_store_result error: " << mysql_error(mysql_);
				mutex_.unlock();
				return false;
			}
			mutex_.unlock();

			// row 要么为0，要么为1
			int row = mysql_num_rows(res);
			if(row == 0)
			{
				std::cerr << "要查找的视频信息不存在！" << std::endl;
				mysql_free_result(res);
				return false;
			}
			MYSQL_ROW rowInfo = mysql_fetch_row(res);
			(*video)["id"] = videoID;
			(*video)["name"] = rowInfo[1];
			(*video)["info"] = rowInfo[2];
			(*video)["video"] = rowInfo[3];
			(*video)["image"] = rowInfo[4];
			mysql_free_result(res);
			return true;
		}
		// 根据视频名称模糊查询视频信息
		bool SelectLike(const std::string& videoName, Json::Value* videos)
		{
			// %...%
			#define SELECTLIKE_SQL "select * from web_video where name like '%%%s%%';"
			char sql[1024];
			std::sprintf(sql, SELECTLIKE_SQL, videoName.c_str());

			// 加锁...
			mutex_.lock();
			bool ret = MysqlQuery(mysql_, sql);
			if(!ret)
			{
				mutex_.unlock();
				return false;
			}
			MYSQL_RES* res = mysql_store_result(mysql_);
			if(res == nullptr)
			{
				std::cout << "mysql_store_result error: " << mysql_error(mysql_);
				mutex_.unlock();
				return false;
			}
			mutex_.unlock();

			Json::Value video;
			int rows = mysql_num_rows(res);
			for(int i = 0; i < rows; ++i)
			{
				MYSQL_ROW rowInfo = mysql_fetch_row(res);
				// id 是 int 类型
				video["id"] = std::stoi(rowInfo[0]);
				video["name"] = rowInfo[1];
				video["info"] = rowInfo[2];
				video["video"] = rowInfo[3];
				video["image"] = rowInfo[4];
				videos->append(video);
			}
			// 释放资源
			mysql_free_result(res);
			return true;
		}
	private:


	private:
		MYSQL* mysql_;
		std::mutex mutex_;
	};
}
