#define _GNU_SOURCE     /* To get defns of NI_MAXSERV and NI_MAXHOST */

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <linux/if_link.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "bar.h"

#define TOTAL_LENGTH 500

#define IP_SIZE 43
#define THEME_IP_BEGIN "%%{B#11AABB} "
#define THEME_IP_END   " %%{B-}"

#define DATE_SIZE 37
#define THEME_DATE_BEGIN "%%{B#FF0000} "
#define THEME_DATE_END   " %%{B-}"

#define ACPI_SIZE 70
#define ACPI_EXTERN_PROG "/usr/bin/acpi"
#define THEME_ACPI_BEGIN "%%{u#00AA00} "
#define THEME_ACPI_END   " %%{B-}"

#define DSPACE_SIZE 26 + 17
#define THEME_DSPACE_BEGIN "%%{r}%%{B#FAA400} "
#define THEME_DSPACE_END   " %%{B-}"

#define ENABLE_WIDGET(x) tasks[i++] = x

void set_task(struct s_task *task,
              int timer,
              int base_timer,
              void (*func)(char*),
              char** str, int str_len, int *count){

  task->timer = timer ;
  task->base_timer = base_timer ;
  task->func = func ;
  task->str = malloc( str_len * sizeof(char));
  *count += 1;
}
void get_ip_addr(char *str){

  struct ifaddrs *addrs ;
  int family, pass;
  char host[NI_MAXHOST];
  struct ifaddrs *p;

  getifaddrs(&addrs);
  pass = 0 ;
  p = addrs ;
  
  while(p){

    family = p->ifa_addr->sa_family;
    if (family == AF_INET && strcmp("lo",p->ifa_name)) {
      getnameinfo(p->ifa_addr,
        (family == AF_INET) ? sizeof(struct sockaddr_in) :
        sizeof(struct sockaddr_in6),
        host,
        NI_MAXHOST,
        NULL,
        0,
        NI_NUMERICHOST);
      pass++;
      snprintf(str, IP_SIZE, THEME_IP_BEGIN "%s@%s" THEME_IP_END, p->ifa_name, host);
    }
    p = p->ifa_next ;
  }
  
  if(!pass){
    strncpy(str,"NoIP :(",IP_SIZE);
  }

  freeifaddrs(addrs);  
}


void update_date(char *out){
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(out, DATE_SIZE,
        THEME_DATE_BEGIN "%02d/%02d/%02d %02d:%02d" THEME_DATE_END,
        tm.tm_mday,
        tm.tm_mon + 1,
        tm.tm_year + 1900 ,
        tm.tm_hour,
        tm.tm_min);
}

void update_acpi(char *out){

    int pip[2], status ;
    char res[50] = {0}, *p ;
    pid_t pid ;
    /*
    pipe : 
    0 = lire
    1 = écrire
    */
    const char *prog = ACPI_EXTERN_PROG ;
    pipe(pip);
    
    pid = fork();

    if(pid == 0){
        /* child process */
        dup2(pip[1], 1);
        close(pip[0]);
        close(pip[1]);
        execv(prog, (char**)NULL);
        exit(EXIT_SUCCESS);
    }
    else {
        close(pip[1]);
        read(pip[0], res, 50);
        waitpid(pid, &status, 0);
        p = res ;
        while(*p){
            if(*p == '\n')
                *p = ' ';
            p++;
        }
        snprintf(out, ACPI_SIZE, THEME_ACPI_BEGIN
            "%s" THEME_ACPI_END ,res+11);
        
    }
}

void update_dspace(char *out){

    struct statvfs root, home;
    float rspace, hspace ;
    statvfs("/", &root);
    statvfs("/home", &home);

    rspace = 100 - (((float)root.f_bfree / (float)root.f_blocks) * 100);
    hspace = 100 - (((float)home.f_bfree / (float)home.f_blocks) * 100) ;

    snprintf(out, DSPACE_SIZE +6, THEME_DSPACE_BEGIN
        " / : %.2f%% /home : %.2f%%" THEME_DSPACE_END,
        rspace, hspace);

}

void add_task(struct s_task *tasks, struct s_task t){
    static int count = 0 ;
    tasks[count++] = t ;
}


int main(void){

    int i, mod;
    struct s_task bar_ip, bar_date, bar_acpi, bar_dspace ;
    struct s_task *tasks;

    char ban[TOTAL_LENGTH];
    char *ban_p ;
    
    mod = i = 0 ;
    set_task( &bar_ip, 1, 60, get_ip_addr, &bar_ip.str, IP_SIZE, &mod);
    set_task( &bar_date, 1, 60, update_date, &bar_date.str, DATE_SIZE, &mod);
    set_task( &bar_acpi, 1, 120, update_acpi, &bar_acpi.str, ACPI_SIZE, &mod);
    set_task( &bar_dspace, 1, 600, update_dspace, &bar_dspace.str, DSPACE_SIZE, &mod);

    tasks = malloc( mod * sizeof(struct s_task));

    ENABLE_WIDGET(bar_dspace);
    ENABLE_WIDGET(bar_ip);
    ENABLE_WIDGET(bar_acpi);
    ENABLE_WIDGET(bar_date);


    while(1){
        memset(ban,0,TOTAL_LENGTH);
        ban_p = ban ;
        for(i = 0 ; i < mod ; i++){

            tasks[i].timer--;
            if(!tasks[i].timer){
                tasks[i].timer = tasks[i].base_timer ;
                tasks[i].func(tasks[i].str);
            }
            strcat(ban_p, tasks[i].str);
            ban_p += strlen(tasks[i].str);
        }
        printf("%s\n", ban);
        fflush(stdout);
        sleep(1);
    }

}
