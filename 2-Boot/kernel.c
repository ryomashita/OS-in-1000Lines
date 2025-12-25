typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef uint32_t size_t;

// リンカースクリプト (kernel.ld) で値がセットされるシンボル
extern char __bss[], __bss_end[], __stack_top[];

// memset : buf の先頭から n バイト分を c で埋める
void *memset(void *buf, char c, size_t n) {
    uint8_t *p = (uint8_t *)buf;
    while (n--) {
        *p++ = (uint8_t)c;
    }
    return buf;
}

void kernel_main(void) {
    // BSS 領域を 0 初期化 (ブートローダなど他のプログラムで初期化してくれるケースもあるが、ここでは明確に初期化する)
    memset(__bss, 0, (size_t)__bss_end - (size_t)__bss);

    for(;;);
}

__attribute__((section(".text.boot")))
__attribute__((naked)) // Function prologue/epilogue を生成しない -> 書いたアセンブリがそのまま配置される
void boot(void) {
    __asm__ __volatile__ (
        "mv sp, %[stack_top] \n" // スタック領域の末尾アドレスを sp に設定
        "j kernel_main      \n" // kernel_main 関数へジャンプ
        :
        : [stack_top] "r" (__stack_top)
    );
}
