#include "user.h"

// システムコールを呼び出す関数
//      sysno: システムコール番号
//      arg0, arg1, arg2: システムコールの引数
int syscall(int sysno, int arg0, int arg1, int arg2) {
    register int a0 __asm__("a0") = arg0;
    register int a1 __asm__("a1") = arg1;
    register int a2 __asm__("a2") = arg2;
    register int a3 __asm__("a3") = sysno;

    __asm__ __volatile__("ecall" // ecall: U-mode から S-mode に切り替えて, カーネルのシステムコールハンドラを呼び出す
                         : "=r"(a0) // 戻り値は a0 に格納される
                         : "r"(a0), "r"(a1), "r"(a2), "r"(a3)
                         : "memory");

    return a0;
}

extern char __stack_top[]; /* カーネル側のリンカースクリプトでセットされる */

void putchar(char ch) {
    syscall(SYS_PUTCHAR, (int) ch, 0, 0);
}
long getchar(void) {
    return syscall(SYS_GETCHAR, 0, 0, 0);
}
__attribute__((noreturn)) void exit(void) {
    syscall(SYS_EXIT, 0, 0, 0);
    for (;;); // 念のため
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
