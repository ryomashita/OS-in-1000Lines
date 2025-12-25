#include "kernel.h"
#include "common.h"

extern char __bss[], __bss_end[], __stack_top[];

// sbi_call : SBI の仕様に準拠して SBI の機能を呼び出す
struct sbiret sbi_call(
    long arg0, 
    long arg1,
    long arg2,
    long arg3,
    long arg4,
    long arg5,
    long fid, // FID (Function ID)
    long eid) // EID (Extension ID)
{
    register long a0 __asm__("a0") = arg0;
    register long a1 __asm__("a1") = arg1;
    register long a2 __asm__("a2") = arg2;
    register long a3 __asm__("a3") = arg3;
    register long a4 __asm__("a4") = arg4;
    register long a5 __asm__("a5") = arg5;
    register long a6 __asm__("a6") = fid;
    register long a7 __asm__("a7") = eid;

    // ecall: CPU を (S-mode から) M-mode に切り替えて, SBI の処理ハンドラを呼び出す.
    // システムコール (U-mode から S-mode への切り替え) でも ecall を使う.
    __asm__ __volatile__ (
        "ecall"
        : "=r"(a0), "=r"(a1) // a0 は error code, a1 は 返り値
        : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6), "r"(a7)
        : "memory"
    );

    return (struct sbiret){ .error = a0, .value = a1 };
}

void putchar(char ch) {
    // SBI function; EID #0x01 (Console putchar) を呼び出す
    // Console putchar:
    //      8250 UART (のエミュレータ) のデバイスドライバを呼び出し, 文字を出力する. それを QEMU のシリアルポートに接続しているので文字が表示される.
    //      long sbi_console_putchar(int ch); に相当
    sbi_call(ch, 0, 0, 0, 0, 0, 0, 1 /* Console Putchar */);
}

void kernel_main(void) {
    // putchar を使い "Hello, World!" を表示
    const char *str = "\nHello, World!\n";
    for (const char *p = str; *p != '\0'; p++) {
        putchar(*p);
    }
    
    // printf を使いフォーマット付きで表示
    printf("1 + 2 = %d, %x\n", 1 + 2, 0x1234abcd);

    for(;;) {
        __asm__ __volatile__ ("wfi"); // wfi: CPU を低消費電力モードに移行させる命令
    }
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
