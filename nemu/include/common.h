#ifndef __COMMON_H__
#define __COMMON_H__

//#define DEBUG
//#define DIFF_TEST
#define MYDEBUG printf("\033[43m&&&&&NEMU: \033[0m");
#define _my_debug_   MYDEBUG

#if _SHARE
// do not enable these features while building a reference design
//#undef DIFF_TEST
//#undef DEBUG
#endif

/* You will define this macro in PA2 */
#define HAS_IOE

#include <stdint.h>
#include <assert.h>
#include <string.h>

typedef uint8_t bool;

typedef uint32_t rtlreg_t;

typedef uint32_t paddr_t;
typedef uint32_t vaddr_t;

typedef uint16_t ioaddr_t;

#define false 0
#define true 1
//test
volatile static int dummy = 0;

#include "debug.h"
#include "macro.h"

#endif
