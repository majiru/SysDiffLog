#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <signal.h>

#define EVENT_SIZE  (sizeof(struct inotify_event))
#define BUF_LEN     (1024 * (EVENT_SIZE + 16))

void watch_Files();
void createDaemon();
int getPid();
void setPid();
void wrapUp();

int main(int argc, char *argv[]){
    int pid;
    
    if(argc < 2){
        printf("Usage: start, stop\n");
        printf("Start: Starts Daemon\n");
        printf("Stop: Stops Daemon\n");
        return 1;
    }
    if(!strcmp(argv[1], "start")){
        if((pid = getPid()) != -1 ){
            printf("Program seems to be running under PID %d\n", pid);
        }else{
            createDaemon();
        }
    }else if(!strcmp(argv[1], "stop")){
        kill(getPid(), SIGTERM);
        return 0;
    }
    return 0;
}

int getPid(){
    FILE *pidfp;
    int pid;
    if(access("pid", F_OK) == -1) return -1;
    pidfp = fopen("pid", "r");
    fscanf(pidfp, "%d", &pid);
    fclose(pidfp);
    return pid;
}

void setPid(int pid){
    FILE *pidfp;
    pidfp = fopen("pid", "w");
    fprintf(pidfp, "%d", pid);
    fclose(pidfp);
}

void createDaemon(){
    pid_t pid, sid;
    
    pid = fork();
    setPid(pid);
    if(pid < 0){
        exit(EXIT_FAILURE);
    }else if(pid != 0){
        exit(EXIT_SUCCESS);
    }
    umask(0);

    sid = setsid();
    if(sid < 0){
        exit(EXIT_FAILURE);
    }

    //Change to application directory when applicable
    //chdir("WORKING_DIRECTORY/");

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    openlog("SysDifLog", LOG_PID, LOG_DAEMON);
    watch_Files();
    closelog();
}
    
void watch_Files(){    
    int length, i = 0;
    int fd;
    int wd;
    char buffer[BUF_LEN];

    fd = inotify_init();

    if (fd < 0) perror("inotify_init");
    
    wd = inotify_add_watch(fd, ".", IN_MODIFY | IN_CREATE | IN_DELETE);


    //Prepare for when program needs to stop
    signal(SIGTERM, wrapUp);

    while(1){
        length = read(fd, buffer, BUF_LEN);
        if (length < 0) perror("read");
        i=0;

        while (i < length) {
            struct inotify_event *event = (struct inotify_event *) &buffer[i];
            if (event->len) {
                if (event->mask & IN_CREATE) {
                    syslog(LOG_NOTICE, "The file %s was created.\n", event->name);
                } else if (event->mask & IN_DELETE) {
                    syslog(LOG_NOTICE, "The file %s was deleted.\n", event->name);
                } else if (event->mask & IN_MODIFY) {
                    syslog(LOG_NOTICE, "The file %s was modified.\n", event->name);
                }
            }
            i += EVENT_SIZE + event->len;
        }

    }
    inotify_rm_watch(fd, wd);
    close(fd);
}

void wrapUp(int signum){
    closelog();
    unlink("pid");
    exit(0);
}
