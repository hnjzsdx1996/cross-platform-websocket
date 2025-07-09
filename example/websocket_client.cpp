#include <libwebsockets.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>

static int interrupted = 0;
static int send_once = 0;
static const char *message = "hnjzsdx";

static void sigint_handler(int sig) {
    interrupted = 1;
}

static int callback_ws(struct lws *wsi, enum lws_callback_reasons reason,
                      void *user, void *in, size_t len) {
    switch (reason) {
        case LWS_CALLBACK_CLIENT_ESTABLISHED:
            printf("[WebSocket] 已连接服务器，准备发送消息...\n");
            lws_callback_on_writable(wsi);
            break;
        case LWS_CALLBACK_CLIENT_WRITEABLE: {
            if (!send_once) {
                unsigned char buf[LWS_PRE + 128];
                size_t msg_len = strlen(message);
                memcpy(&buf[LWS_PRE], message, msg_len);
                lws_write(wsi, &buf[LWS_PRE], msg_len, LWS_WRITE_TEXT);
                printf("[WebSocket] 已发送消息: %s\n", message);
                send_once = 1;
            }
            break;
        }
        case LWS_CALLBACK_CLIENT_RECEIVE:
            printf("[WebSocket] 收到服务器回复: %.*s\n", (int)len, (char *)in);
            interrupted = 1; // 收到回复后退出
            break;
        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
            printf("[WebSocket] 连接出错\n");
            interrupted = 1;
            break;
        case LWS_CALLBACK_CLIENT_CLOSED:
            printf("[WebSocket] 连接关闭\n");
            interrupted = 1;
            break;
        default:
            break;
    }
    return 0;
}

static struct lws_protocols protocols[] = {
    {
        "ws-protocol",
        callback_ws,
        0,
        128,
    },
    { NULL, NULL, 0, 0 } // terminator
};

int main(void) {
    struct lws_context_creation_info info;
    struct lws_client_connect_info ccinfo;
    struct lws_context *context;
    struct lws *wsi;

    signal(SIGINT, sigint_handler);

    memset(&info, 0, sizeof info);
    info.port = CONTEXT_PORT_NO_LISTEN;
    info.protocols = protocols;

    // 可选：提升日志级别，便于调试
    // lws_set_log_level(LLL_ERR | LLL_WARN | LLL_NOTICE | LLL_INFO | LLL_DEBUG | LLL_HEADER | LLL_CLIENT | LLL_LATENCY, NULL);

    context = lws_create_context(&info);
    if (!context) {
        fprintf(stderr, "lws init failed\n");
        return 1;
    }

    memset(&ccinfo, 0, sizeof ccinfo);
    ccinfo.context = context;
    ccinfo.address = "ws.ifelse.io";
    ccinfo.port = 80;
    ccinfo.path = "/";
    ccinfo.host = ccinfo.address;
    ccinfo.origin = ccinfo.address;
    ccinfo.protocol = protocols[0].name;
    ccinfo.ssl_connection = 0; // 关闭SSL，使用明文ws连接

    wsi = lws_client_connect_via_info(&ccinfo);
    if (!wsi) {
        fprintf(stderr, "WebSocket connect failed\n");
        lws_context_destroy(context);
        return 1;
    }

    while (!interrupted)
        lws_service(context, 100);

    lws_context_destroy(context);

    return 0;
}