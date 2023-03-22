#include <http.h>
#include <macro.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// #define is_char_ignore(str, at) (str[at] == ' ' || (str[at] == '\r' && str[at + 1] == '\n'))

void http_req_parse(http_client_t* cl);

http_client_t http_client_new(int sockfd);

/*

private define

 */

// typedef struct
// {
//     char buf[DEFAULT_BUFSIZ];
//     int delta_len;
// } info_t;

// const info_t* get_info(const char* src)
// {
//     static info_t local_info;
//     int pos = 0;
//     while (is_char_ignore(src, pos)) {
//         pos++;
//     }
//     int beg = pos;
//     while (!is_char_ignore(src, pos)) {
//         pos++;
//     }
//     memmove(local_info.buf, &src[beg], pos - beg);
//     local_info.buf[pos - beg] = '\0';
//     local_info.delta_len = pos;
//     return &local_info;
// }

const char* get_line(const char* src)
{
    todo("fix to be consitanse");
    static return_buf[DEFAULT_BUFSIZ];
    const char* ptr = src;
    for (int i = 0;; i++) {
        /* code */
        if (ptr[i] == '\r' || ptr[i] == '\n') {
            memmove(return_buf, src, i);
            return_buf[i] = '\0';
            return return_buf;
        }
    }
    return_buf[0] = '\0';
    return return_buf;
}

/* arg line shouldn't include enter, and should end with '\0' */
/* only deal space ' ' */
const char* get_info(const char* line)
{
    static return_buf[1024];
    todo("fix to be consitanse")
    const char* beg = line;
    while (*beg == ' ')
        beg++;
    const char* end = beg;
    while (*end != ' ' && *end != '\0')
        end++;
    
    memmove(return_buf,beg,(size_t)(end-beg));
    return_buf[(size_t)(end-beg)]='\0';

    return return_buf;
}

int http_recv_req(http_client_t* cl)
{
    char buf[DEFAULT_BUFSIZ];
    int n = read(cl->sockfd, buf, DEFAULT_BUFSIZ - 1);
    if (n <= 0) {
        return n;
    }
    buf[n] = '\0';
    strcat(cl->raw_request, buf);
    return n;
}

http_request_t http_req_new()
{
    http_request_t r;
    r.head.eof = 0;
    r.body.len = 0;
    r.status = ON;
    return r;
}
/*


public define


 */
void http_req_parse(http_client_t* cl)
{
    int ret = http_recv_req(cl);
    if (ret <= 0) {
        cl->req.status = BAD;
    }

    const char* empty_line = strstr(cl->raw_request, "\r\n\r\n");
    if (empty_line && !cl->req.head.eof) {
        const char*ptr=cl->raw_request;
        const char*line=get_line(cl->raw_request);
        ptr+=strlen(line);
        /* deal with require line */

        /* deal with req-head */
        for(;ptr!=empty_line;ptr++){
            line=get_line(ptr);
        }

        todo("parse head");
        cl->req.head.eof = 1;
    }
    if (cl->req.head.eof) {
        todo("parse body");
        if (cl->req.head.content_length == cl->req.body.len) { /* include empty body */
            cl->req.body.eof = 1;
            cl->req.status = EOF;
        }
    }
}

http_client_t http_client_new(int sockfd)
{
    http_client_t cl;
    cl.sockfd = sockfd;
    cl.raw_req_len = 0;
    cl.req = http_req_new();
    memset(cl.raw_request, 0, LONG_BUFSIZ);
    return cl;
}