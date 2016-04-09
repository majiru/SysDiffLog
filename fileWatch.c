#include <stdio.h>
#include <errno.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <syslog.h>

#include "fileWatch.h"

#define EVENT_SIZE  (sizeof(struct inotify_event))
#define BUF_LEN     (1024 * (EVENT_SIZE + 16))

void watch_Files(){    
    int length, i = 0;
    int fd;
    int wd;
    char buffer[BUF_LEN];

    fd = inotify_init();

    if (fd < 0) perror("inotify_init");
    
    wd = inotify_add_watch(fd, ".", IN_MODIFY | IN_CREATE | IN_DELETE);

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

