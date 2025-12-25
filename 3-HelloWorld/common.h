#pragma once

// 可変長引数に必要な構造体を定義
// __builtin_** は clang が用意している.
#define va_list     __builtin_va_list
#define va_start    __builtin_va_start
#define va_end      __builtin_va_end
#define va_arg      __builtin_va_arg

void printf(const char *fmt, ...);
