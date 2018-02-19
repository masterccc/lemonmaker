# lemonmaker

Generate output for lemonbar (https://github.com/LemonBoy/bar)

Every block can have a different refresh delay.

![alt text](https://raw.githubusercontent.com/masterccc/lemonmaker/master/scrsht.png)

## Available modules :

* Battery level
* Local IP
* Date & hour
* Disk space indicator (/ & /home)

## How to add your own block

* Create its function :

```
void foo_task(char *out){
    /* out is the output of the block */
}
```

* Declare the task among others ```struct s_task bar_ip, bar_date, bar_acpi, bar_foo; ```
* Bind task to function and choose options : 
```
set_task( &bar_foo, /* Block struct */
              1, /* Inital timer, "1" update the block at start (seconds) */
              120, /* Timer before update the block (seconds)*/
              update_acpi, /* update function*/
              &bar_acpi.str, /* output string */
              ACPI_SIZE);  /* output string max lenght */
              ``` 
