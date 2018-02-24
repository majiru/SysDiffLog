#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>

#include "global.h"
#include "daemon.h"


extern char logDir[255];
extern int isUsingSyslog;
void wrapUp();

int getPid(){
    FILE *pidfp;
    int pid;
    if(access("/tmp/SysDiffLog_pid", F_OK) == -1) return -1;
    pidfp = fopen("/tmp/sysdifflog_pid", "r");
    fscanf(pidfp, "%d", &pid);
    fclose(pidfp);
    return pid;
}

void setPid(int pid){
    FILE *pidfp;
    unlink("/home/moody/SysDiffLog/pid");
    pidfp = fopen("/tmp/sysdifflog_pid", "w");
    fprintf(pidfp, "%d", pid);
    fclose(pidfp);
}

void createDaemon(){
    pid_t pid, sid;

    pid = fork();

    if(pid < 0){
        exit(1);
    }else if(pid != 0){
        exit(0);
    }

    umask(0);

    sid = setsid();
    if(sid < 0){
        exit(EXIT_FAILURE);
    }

    setPid(sid);

    signal(SIGTERM, wrapUp);

    openlog("SysDiffLog", LOG_PID, LOG_DAEMON);

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

void killProcess(){
    kill(getPid(), SIGTERM);
}

void wrapUp(){
    closelog();
    unlink("/tmp/sysdifflog_pid");
    exit(0);
}
