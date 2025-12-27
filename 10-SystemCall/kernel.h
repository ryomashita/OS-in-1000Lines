#pragma once
#include "common.h"

struct sbiret {
    long error;
    long value;
};

// Kernel Panic: カーネルが続行不能なエラーに遭遇したとき発生する.
// -> 画面にエラーメッセージを表示して無限ループに入る.
#define PANIC(fmt, ...)                                                        \
    do {                                                                       \
        printf("PANIC: %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__);  \
        while (1) {}                                                           \
    } while (0)

// trap_frame: 例外発生時のレジスタの状態を保存する構造体
struct trap_frame {
    uint32_t ra;
    uint32_t gp;
    uint32_t tp;
    uint32_t t0;
    uint32_t t1;
    uint32_t t2;
    uint32_t t3;
    uint32_t t4;
    uint32_t t5;
    uint32_t t6;
    uint32_t a0;
    uint32_t a1;
    uint32_t a2;
    uint32_t a3;
    uint32_t a4;
    uint32_t a5;
    uint32_t a6;
    uint32_t a7;
    uint32_t s0;
    uint32_t s1;
    uint32_t s2;
    uint32_t s3;
    uint32_t s4;
    uint32_t s5;
    uint32_t s6;
    uint32_t s7;
    uint32_t s8;
    uint32_t s9;
    uint32_t s10;
    uint32_t s11;
    uint32_t sp;
} __attribute__((packed));

// CSR レジスタを読み書きする csrr, csrw 命令を扱うマクロ.
// ({ ... }) は GCC/Clang の拡張構文. 複文を式として扱える (最後の式の値が全体の値になる).
#define READ_CSR(reg)                                                          \
    ({                                                                         \
        unsigned long __tmp;                                                   \
        __asm__ __volatile__("csrr %0, " #reg : "=r"(__tmp));                  \
        __tmp;                                                                 \
    })
#define WRITE_CSR(reg, value)                                                  \
    do {                                                                       \
        uint32_t __tmp = (value);                                              \
        __asm__ __volatile__("csrw " #reg ", %0" ::"r"(__tmp));                \
    } while (0)

// プロセス情報の管理
#define PROCS_MAX 8 // 最大プロセス数
#define PROC_UNUSED     0 // プロセス未使用状態
#define PROC_RUNNABLE   1 // プロセス使用中状態
#define PROC_EXITED     2 // プロセス終了状態

// プロセス管理構造体 (PCB; Process Control Block)
struct process {
    int pid;
    int state; // PROC_UNUSED or PROC_USED
    vaddr_t sp; // stack の末尾アドレス = コンテキストを保存するスタックのポインタ
    uint32_t *page_table; // 1段目ページテーブルを指すポインタ
    // プロセス用スタック領域
    // プロセスごとにカーネルスタックを用意することで、独立したコンテキストを実現できる. 
    uint8_t stack[8182];
};


/* ページテーブルの管理 (Sv32) */

#define SATP_SV32 (1u << 31) // satp レジスタ Sv32 モードのビット

// ページテーブルエントリのフラグ
#define PAGE_V    (1 << 0)   // 有効化ビット
#define PAGE_R    (1 << 1)   // 読み込み可能
#define PAGE_W    (1 << 2)   // 書き込み可能
#define PAGE_X    (1 << 3)   // 実行可能
#define PAGE_U    (1 << 4)   // ユーザーモードでアクセス可能

/* ユーザーモードアプリケーションの実行 */
// ユーザープログラムの格納先アドレス (ここでは raw binary を使用するため、リンカースクリプトと揃えて設定する必要がある)
#define USER_BASE 0x1000000

// sstatuc レジスタの SPIE ビット (Supervisor Previous Interrupt Enable) 
// S-Mode から復帰するとき, 割り込みを有効化する (ハンドラは stvec に設定)
#define SSTATUS_SPIE (1 << 5)

// S-Mode からの ecall による例外コード
#define SCAUSE_ECALL 8

// 関数定義
void yield(void);
