#include "../util.hpp"
#include <iostream>
using namespace std;
using namespace util;

void test_file()
{
  File fi("index.html");
	// cout << fi.Exists() << endl;
	// fi.CreateDir();
	// cout << fi.Exists() << endl;

	// fi.WriteFile("sssssssssss");
	string str;
	fi.ReadFile(&str);
	cout << str << endl;
  cout << fi.Size() << endl;
}

int main()
{
	test_file();


	return 0;
}
