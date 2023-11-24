#include "../util.hpp"
#include <iostream>
#include <jsoncpp/json/value.h>
#include <string>
using namespace std;
using namespace web_video;

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

int main()
{
	// test_file();
	test_json();


	return 0;
}
