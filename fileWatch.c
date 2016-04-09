#include <stdio.h>
#include <errno.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>

#include "fileWatch.h"
#include "global.h"

#define EVENT_SIZE  (sizeof(struct inotify_event))
#define BUF_LEN     (1024 * (EVENT_SIZE + 16))

extern int isUsingSyslog;

void watch_Files(){    
    int length, i = 0;
    int fd;
    int wd;
    char buffer[BUF_LEN];
    char objectType[16] = "NULL";
    char message_buffer[255];

    fd = inotify_init();

    if (fd < 0) perror("inotify_init");
    
    wd = inotify_add_watch(fd, ".", IN_MODIFY | IN_CREATE | IN_DELETE | IN_ISDIR);

    while(1){
        length = read(fd, buffer, BUF_LEN);
        if (length < 0) perror("read");
        i=0;
        message_buffer[0] = '\0';

        while (i < length) {
            struct inotify_event *event = (struct inotify_event *) &buffer[i];
            if (event->len) {
                if(event->mask & IN_ISDIR){
                    strcpy(objectType, "directory");
                }else{
                    strcpy(objectType, "file");
                }
                if (event->mask & IN_CREATE) {
                    sprintf(message_buffer, "The %s %s was created.\n", objectType ,event->name);
                } else if (event->mask & IN_DELETE) {
                   sprintf(message_buffer,"The %s %s was deleted.\n", objectType ,event->name);
                } else if (event->mask & IN_MODIFY) {
                    sprintf(message_buffer, "The %s %s was modified.\n", objectType ,event->name);
                }
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
    inotify_rm_watch(fd, wd);
    close(fd);
}

void logChangesToSyslog(char message[]){
    syslog(LOG_NOTICE, message);
}

void logChangesToFile(char message[]){
    FILE *logfp = fopen("sysDiffLog", "a");
    fprintf(logfp, "%s\n", message);
    fclose(logfp);
}
