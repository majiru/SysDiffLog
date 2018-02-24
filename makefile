default: sysdifflog

sysdifflog: main.c daemon.c fileWatch.c settings.c
	gcc -l config main.c daemon.c fileWatch.c settings.c -o sysDiffLogController
