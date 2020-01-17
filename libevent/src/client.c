#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

#include <event.h>
#include <event2/bufferevent.h>

int tcp_connect_server(const char* server_ip, const int port);

//回调申明
void error_cb(struct bufferevent* bev, short event, void* arg);
void input_cb(int fd, short events, void* arg);
void read_msg_cb(struct bufferevent* bev, void* arg);

int main(int argc, char* argv[]){
    if(argc != 3) {
        printf("Usage: libevent_client ip port");
        return -1;
    }

    //两个参数分别作为服务器的ip和端口号
    int sockfd = tcp_connect_server(argv[1], atoi(argv[2]));
    if(sockfd < 0) {
        printf("tcp_connect_server failed\n");
    } else {
        printf("Success to connect server\n");
    }

    //配置event
    struct event_base* base = event_base_new();
    struct bufferevent* bev = bufferevent_socket_new(base, sockfd, BEV_OPT_CLOSE_ON_FREE);

    //添加终端输入事件
    struct event* event_input = event_new(base, STDIN_FILENO, EV_READ | EV_PERSIST, input_cb, (void*)bev);
    event_add(event_input, NULL);

    bufferevent_setcb(bev, read_msg_cb, NULL, error_cb, (void*)event_input);
    bufferevent_enable(bev, EV_READ|EV_PERSIST);

    event_base_dispatch(base);

    printf("CLose client\n");

    return 0;
}

int tcp_connect_server(const char* server_ip, const int port) {
    evutil_socket_t sockfd;
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if(inet_aton(server_ip, &server_addr.sin_addr) <= 0) {
        printf("error: Invalid server ip: %s\n", server_ip);
        return -1;
    }

    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        printf("socket() error: %s\n", strerror(errno));
        return -1;
    }

    if(connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        printf("connect() error: %s\n", strerror(errno));
        evutil_closesocket(sockfd);
        return -1;
    }

    //设置非阻塞
    if(evutil_make_socket_nonblocking(sockfd) == -1) {
        printf("evutil_make_socket_nonblocking() error: %s\n", strerror(errno));
        evutil_closesocket(sockfd);
        return -1;
    }

    return sockfd;
}

void error_cb(struct bufferevent* bev, short event, void* arg) {
    if(event & BEV_EVENT_EOF) {
        printf("Connection closed\n");
    } else if(event & BEV_EVENT_ERROR) {
        printf("Other error\n");
    }

    //释放socket和读写缓冲区
    bufferevent_free(bev);

    //删除event
    struct event* event_input = (struct event*)arg;
    event_free(event_input);
}

void input_cb(int fd, short events, void* arg) {
    char msg[4096];
    memset(msg, 0, sizeof(msg));

    int len = read(fd, msg, sizeof(msg));
    if(len <= 0 ) {
        perror("read input failed: ");
        return;
    }

    struct bufferevent* bev = (struct bufferevent*)arg;

    //发送消息给服务器
    bufferevent_write(bev, msg, strlen(msg)+1);
}

void read_msg_cb(struct bufferevent* bev, void* arg) {
    char msg[4096];
    memset(msg, 0, sizeof(msg));

    size_t len = bufferevent_read(bev, msg, sizeof(msg));
    msg[len] = '\0'; 
    printf("recv msg from server: %s\n", msg);
}