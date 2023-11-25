#include "../util.hpp"
#include "../data.hpp"
#include "../server.hpp"
#include <iostream>
#include <jsoncpp/json/value.h>
#include <string>
using namespace std;
using namespace web_video;

void PrintfValue(Json::Value& val)
{
	// id name info videoURL imageURL
	for(int i = 0; i < val.size(); ++i)
	{
	  cout << "id: " << val[i]["id"].asInt() << endl;
	  cout << "info: " << val[i]["info"].asString() << endl;
	  cout << "videoURL: " << val[i]["videoURL"].asString() << endl;
	  cout << "imageURL: " << val[i]["imageURL"].asString() << endl;
		cout << endl;
	}
}
void PrintOne(Json::Value& val)
{
	// id name info videoURL imageURL
	cout << "id: " << val["id"].asInt() << endl;
	cout << "info: " << val["info"].asString() << endl;
	cout << "videoURL: " << val["videoURL"].asString() << endl;
	cout << "imageURL: " << val["imageURL"].asString() << endl;
}

void test_file()
{
  UtilFile fi("index.html");
	// cout << fi.Exists() << endl;
	// fi.CreateDir();
	// cout << fi.Exists() << endl;

	// fi.WriteFile("sssssssssss");
	string str;
	fi.ReadFile(&str);
	cout << str << endl;
  cout << fi.Size() << endl;
}
void test_json()
{
	const char* name = "zhangsan";
	int age = 11;
	double score[] = {11.2, 22.2, 33.3};
	Json::Value val;
	val["name"] = name;
	val["age"] = age;
	for(int i = 0; i < sizeof(score)/sizeof(score[0]); ++i)
		val["score"].append(score[i]);

	string str;
	UtilJson::Serialize(val, &str);
	cout << str << endl;

	Json::Value newVal;
	UtilJson::UnSerialize(str, &newVal);

	cout << "name: " << newVal["name"].asString() << endl;
	cout << "age: " << newVal["age"].asInt() << endl;

	int sz = newVal["score"].size();
	cout << "score: ";
	for(int i = 0; i < sz; ++i)
		cout << newVal["score"][i].asFloat() << "  ";
	cout << endl;
}

void test_data()
{
	web_video::VideoTable videoT;
	Json::Value value;
	// 视频数据表：id name info videoURL imageURL
	value["name"] = "xxx";
	value["info"] = "this is a xxx video";
	value["videoURL"] = "this is the video URL";
	value["imageURL"] = "this is the imageURL";

	// 测试	insert
	// videoT.Insert(value);

	Json::Value newvalue;
	// newvalue["name"] = "111";
	// newvalue["info"] = "this is a 111 video";
	// newvalue["videoURL"] = "this is the video URL";
	// newvalue["imageURL"] = "this is the imageURL";
	// 测试	update
	// videoT.Update(1, newvalue);

	// 测试	delete
  // videoT.Delete(1);

	// 测试 selectAll
	// videoT.SelectAll(&newvalue);
	// PrintfValue(newvalue);
	// string str;
	// UtilJson::Serialize(newvalue, &str);
	// cout << str << endl;

	// 测试 selectId
	// videoT.SelectID(2, &newvalue);
	// PrintOne(newvalue);
	// string str;
	// UtilJson::Serialize(newvalue, &str);
	// cout << str << endl;

	// 测试 selectLike
	videoT.SelectLike("x", &newvalue);
	PrintfValue(newvalue);
	string str;
	UtilJson::Serialize(newvalue, &str);
	cout << str << endl;
}

void test_server()
{
	Server server(8080);

	server.Init();
	server.Run();


}

int main()
{
	// test_file();
	// test_json();
	// test_data();
	test_server();


	return 0;
}
