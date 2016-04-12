#include <stdio.h>
#include <errno.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "fileWatch.h"
#include "global.h"

#define EVENT_SIZE  (sizeof(struct inotify_event))
#define BUF_LEN     (1024 * (EVENT_SIZE + 16))

extern int isUsingSyslog;
extern char directoryToWatch[255];

void watch_Files(){    
    int length, i = 0;
    int fd;
    char buffer[BUF_LEN];
    char objectType[16] = "NULL";
    char message_buffer[255];
    char *currentTime;


    fd = inotify_init();

    if (fd < 0) perror("inotify_init");
    
    parseDirectoryInput(directoryToWatch, fd);
    
    logChangesToSyslog("--SysDiffLogStarted--");
    while(1){
        length = read(fd, buffer, BUF_LEN);
        if (length < 0) perror("read");
        i=0;
        message_buffer[0] = '\0';

        while (i < length) {
            struct inotify_event *event = (struct inotify_event *) &buffer[i];
            if (event->len) {
                currentTime = getCurrentTimeString();
                if(event->mask & IN_ISDIR){
                    strcpy(objectType, "directory");
                }else{
                    strcpy(objectType, "file");
                }
                if (event->mask & IN_CREATE) {
                    sprintf(message_buffer, "%s: The %s %s was created.\n", currentTime, objectType ,event->name);
                } else if (event->mask & IN_DELETE) {
                   sprintf(message_buffer,"%s: The %s %s was deleted.\n", currentTime, objectType ,event->name);
                } else if (event->mask & IN_MODIFY) {
                    sprintf(message_buffer, "%s: The %s %s was modified.\n", currentTime, objectType ,event->name);
                }
                free(currentTime);
            }
            
            if(message_buffer[0] != '\0'){
                if(isUsingSyslog){
                    logChangesToSyslog(message_buffer);
                }else{
                    logChangesToFile(message_buffer);
                }
            }

            i += EVENT_SIZE + event->len;
        }

    }
}

void logChangesToSyslog(char message[]){
    syslog(LOG_NOTICE, message);
}

char *getCurrentTimeString(){
    char *tBuff = malloc (sizeof(char) * 100);
    time_t currentTime = time (0);
    strftime (tBuff, 100, "%Y-%m-%d %H:%M:%S", localtime (&currentTime));
    return tBuff;
}

void logChangesToFile(char message[]){
    FILE *logfp = fopen("sysDiffLog", "a");
    fprintf(logfp, "%s\n", message);
    fclose(logfp);
}

void parseDirectoryInput(char argument[], int fd){
    int i, x;
    x = 0;
    char buffer[255];
    for(i=0; argument[i] != '\0'; i++){
        if(argument[i] == ','){
            inotify_add_watch(fd, buffer, IN_MODIFY | IN_CREATE | IN_DELETE | IN_ISDIR);
            x=0;
        }else{
            buffer[x] = argument[i];
            buffer[++x] = '\0';
        }
    }
}
