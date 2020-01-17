Linux上运行的基于libevent实现的echo演示程序，通过cmake编译方式。

编译前请先配置libevent的环境: https://www.cnblogs.com/xl2432/p/12206020.html .

编译：
```
mkdir build && cd build
cmake ..

#run server
./libevent_server

#run client
./libevent_client server_ip server_port
```
然后在客户端的控制台直接输入字符串，按回车发送。