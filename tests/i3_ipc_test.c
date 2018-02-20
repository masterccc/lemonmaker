
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

#define MSG_HEADER_SIZE 14
#define HEADER_IPC "i3-ipc"
#define TO_ASCII(x) (x + 48)

void getSockPath(char *out);
void create_ipc_fd(char *sockpath, int *sock);
void send_change_workspace(int sock);

int main (void){

    int sock, msize ;
    char out[50] = {0};
    getSockPath(out);
    printf("out : <%s>", out);
    create_ipc_fd(out, &sock);
    
    send_change_workspace(sock);

    return 0;

}

void create_ipc_fd(char *sockpath, int *sock){
     
    struct sockaddr_un addr;

    *sock = socket(AF_LOCAL, SOCK_STREAM, 0);
   
    if (*sock == -1) {
        puts("Socket() error");
    }
    else {
        printf("Socket created...");
    

        memset(&addr, 0, sizeof(struct sockaddr_un));
        addr.sun_family = AF_LOCAL;
        strncpy(addr.sun_path, sockpath, sizeof(addr.sun_path) - 1);

        if (connect(*sock, (const struct sockaddr*)&addr, sizeof(struct sockaddr_un)) < 0) {
            puts("connect error");
        }
        else {
            printf("Socket connected...");
        }
    }

}


char* forge_ipc_message(char type, char payload_size, char *payload){

    int i ;
    char *msg = malloc((MSG_HEADER_SIZE + payload_size) * sizeof(char));
    char *p = msg ;

    memcpy(p, HEADER_IPC, strlen(HEADER_IPC));
    p += strlen(HEADER_IPC);
    *p++ = payload_size ;
    
    for(i=0;i<7;i++){
        *p++ = 0 ;
    }

    memcpy(p, payload, payload_size);

    return msg ;
}



void send_change_workspace(int sock){

    char *msg = forge_ipc_message((char)0, (char)11, "workspace 5");
    int i,ret;
    char buffer[1024];

/* 
    Go to workspace 5
    cmd : "workspace 5"
    char msg[] = {
        0x69,0x33,0x2d,0x69,0x70,
        0x63,0x0B,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x77,
        0x6f,0x72,0x6b,0x73,0x70,
        0x61,0x63,0x65,0x20,0x35 };
*/
    ret = send(sock, msg, MSG_HEADER_SIZE + 11,MSG_CONFIRM);

    if(ret <= 0)
        printf("Error (send) : %d", ret);

    for(i=0;i<30;i++){
        read(sock, &buffer,sizeof(buffer));
        printf("%s", buffer);    
    }

    free(msg);

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
