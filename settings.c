#include <stdio.h>
#include <stdlib.h>
#include <libconfig.h>
#include <string.h>

#include "global.h"

extern char logDir[255];
extern char directoryToWatch[255];
extern int isUsingSyslog;

config_t initConfig(){
    config_t cfg;
    config_init(&cfg);
    if(!config_read_file(&cfg, "sysdifflog.cfg")){
       fprintf(stderr, "%s:%d - %s\n",
            config_error_file(&cfg),
            config_error_line(&cfg),
            config_error_text(&cfg));
        config_destroy(&cfg);
    }
    return cfg;
}

int readConfig(config_t *cf){
    const char *buffer = NULL;
    if(!config_lookup_string(cf, "logDir", &buffer)){
        fprintf(stderr, "Error reading log directory");
        return(0);
    }
    strcpy(logDir, buffer);
    if(!config_lookup_string(cf, "directoryToWatch", &buffer)){
        fprintf(stderr, "Error reading directory(s) to watch");
        return(0);
    }
    strcpy(directoryToWatch, buffer);
    if(!config_lookup_bool(cf, "useSyslog", &isUsingSyslog)){
        fprintf(stderr, "Error reading sys log flag");
        return(0);
    }
    return 1;
}
