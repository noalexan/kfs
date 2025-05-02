#pragma once

#include <types.h>

#define KERN_EMERG   "0"
#define KERN_ALERT   "1"
#define KERN_CRIT    "2"
#define KERN_ERR     "3"
#define KERN_WARNING "4"
#define KERN_NOTICE  "5"
#define KERN_INFO    "6"
#define KERN_DEBUG   "7"
#define KERN_DEFAULT ""
#define KERN_CONT    "c"

void printk(const char *fmt, ...);
