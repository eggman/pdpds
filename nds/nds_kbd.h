#ifndef _GBA_KBD_H_
#define _GBA_KBD_H_

const char *gba_kbdinput[] = {
    "unix\r",
    "root\r",
    "chdir /work\r",
    "ls -l\r",
    "./fact 100\r",
    "cat hanoi.c\r",
    "./hanoi\r",
    "./hanoi 3\r",
    "chdir /tmp\r",
    "echo 'main() { printf(\"Hello, World!\\n\"); }' > hello.c\r",
    "cc hello.c\r",
    "ls -l\r",
    "./a.out\r",
    "echo '10 prin \"Hello, World!\"' > hello.bas\r",
    "echo 'list' > hello.run\r",
    "echo 'run' >> hello.run\r",
    "bas hello.bas < hello.run\r", 
    "chdir /\r",
    "who\r",
    "tty\r",
    "cal 8 2004\r",
    "mail\r",
    "ps -l\r",
    "ls\r",
    "ls -l /unix\r",
    "ls -l /bin/sh\r",
    "ls -l /etc/init\r",
    "cat /etc/passwd\r",
    "cat /etc/rc\r",
    "chdir /usr/bin\r",
    "echo *\r",
    "chdir /bin\r",
    "echo *\r",
    "chdir /\r",
    "ls -l /lib\r",
    "ls -l /usr\r",
    "ls -l /usr/games\r",
    "cat /usr/pub/greek\r",
    "ls -l /usr/sys\r",
    "cat /usr/sys/proc.h\r",
    "sync\r",
    NULL,
};

#endif /* _GBA_KBD_H_ */
