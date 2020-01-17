cmake_minimum_required (VERSION 2.8)
project(libeventDemo)

# 生成服务端
add_executable(libevent_server src/server.c)
# 添加链接库
target_link_libraries(libevent_server event)

# 生成客户端
add_executable(libevent_client src/client.c)
# 添加链接库
target_link_libraries(libevent_client event)