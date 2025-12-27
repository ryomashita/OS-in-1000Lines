#include "user.h"

extern char __stack_top[]; /* カーネル側のリンカースクリプトでセットされる */

__attribute__((noreturn)) void exit(void) {
    for (;;);
}

void putchar(char ch) {
    /* 後で実装する */
}

/* リンカースクリプトで指定したエントリーポイント */
__attribute__((section(".text.start")))
__attribute__((naked))
void start(void) {
    // BSS 領域のゼロクリアは不要（カーネル側で実行されているため）

    __asm__ __volatile__(
        "mv sp, %[stack_top]\n" // スタックポインタを初期化
        "call main\n" // main 関数を呼び出す
        "call exit\n" // exit 関数を呼び出す (user.c)
        ::[stack_top] "r"(__stack_top)); // 
}
