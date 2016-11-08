default: sysdifflog

sysdifflog: main.c daemon.c fileWatch.c  
	gcc main.c daemon.c fileWatch.c -o SysDiffLogController
