#include <iostream>
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/value.h>
#include <jsoncpp/json/writer.h>
#include <sstream>
#include <cstring>

using namespace std;

int main()
{
	const char* name = "zhangsan";
	int age = 11;
	double score[] = {11.2, 22.2, 33.3};
	Json::Value val;
	val["name"] = name;
	val["age"] = age;
	for(int i = 0; i < sizeof(score)/sizeof(score[0]); ++i)
		val["score"].append(score[i]);

	Json::StreamWriterBuilder jsbuil;

	// 智能指针
	unique_ptr<Json::StreamWriter> sw(jsbuil.newStreamWriter());
	stringstream ss;
	// 序列化
	int ret = sw->write(val, &ss);
	if(ret != 0)
	{
		cerr << "write error" << endl;
		return 1;
	}
	cout << ss.str() << endl;

	string str = ss.str();

	Json::Value newVal;
	Json::CharReaderBuilder crb;
	unique_ptr<Json::CharReader> cr(crb.newCharReader());

	string err;
	// 反序列化
	bool parRet = cr->parse(str.c_str(), str.c_str() + str.size(), &newVal, &err);
	if(!parRet)
		cerr << "parse error: " << err << endl;
	cout << "name: " << newVal["name"].asString() << endl;
	cout << "age: " << newVal["age"].asInt() << endl;

	int sz = newVal["score"].size();
	cout << "score: ";
	for(int i = 0; i < sz; ++i)
		cout << newVal["score"][i].asFloat() << "  ";
	cout << endl;

	// 迭代器遍历
	cout << "score: ";
	for(auto it = newVal["score"].begin(); it != newVal["score"].end(); ++it)
		cout << it->asFloat() << "  ";
	cout << endl;

	return 0;
}
