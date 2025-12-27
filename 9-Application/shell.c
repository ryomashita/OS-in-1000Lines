#include "user.h"

// user.c の start 関数から呼び出される main 関数
void main(void) {
    // *((volatile int *) 0x80200000) = 0x1234; // 正しく U-Mode で実行していれば、ページフォルトが発生しパニックする.
    for (;;);
}
