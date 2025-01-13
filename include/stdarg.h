#pragma once

#define va_start(ap, fmt) (ap = (char *)&fmt)
#define va_arg(ap, type)  *(type *)(ap += sizeof(type))

typedef char *va_list;
