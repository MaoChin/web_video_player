#include <httplib.h>

using namespace std;
using namespace httplib;
int main()
{
	Server server;
	// 设置静态资源根目录
	if(server.set_mount_point("/", "./www") == false)
	{
		cerr << "set_mount_point error" << endl;
		return -1;
	}

	// lambda表达式，设置动态回调函数！
	server.Get("/hi", [](const Request& req, Response& resp){
			resp.set_content("hello world", "test/plain");
		});

	// 正则表达式：\d表示数字，+表示匹配一次或多次，()表示单独捕捉数据
	// 总体表示：字符串是 /number/12 (一个或多个数字)
	server.Get(R"(/number/(\d+))", [&](const Request& req, Response& resp){
			// matches[0] 就是 "/number/12", matches[1]就是 "12"
			auto number = req.matches[1];   // 单独捕捉！！
			resp.set_content(number, "text/plain");
	});

	server.Post("/multipart", [&](const auto& req, auto& resp){
		auto size = req.files.size();
		auto ret = req.has_file("file1");
		const auto& file = req.get_file_value("file1");
		std::cout << file.filename << std::endl;
		std::cout << file.content_type << std::endl;
		std::cout << file.content << std::endl;
	});

	server.listen("0.0.0.0", 8080);
	return 0;
}
