# BrokeBerry
 A simple project exploring raspberry pi and MQTT for manufacturing diagnostic projects



## Setup

### MQTT

MQTT version: Mosquitto 
Source: Article at adafruit

Tutorial how to set on Raspberry Pi
https://www.youtube.com/watch?v=AsDHEDbyLfg

Steps
1 - sudo apt-get install mosquitto
2 - sudo apt-get install mosquitto-clients
3 - change config as necessary on -> /etc/mosquitto/mosquitto.conf
  For testing and dev purpose the following lines were added
    allow_anonymous true
    password_file /etc/mosquitto/pwfile
    listener 1883
4 - sudo mosquitto_passwd -c /etc/mosquitto/pwfile 'username'
    -> username = name of user, it can be anything
5 - After hiting enter, type the password to generate the cryptoghraphy file
6 - To test = mosquitto_sub -d -u 'username' -P 'password' -t dev/test

Compilling
 Problems to identify mosquitto.h -> I had to install the "apt-get install libmosquitto-dev" 
 When compilling: gcc -lmosquitto -o outFileName inFileName.c


### C Compiler

Reference: https://www3.ntu.edu.sg/home/ehchua/programming/cpp/gcc_make.html

Files
  main.c : c program with a main function (entry point)
  file.c : c program with a module for the main.c
  .h : header of the module
  .o : object file, output of the compile but not executable themselves
  .a : notably libraries contain a collection of object files

gcc
  -E Preprocess only, do not compile, assemble or link
  -S : Compile only, do not assemble or link
  -c : compile and assemble (into o files), but do not link
  -o <file> : Place the output into <file>

Example:

1. gcc -c file1.c file2.c file3.c : compile and assemble each files
2. ar -cvq sharedLib.a file1.o file2.o file3.o : create shared library lib.a
3. gcc main.c -lib1 -lib2 sharedLib.a -o mainOutput : 
4. 

### WiringPi Lib

Using wiringPi lib. To compile:

On cmd line after calling compiler "gcc" or "g++", add "-lwiringPi" "-o" outFileName InputFileName.c
 - Where -lwiringPi 
 - Where -o will receive the outFileName and use it as the file output
 
 

### Samba

Used to share folders between different OS

Tutorial: https://www.raspberrypi.org/magpi/samba-file-server/

Steps
1 - apt-get update
2 - apt-get upgrade
3 - apt-get install samba samba-common-bin
4 - Create a directory -> sudo mkdir -m 1777 /share
5 - sudo vi /etc/samba/smb.conf
    [share]
    comment = Pi shared folder
    path = /share
    browsable = yes
    writeable = yes
    onlty guest = no
    create mask = 0777
    directory mask = 0777
    public = yes
6 - Create user -> sudo smbpasswd -a pi
7 - Restart Samba -> sudo /etc/init.d/samba restart
