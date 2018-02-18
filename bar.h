
#ifndef __BAR_H__
#define __BAR_H__

struct s_task {
	int timer, base_timer;
	void (*func)(char*);
	char *str;
};


void set_task(struct s_task *task,int timer,
              int base_timer,
              void (*func)(char*),
              char** str,
              int str_len);


void get_ip_addr(char *str);
void update_date(char *out);
void update_acpi(char *out);

#endif