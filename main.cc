#include "util.hpp"
#include "data.hpp"
#include "server.hpp"

int main()
{
	web_video::Server server(8080);
	server.Init();
	server.Run();
	return 0;
}
