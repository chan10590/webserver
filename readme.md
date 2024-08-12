非阻塞+ET+epoll
1）在v2.1基础上将Server类监听和连接分离，更加模块化
2）服务器的工作是依赖一系列的回调函数
|事件|回调|
|:-:|:-:|
|监听套接字发生读事件|Listener::addNewSocket()---新建通信套接字, Server::newConnection(Socket* sock)---为通信套接字设置回调，存放在map中是为了方便后续断开连接|
|通信套接字发生读事件|Connection::handleCommun()---处理通信请求|
|客户端断开连接|Server::deleteConnection(Socket* sock)---删除连接|

切换到test目录下，运行以下命令
```bash
make server     //编译服务器
make client     //编译客户端
./server 9527   //可自己指定端口号
./client 9527   //可自己指定端口号
```
