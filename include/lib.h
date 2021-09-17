#pragma once

#include <types.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <error.h>
#include <assert.h>
#include <task.h>
#include <syscall.h>
#include <paging.h>

#include <x86-64/memory.h>

#define USED(x) (void)(x)

/* main user program */
void umain(int argc, char **argv);

/* libmain.c or entry.S */
extern const char *binary_name;

/* exit.c */
void exit(void);

/* readline.c */
char *readline(const char *buf);

/* syscall.c */
void puts(const char *string, size_t len);
int getc(void);
pid_t getpid(void);
int kill(pid_t);

/* File open modes */
#define O_RDONLY    0x0000      /* open for reading only */
#define O_WRONLY    0x0001      /* open for writing only */
#define O_RDWR      0x0002      /* open for reading and writing */
#define O_ACCMODE   0x0003      /* mask for above modes */

#define O_CREAT     0x0100      /* create if nonexistent */
#define O_TRUNC     0x0200      /* truncate to zero length */
#define O_EXCL      0x0400      /* error if already exists */
#define O_MKDIR     0x0800      /* create directory, not regular file */
