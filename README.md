# 项目--网页视频播放器

总体模块划分：

![image-20231122191222082](E:\Note\项目\项目--网页视频播放器.assets\image-20231122191222082.png)

## 1. 环境搭建

需要的环境：`gcc 7`以上，`gdb`，`mysql`以及其他常用工具。

## 2. 第三方库介绍

需要安装的第三方库：`jsoncpp`，`httplib`

```shell
# ubuntu下
sudo apt install libjsoncpp-dev
sudo apt install libcpp-httplib-dev
```

### 1. `json`介绍

一种常用的`key-value`数据传输格式，整体是一个字符串！主要分为对象，数组，字符串，数组四大类。

1. 对象：用 `{}`括起来的就是一个对象。
2. 数组：用 `[]` 括起来的就是一个数组。
3. 字符串：不多说。
4. 数字：分为整型和浮点型两类。

```json
// json格式示例
[
  {
    "姓名" : "小明",
    "年龄" : 18,
    "成绩" : [88.5, 99, 58]
  },
  {
    "姓名" : "小黑",
    "年龄" : 18,
    "成绩" : [88.5, 99, 58]
  }
]
```

`jsoncpp`就是用来进行`json`格式的序列化和反序列化的库。

一个关键类：`Json::Value`

```C++
//Json数据对象类
class Json::Value{
  //Value重载了[]和=，因此所有的赋值和获取数据都可以通过
  Value &operator=(const Value &other);
  //简单的方式完成 val["姓名"] = "小明";
  Value& operator[](const std::string& key);
  Value& operator[](const char* key);
  Value removeMember(const char* key);  //移除元素
  const Value& operator[](ArrayIndex index) const; //val["成绩"][0]
  //添加数组元素val["成绩"].append(88);
  Value& append(const Value& value);
  //获取数组元素个数 val["成绩"].size();
  ArrayIndex size() const;
  //转string string name =
  std::string asString() const;val["name"].asString();
  //转char* char *name =
  const char* asCString() const;val["name"].asCString();
  //转int int age = val["age"].asInt();
  Int asInt() const;
  float asFloat() const;  //转float
  bool asBool() const;    //转 bool
};
```

序列化方法：`Value`对象 --> `json`串

```C++
//json序列化类，高版本推荐，如果用低版本的接口可能会有警告
// 抽象类
class JSON_API StreamWriter {
  // 纯虚函数
  virtual int write(Value const& root, std::ostream* sout) = 0;
}
class JSON_API StreamWriterBuilder : public StreamWriter::Factory {
  virtual StreamWriter* newStreamWriter() const;
}
```

反序列化方法： `json`串 --> `Value`对象

```C++
//json反序列化类，高版本更推荐
class JSON_API CharReader {
	virtual bool parse(char const* beginDoc, char const* endDoc,
  	Value* root, std::string* errs) = 0;
}
class JSON_API CharReaderBuilder : public CharReader::Factory {
  virtual CharReader* newCharReader() const;
}
```

`jsoncpp`的机制流程图：

![image-20231122213535640](E:\Note\项目\项目--网页视频播放器.assets\image-20231122213535640.png)

### 2. `httplib`库介绍

#### `http`协议

一种常用的应用层协议，内容包括首行(包括请求方法，URL，协议版本；状态码，状态码描述)，头部(`key-value`键值对)，空行，正文。

`httplib`库实现了具体的网络通信，我们只需要直接使用其接口函数即可，不用在自己编写`socket`网络服务器！

```c++
// httplib库中的几个关键类
struct Request {
  std::string method;			//存放请求方法
  std::string path;				//存放请求资源路径
  Headers headers;				//存放头部字段的键值对map
  std::string body;				//存放请求正文
  // for server
  std::string version;		//存放协议版本
  //存放url中查询字符串 key=val&key=val的 键值对map
  Params params;
  MultipartFormDataMap files;		//存放文件上传时，正文中的文件信息
  Ranges ranges;
  bool has_header(const char *key) const;		//判断是否有某个头部字段
  //获取头部字段值
  std::string get_header_value(const char *key, size_t id = 0) const;
  //设置头部字段
  void set_header(const char *key, const char *val);
  //文件上传中判断是否有某个文件的信息
  bool has_file(const char *key) const;
  //获取指定的文件信息
  MultipartFormData get_file_value(const char *key) const;
};
struct Response {
  std::string version;			//存放协议版本
  int status = -1;					//存放响应状态码
  std::string reason;
  Headers headers;					//存放响应头部字段键值对的map
  std::string body;					//存放响应正文
  std::string location; 		// Redirect location重定向位置
  //添加头部字段到headers中
  void set_header(const char *key, const char *val);
  //添加正文到body中
  void set_content(const std::string &s, const char *content_type);
  //设置全套的重定向信息
  void set_redirect(const std::string &url, int status = 302);
};

// 重要的方法类
class Server {
  // 函数指针类型
  using Handler = std::function<void(const Request &, Response &)>;
  // 存放请求-处理函数映射 (一张映射表)
  using Handlers = std::vector<std::pair<std::regex, Handler>>;
  // 线程池
  std::function<TaskQueue *(void)> new_task_queue;
  // 添加指定GET方法的处理映射
  Server &Get(const std::string &pattern, Handler handler);
  Server &Post(const std::string &pattern, Handler handler);
  Server &Put(const std::string &pattern, Handler handler);
  Server &Patch(const std::string &pattern, Handler handler);
  Server &Delete(const std::string &pattern, Handler handler);
  Server &Options(const std::string &pattern, Handler handler);
  // 开始服务器监听
  bool listen(const char *host, int port, int socket_flags = 0);
  // 设置http服务器静态资源根目录(将mount_point路径替换成dir路径)
  bool set_mount_point(const std::string &mount_point, const std::string &dir, Headers headers = Headers());
};
```

`httplib`大致流程：

1. 接受客户端请求数据
2. 对请求数据进行解析，得到`Request`类对象
3. 如果是**请求静态资源**，就会进行静态资源根目录替换，然后到目录中找对应的静态资源，找到了就直接构建`Response`对象返回，否则返回404！若**不是请求静态资源**，就检测`Handlers`映射表，根据请求方法和资源路径查找对应的方法函数。
4. 找到方法函数，就调用，并构建`Response`对象返回，没有对应的方法函数，就返回404！
5. 如果请求是短连接，就关闭连接，否则保持连接。

我们需要做的就是添加方法函数和静态资源！

### 3. `MySQL`的C语言接口库

下面是常用的几个接口函数：

```C
//Mysql操作句柄初始化
MYSQL *mysql_init(MYSQL *mysql)；
//参数为空则动态申请句柄空间进行初始化
//失败返回NULL
//连接mysql服务器
MYSQL *mysql_real_connect(MYSQL *mysql, const char *host, const char *user, const char *passwd,const char *db, unsigned int port,
const char *unix_socket, unsigned long client_flag);
//mysql--初始化完成的句柄
//host---连接的mysql服务器的地址
//user---连接的服务器的用户名
//passwd-连接的服务器的密码
//db ----默认选择的数据库名称
//port---连接的服务器的端口： 默认0是3306端口
//unix_socket---通信管道文件或者socket文件，通常置NULL
//client_flag---客户端标志位，通常置0
//返回值：成功返回句柄，失败返回NULL
//设置当前客户端的字符集
int mysql_set_character_set(MYSQL *mysql, const char *csname)
//mysql--初始化完成的句柄
//csname--字符集名称，通常："utf8"
//返回值：成功返回0， 失败返回非0；
//选择操作的数据库
int mysql_select_db(MYSQL *mysql, const char *db)
//mysql--初始化完成的句柄
//db-----要切换选择的数据库名称
//返回值：成功返回0， 失败返回非0；
//执行sql语句
int mysql_query(MYSQL *mysql, const char *stmt_str)
//mysql--初始化完成的句柄
//stmt_str--要执行的sql语句
//返回值：成功返回0， 失败返回非0；
//保存查询结果到本地
MYSQL_RES *mysql_store_result(MYSQL *mysql)
//mysql--初始化完成的句柄
//返回值：成功返回结果集的指针， 失败返回NULL；
//获取结果集中的行数与列数
uint64_t mysql_num_rows(MYSQL_RES *result)；
//result--保存到本地的结果集地址
//返回值：结果集中数据的条数；
unsigned int mysql_num_fields(MYSQL_RES *result)
//result--保存到本地的结果集地址
//返回值：结果集中每一条数据的列数；
//遍历结果集
MYSQL_ROW mysql_fetch_row(MYSQL_RES *result)
//result--保存到本地的结果集地址
//返回值：实际上是一个char **的指针，将每一条数据做成了字符串指针数组 row[0]-第0列 row[1]-第1列
//并且这个接口会保存当前读取结果位置，每次获取的都是下一条数据
//释放结果集
void mysql_free_result(MYSQL_RES *result)
//result--保存到本地的结果集地址
//返回值：void
//关闭数据库客户端连接，销毁句柄：
void mysql_close(MYSQL *mysql)
//获取mysql接口执行错误原因
const char *mysql_error(MYSQL *mysql)
```

`MySQL`操作基本流程：

1. 初始化`MySQL`操作句柄
2. 连接`MySQL`服务器
3. 设置字符集("UTF8")
4. 执行增删改查语句，其中增删改只需要执行成功即可，**查找语句需要将查找的结果保存到本地**，然后遍历打印结果，最后别忘了**释放结果集。**
5. 最后关闭连接，销毁句柄。

## 3. 服务端数据管理模块

### 1. 数据库的设计

实际的**视频文件和封面图片等文件是存在文件中的**！！！数据库中只存储了视频的相关信息，包括：

1. 视频ID
2. 视频名称
3. 视频描述信息
4. 视频本身的`URL`路径(加上 ==服务器静态资源根目录== 就是其实际存储目录)
5. 视频封面图片的`URL`路径(加上 ==服务器静态资源根目录== 就是其实际存储目录)

### 2. 数据管理类

数据库中的一张表(table)对应一个类，用这个类对象操作这张表---增删查改。视频信息的传递通过`Json::Value`完成！

## 4. 网络通信模块

主要是使用了 `cpp-httplib` 第三方库！！

这一块主要功能就是定义 什么请求是查询请求，什么请求是插入请求，什么请求是删除请求......

### 1. `restful`

`Representational State Transfer  `，一种==接口设计风格==，可以让软件更加清晰，更简洁，更有层次，可维护性更好。通常正文采用 `json`或 `html`格式。

`restful`使用四种 `HTTP` 方法，对应增删查改(`CURD`)操作：

1. `GET` --- 查询/获取
2. `POST`--- 新增
3. `PUT`---修改
4. `DELETE` --- 删除

```shell
# restful 请求接口示例

# 查询所有视频的请求：
GET /video HTTP/1.1
Connection: keep-alive
.....
# 查询指定关键字视频的请求：
GET /video?search="Mysql" HTTP/1.1
.....
# 查询指定视频的请求：
GET /video/1 HTTP/1.1
.....
# 删除指定视频的请求：
DELETE /video/1 HTTP/1.1
.....
# 修改指定视频的请求：
PUT /video/1 HTTP/1
.....

# 上传数据，采用html的格式！
POST /video HTTP/1.1
Content-Type: multipart/form-data;boundary="xxxxxxxxxx"
Content-Length: xxx
...
xxxxxxxxxxxxxxxxxxxxx   # 分割符
Content-Type: xxx; name="name"  # 视频名称
....
xxxxxxxxxxxxxxxxxxxxx
Content-Type: xxx; name="info"  # 视频信息
....
```

```shell
# restful 响应接口示例
HTTP/1.1 200 OK
Content-Length: xxx
Content-Type: xxx
......
[
  {
    "info": "xxx",
    "id": 1,
    "image": "/img/thumbs/mysql.png",
    "name": "Mysql注意事项",
    "video": "/video/movie.mp4",
  },
  {
    "info": "xxx",
    "id": 2,
    "image": "/img/thumbs/linux.png",
    "name": "Linux注意事项",
    "video": "/video/movie.mp4",
  }
]
```

## 5. 业务处理模块

这里主要是接收客户端的请求，并对请求进行解析(网络通信模块)，根据请求类型完成业务处理，并对客户端进行相应。

主要处理以下几个业务：

1. 客户端上传视频
2. 客户端视频列表展示
3. 客户端查询视频
4. 客户端观看视频(获取视频数据)
5. 客户端修改/删除视频

主要处理两个方面：一是文件中数据的新增删除，二是数据库中视频信息的新增删除。

## 6. 前端模块

前端三大件：`html`，`css`，`js`。

`jQuery.ajax`：客户端与服务器进行数据交换的技术。

## 7. 其他工具模块

### 1. 文件工具

主要提供以下接口：

1. 获取文件大小
2. 判断文件是否存在
3. 往文件写入数据
4. 获取文件内数据
5. 创建目录功能

### 2. 序列化反序列化工具

主要是封装了`jsoncpp`库的接口提供了序列化和反序列化的功能。

## 8. 后续扩展

目前只是实现了最简单的视频的增删改查的功能，后续可以从以下几个方向进行扩展：

1. 增加用户管理模块，每个用户需要登录访问。
2. 增加用户的评论，打分等功能。
3. 对视频进行分类管理，热点视频与非热点视频管理。
4. 在服务端对视频图片进行压缩处理(非热点视频进行压缩)，以节省空间。
5. 多用户并发访问的问题。











