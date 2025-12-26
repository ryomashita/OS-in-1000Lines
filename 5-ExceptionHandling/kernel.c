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

__attribute__((naked))
__attribute__((aligned(4)))
void kernel_entry(void) {
    __asm__ __volatile__(
        "csrw sscratch, sp\n" // sscratch = sp; (sscratch: カーネルが自由に使用できるレジスタ)
        "addi sp, sp, -4 * 31\n" // スタック領域を確保
        // trap_frame 構造体のメモリ配置で, レジスタをスタックに保存
        "sw ra,  4 * 0(sp)\n" // sw rs, offset(base) -> *(base + offset) = rs;
        "sw gp,  4 * 1(sp)\n"
        "sw tp,  4 * 2(sp)\n"
        "sw t0,  4 * 3(sp)\n"
        "sw t1,  4 * 4(sp)\n"
        "sw t2,  4 * 5(sp)\n"
        "sw t3,  4 * 6(sp)\n"
        "sw t4,  4 * 7(sp)\n"
        "sw t5,  4 * 8(sp)\n"
        "sw t6,  4 * 9(sp)\n"
        "sw a0,  4 * 10(sp)\n"
        "sw a1,  4 * 11(sp)\n"
        "sw a2,  4 * 12(sp)\n"
        "sw a3,  4 * 13(sp)\n"
        "sw a4,  4 * 14(sp)\n"
        "sw a5,  4 * 15(sp)\n"
        "sw a6,  4 * 16(sp)\n"
        "sw a7,  4 * 17(sp)\n"
        "sw s0,  4 * 18(sp)\n"
        "sw s1,  4 * 19(sp)\n"
        "sw s2,  4 * 20(sp)\n"
        "sw s3,  4 * 21(sp)\n"
        "sw s4,  4 * 22(sp)\n"
        "sw s5,  4 * 23(sp)\n"
        "sw s6,  4 * 24(sp)\n"
        "sw s7,  4 * 25(sp)\n"
        "sw s8,  4 * 26(sp)\n"
        "sw s9,  4 * 27(sp)\n"
        "sw s10, 4 * 28(sp)\n"
        "sw s11, 4 * 29(sp)\n"

        "csrr a0, sscratch\n" // a0 = sscratch;
        "sw a0, 4 * 30(sp)\n"

        "mv a0, sp\n" // a0 = sp;
        "call handle_trap\n" // handle_trap (カーネル側で定義するC関数) を呼び出す

        // レジスタの値をスタックから復元
        "lw ra,  4 * 0(sp)\n"
        "lw gp,  4 * 1(sp)\n"
        "lw tp,  4 * 2(sp)\n"
        "lw t0,  4 * 3(sp)\n"
        "lw t1,  4 * 4(sp)\n"
        "lw t2,  4 * 5(sp)\n"
        "lw t3,  4 * 6(sp)\n"
        "lw t4,  4 * 7(sp)\n"
        "lw t5,  4 * 8(sp)\n"
        "lw t6,  4 * 9(sp)\n"
        "lw a0,  4 * 10(sp)\n"
        "lw a1,  4 * 11(sp)\n"
        "lw a2,  4 * 12(sp)\n"
        "lw a3,  4 * 13(sp)\n"
        "lw a4,  4 * 14(sp)\n"
        "lw a5,  4 * 15(sp)\n"
        "lw a6,  4 * 16(sp)\n"
        "lw a7,  4 * 17(sp)\n"
        "lw s0,  4 * 18(sp)\n"
        "lw s1,  4 * 19(sp)\n"
        "lw s2,  4 * 20(sp)\n"
        "lw s3,  4 * 21(sp)\n"
        "lw s4,  4 * 22(sp)\n"
        "lw s5,  4 * 23(sp)\n"
        "lw s6,  4 * 24(sp)\n"
        "lw s7,  4 * 25(sp)\n"
        "lw s8,  4 * 26(sp)\n"
        "lw s9,  4 * 27(sp)\n"
        "lw s10, 4 * 28(sp)\n"
        "lw s11, 4 * 29(sp)\n"
        "lw sp,  4 * 30(sp)\n"
        "sret\n" // sret: S-mode からの例外処理を終了して, 例外発生前の命令に復帰する
    );
}

void handle_trap(struct trap_frame *) {
    // CSR は、例外発生時に M-mode でセットされる.
    uint32_t scause = READ_CSR(scause); // scause: 例外の種類
    uint32_t stval = READ_CSR(stval); // stval: scause 別に定義された付加情報 (アドレスなど)
    uint32_t user_pc = READ_CSR(sepc); // sepc: 例外発生時のpcの値

    PANIC("Unexpected trap: scause=0x%x, stval=0x%x, user_pc=0x%x", scause, stval, user_pc);
}

void kernel_main(void) {
    memset(__bss, 0, (size_t)__bss_end - (size_t)__bss); // BSS セクションをゼロクリア

    WRITE_CSR(stvec, (uint32_t)kernel_entry); // 例外発生時のエントリポイントを設定

    // putchar を使い "Hello, World!" を表示
    const char *str = "\nHello, World!\n";
    for (const char *p = str; *p != '\0'; p++) {
        putchar(*p);
    }
    
    // printf を使いフォーマット付きで表示
    printf("1 + 2 = %d, %x\n", 1 + 2, 0x1234abcd);

    // unimp: unimplemented な命令を意図的に呼び出す機能
    __asm__ __volatile__ ("unimp"); 

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
