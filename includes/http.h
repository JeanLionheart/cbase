#ifndef __HTTPH__
#define __HTTPH__

#include<macro.h>

typedef struct http_request_s http_request_t;

typedef struct http_respond_s http_respond_t;

typedef struct http_client_s http_client_t;

#define DEFAULT_HOST_CAP 100

#define DEFAULT_URL_CAP 100

enum http_method_t { GET,
    POST };

enum file_type_t { TEXT /* ,IMAGE,VIDEO,APPLICATION */ };

enum file_format_t { HTML,
    PLAIN };

enum connect_status_t { ON,
    OFF,
    BAD,EOF };

enum protocal_t { HTTP1D1 };

struct http_request_s {
    struct {
        char http_method;
        const char* protocal;
        char url[DEFAULT_URL_CAP];
        char host[DEFAULT_HOST_CAP];
        char file_type;
        char file_format;
        int content_length;
        char eof;

        /* todo */
        char char_set;
        char encoding;
    } head;

    /* todo */
    struct
    {
        /* data */
        char content[LONG_BUFSIZ];
        int len;
        char eof;
    } body;
    int status;
};

struct http_client_s {
    int sockfd;
    char raw_request[LONG_BUFSIZ];
    int raw_req_len;
    http_request_t req;
};

struct http_respond_s {
    /* data */
};

void http_req_parse(http_client_t* cl);

http_client_t http_client_new(int sockfd);

#undef DEFAULT_URL_CAP

#endif