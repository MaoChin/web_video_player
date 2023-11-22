#include <iostream>
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/value.h>
#include <jsoncpp/json/writer.h>
#include <sstream>

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

	return 0;
}
