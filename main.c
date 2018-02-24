#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <libconfig.h>

#include "daemon.h"
#include "global.h"
#include "fileWatch.h"
#include "settings.h"

extern char logDir[255];
extern char directoryToWatch[255];
extern int isUsingSyslog;

int main(int argc, char **argv){
    int pid;
    directoryToWatch[0] = '\0';
    logDir[0] = '\0';
    config_t cfg = initConfig();
    readConfig(&cfg);
    //printf("%s", logDir);
    //printf("%s", directoryToWatch);
    //printf("%d", isUsingSyslog);

    if(argc < 2){
        printf("Usage: start, stop\n");
        printf("Start: Starts Daemon\n");
        printf("Stop: Stops Daemon\n");
        return 1;
    }

    while(*argv != NULL ){
        //printf("Got Arg: %s \n", *argv);
        if(**argv == '-'){
            //printf("Got Flag: %c \n", *(*argv+1)  );
            switch(*(*argv+1)){
                case 'd':
                case 'D':
                    strcpy(directoryToWatch, *(++argv));
                    //printf("Got directory %s \n", directoryToWatch);
                    break;
                case 'l':
                case 'L':
                    strcpy(logDir, *(++argv));
                    //printf("Set logDir as %s\n", logDir);
                    break;
                default:
                    printf("Flag %c is not recognized\n", *(*argv+1));
            }
        }else if(!strcmp(*argv, "start")){
            if((pid = getPid()) != -1 ){
                printf("Program seems to be running under PID %d\n", pid);
            }else{
                createDaemon();
                watch_Files();
                break;
            }
        }else if(!strcmp(*argv, "stop")){
            if(getPid() == -1 ){
                printf("The program appears to not be running or the pid file can not be found/read \n");
                return 2;
            }else{
                killProcess();
                return 0;
            }
        }

        ++argv;
    }
    return 5;
}
