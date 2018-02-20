
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define TO_ASCII(x) (x + 48)

void getSockPath(char *out);
void create_ipc_fd(char *sockpath, int *sock);
void send_change_workspace(int sock, int ws);


int main (void){

    int sock ;
    char out[50] = {0};
    getSockPath(out);
    printf("out : <%s>", out);
    create_ipc_fd(out, &sock);
    send_change_workspace(sock,5);

}

void create_ipc_fd(char *sockpath, int *sock){
     
    struct sockaddr_un addr;

    *sock = socket(AF_LOCAL, SOCK_STREAM, 0);
   
    if (*sock == -1) {
        puts("Socket() error");
        return -1;
    }
    else {
        printf("Socket created...");
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_LOCAL;
    strncpy(addr.sun_path, sockpath, sizeof(addr.sun_path) - 1);

    if (connect(*sock, (const struct sockaddr*)&addr, sizeof(struct sockaddr_un)) < 0) {
        puts("connect error");
        return -1;
    }
    else {
        printf("Socket connected...");
    }

}

void send_change_workspace(int sock, int ws){

    /* exit i3 payload */
    /* char msg[] = { 0x69,0x33,0x2d,0x69,0x70,0x63,0x0D,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x65,0x78,0x69,0x74 } ;*/
    
    int i,ret;
    char buffer[1024];

    /* goto workspace 2 */
    char msg[] = { 0x69,0x33,0x2d,0x69,0x70,0x63,0x0B,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x77,0x6f,0x72,0x6b,0x73,0x70,0x61,0x63,0x65,0x20, TO_ASCII(ws) } ;

    ret = send(sock, msg,sizeof(msg),MSG_CONFIRM);

    if(ret <= 0)
        printf("Error (send) : %d", ret);

    for(i=0;i<30;i++){
        read(sock, &buffer,sizeof(buffer));
        printf("%s", buffer);    
    }

}


void getSockPath(char *out){

    int pip[2], status ;
    char res[50] = {0}, *p ;
    pid_t pid ;

    const char *prog = "/usr/bin/i3";
    const char *args = "--get-socketpath" ;

    pipe(pip);
    
    pid = fork();

    if(pid == 0){
        /* child process */
        dup2(pip[1], 1);
        close(pip[0]);
        close(pip[1]);
        execl(prog, "i3" ,args, (char**)NULL);
        exit(EXIT_SUCCESS);
    }
    else {
        close(pip[1]);
        read(pip[0], out, 49);
        waitpid(pid, &status, 0);
        for(p = out;*p; p++ && (*p == '\n'?*p=0:(*p=*p)));
    }  
}
