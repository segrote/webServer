# Lab #1 – HTTP Web Server

```
Test Machine: CSEL-KH1250-12
```
March 22, 2022
```


## Algorithms

pid_t pid1;
            char* command[3];
            command[0] = "/usr/bin/perl";
            char* args[] = {"/usr/bin/perl","cgi-bin/hello_post.cgi","NULL"};


            pid1 = fork();
            if(pid1 == 0) {
                dup2(fd,1);
                execv(command[0],args);
                printf("(%d,%d) Error: execv returned... error... exiting\n",thread_id,fd);
                exit(0);
            }
            else {
                ;
            }
