#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <event.h>
#include <event2/bufferevent.h>

#include <arpa/inet.h>

#define PORT 9999

//回调函数申明
void accept_cb(int fd, short events, void* arg);
void read_cb(struct bufferevent* bev, void* arg);
void error_cb(struct bufferevent* bev, short event, void* arg);

int init_tcp_server(int port);

int main(int argc, void* args[]){
    int fd = init_tcp_server(PORT);
    if(fd < 0) {
        return -1;
    }

    //初始化event
    struct event_base* base = event_base_new();

    //添加请求连接事件，并讲event_base对象作为参数传递到回调函数中，以方便进行事件的更改
    struct event* ev_listen = event_new(base, fd, EV_READ|EV_PERSIST, accept_cb, base);
    event_add(ev_listen, NULL);
    
    //进入event处理
    event_base_dispatch(base);
    //释放event
    event_base_free(base);

    return 0;
}


int init_tcp_server(int port) {
    evutil_socket_t listener = socket(PF_INET, SOCK_STREAM, 0);

    if(listener == -1)
    {
        printf("socket() error: %s\n", strerror(errno));
        return -1;
    }

    //设置绑定地址
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = 0;
    sin.sin_port = htons(port);

    //允许地址复用, 需要用在bind之前
    if(evutil_make_listen_socket_reuseable(listener) == -1) {
        printf("evutil_make_listen_socket_reuseable() error: %s\n", strerror(errno));
        evutil_closesocket(listener);
        return -1;
    }

    //允许端口复用
    if(evutil_make_listen_socket_reuseable_port(listener) == -1) {
        printf("evutil_make_listen_socket_reuseable_port() error: %s\n", strerror(errno));
        evutil_closesocket(listener);
        return -1;
    }

    if(bind(listener, (struct sockaddr*)&sin, sizeof(sin)) == -1)
    {
        printf("bind() error: %s\n", strerror(errno));
        evutil_closesocket(listener);
        return -1;
    }

    if(listen(listener, 10) == -1) {
        printf("listen() error: %s\n", strerror(errno));
        evutil_closesocket(listener);
        return -1;
    }

    //设置socket为非阻塞
    if(evutil_make_socket_nonblocking(listener) == -1) {
        printf("evutil_make_socket_nonblocking() error: %s\n", strerror(errno));
        evutil_closesocket(listener);
        return -1;
    }

    //初始化成功
    //printf("Init socket : %d\n", listener);
    return listener;
}

// 处理客户端连接请求事件的回调
void accept_cb(int fd, short events, void* arg) {
    evutil_socket_t sockfd;

    struct sockaddr_in clientAddr;
    socklen_t len = sizeof(clientAddr);

    sockfd = accept(fd, (struct sockaddr*)&clientAddr, &len);
    if(sockfd < 0) {
        printf("accept() error: %s\n", strerror(errno));
        return;
    } else {
        printf("Client connected, fd: %d, addr: %s:%d\n", sockfd, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
    }

    struct event_base* base = (struct event_base*)arg;
    struct bufferevent* bev = bufferevent_socket_new(base, sockfd, BEV_OPT_CLOSE_ON_FREE);
    //设置读回调、写回调和错误时回调
    bufferevent_setcb(bev, read_cb, NULL, error_cb, arg);

    bufferevent_enable(bev, EV_READ|EV_PERSIST);
}

//接收数据时的回调
void read_cb(struct bufferevent* bev, void* arg) {
    char msg[4096];

    //读取数据
    size_t len = bufferevent_read(bev, msg, sizeof(msg)-1);
    msg[len] = '\0';

    printf("Recv client msg: %s\n", msg);

    //将信息回传给客户端
    bufferevent_write(bev, msg, strlen(msg));
}

//处理错误消息的回调
void error_cb(struct bufferevent* bev, short event, void* arg) {
    if(event & BEV_EVENT_EOF) {
        //客户端断开
        printf("connection closed\n");
    } else if(event&BEV_EVENT_ERROR) {
        printf("some other error\n");
    }

    //释放资源,将自动释放套接字和读写缓冲区
    bufferevent_free(bev);
}