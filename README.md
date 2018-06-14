# SysDiffLog
A utility for creating a comprehensive log for system changes. This program works by monitoring the inodes of files, and printing out to a log upon any changes in the inode. 

The program will monitor all files in a given directory, and multiple directories are supported if seperated my a comma(',')

Right now the program supports output to the UNIX syslog as well as a standard text file.

## Compiling
To compile the program from source you will need the C libconfig libraries(installable from most package managers) and GNU make to use the makefile.

Then it is simply `make` in the directory of the repo to compile the binary.

## Configuring SysDiffLog
There are two ways to set up SysDiffLog, either with the configuration file included with the repo or to use flags when running the program.

### Config files
Simply edit the SysDiffLog.cfg file by setting directoryToWatch to the comma seperated list of directories. Then if you would like the output to be redirected to SYSLOG simply ensure useSyslog is set to True. If you would like to use a static text file, then change the logDir to the folder where you would like SysDiffLog to dump the log file.  The sample .cfg file in the repo already contains these fields.

### Run time flags
SysDiffLog can also be configured by using flags at runtime, using the -d or -D flags sets the directory to watch for file changes, while the -l or -L flags set the log file directory. SysDiffLog will use the SYSLOG if no log directory is defined. 


Example:
`./sysDiffLogController -D /home/importantUser/ start` will monitor all changes to the importantUser home directory and log the changes to the syslog

## Using SysDiffLog
After configuring your flags or .cfg file, run the program with: `./sysDiffLogController <flags> start`to start the program.

SysDiffLog will try to fork itself and run in the background, storing it's PID inside of a pid file for referance.

Running `./sysDiffLogController stop` will attempt to kill the process by reading the PID from the pid file.

## About
I wrote this program for use with the ISEAGE IT Olympics CDC and to help myself learn more about the C programming language

License
=======
```
SysDiffLog - An easy way to view and track system changes.
Copyright (C) 2016 Jacob Moody

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
```
