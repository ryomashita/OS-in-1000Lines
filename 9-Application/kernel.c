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

// kernel_enty: 
//      例外ハンドラのエントリポイント
//      kernel_main でこの関数のアドレスをエントリポイントとしてセットする.
__attribute__((naked))
__attribute__((aligned(4)))
void kernel_entry(void) {
    __asm__ __volatile__(
        // yield() でプロセスのコンテキストの sp を sscratch に設定しているので,
        // sp に sscratch の値を読み込む. 
        // (※ sp はユーザープロセスで変更されている可能性があるため, 信用できない. 代わりにカーネルが sscratch に sp を保存しておく.)
        "csrrw sp, sscratch, sp\n"
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

        // 例外発生時のspを取り出して保存
        "csrr a0, sscratch\n"
        "sw a0,  4 * 30(sp)\n"
        // カーネルスタックを設定し直す
        "addi a0, sp, 4 * 31\n"
        "csrw sscratch, a0\n"

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

void handle_trap(struct trap_frame *f) {
    (void)f; // unused
    // CSR は、例外発生時に M-mode でセットされる.
    uint32_t scause = READ_CSR(scause); // scause: 例外の種類
    uint32_t stval = READ_CSR(stval); // stval: scause 別に定義された付加情報 (アドレスなど)
    uint32_t user_pc = READ_CSR(sepc); // sepc: 例外発生時のpcの値

    PANIC("Unexpected trap: scause=0x%x, stval=0x%x, user_pc=0x%x", scause, stval, user_pc);
}

extern char __free_ram[], __free_ram_end[];

// n ページ分の物理メモリを割り当てて, その先頭アドレスを返す
// これだけではメモリ解放ができない.
paddr_t alloc_pages(uint32_t n) {
    static paddr_t next_paddr = (paddr_t)__free_ram; // static 変数の初期化は一度だけ行われる点に注意
    paddr_t paddr = next_paddr;
    next_paddr += n * PAGE_SIZE;

    if (next_paddr > (paddr_t)__free_ram_end) {
        PANIC("Out of memory!");
    }
    
    // 割り当てたメモリをゼロクリア
    memset((void *)paddr, 0, n * PAGE_SIZE);
    return paddr;
}


/* ページテーブルの管理 (Sv32) */

// map_page: ページテーブルにエントリを追加する
//      エントリのデータ形式は RISC-V Sv32 仕様に準拠している.
//      table1: 1段目のページテーブルの先頭アドレス
//      vaddr: 仮想アドレス, paddr: 物理アドレス
//      flags: ページテーブルエントリのフラグ (R/W/X/U など)
void map_page(uint32_t *table1, uint32_t vaddr, paddr_t paddr, uint32_t flags) {
    if (!is_aligned(vaddr, PAGE_SIZE))
        PANIC("unaligned vaddr %x", vaddr);

    if (!is_aligned(paddr, PAGE_SIZE))
        PANIC("unaligned paddr %x", paddr);

    uint32_t vpn1 = (vaddr >> 22) & 0x3ff; // VPN[1] : 1段階目ページテーブルのindex
    if ((table1[vpn1] & PAGE_V) == 0) { // 有効化フラグが立っていないか
        // 1段目のページテーブルを作成する.
        uint32_t pt_paddr = alloc_pages(1);
        table1[vpn1] = ((pt_paddr / PAGE_SIZE) << 10) | PAGE_V; // ページ番号をセット (下位10bit はフラグ用)
    }

    // 2段目のページテーブルにエントリを追加する
    uint32_t vpn0 = (vaddr >> 12) & 0x3ff; // VPN[0] : 2段階目ページテーブルのindex
    uint32_t *table0 = (uint32_t *) ((table1[vpn1] >> 10) * PAGE_SIZE);
    table0[vpn0] = ((paddr / PAGE_SIZE) << 10) | flags | PAGE_V; // 物理アドレスをセット (下位10bit はフラグ用)
}

/* ユーザーアプリケーションの実行 */

// shell.bin.o のシンボル
extern char _binary_shell_bin_start[], _binary_shell_bin_size[];

__attribute__((naked)) void user_entry(void) {
    __asm__ __volatile__(
        "csrw sepc, %[sepc]\n" // 復帰後の pc = U-Mode のエントリーポイントをセット
        "csrw sstatus, %[sstatus]\n" // S-Mode を抜けた先 (U-Mode) で割り込みを有効化する
        "sret\n" // S-mode を終了し U-Mode に復帰する (移行先モードは sstatus の SPP ビットで決まる)
        :
        : [sepc] "r" (USER_BASE),
          [sstatus] "r" (SSTATUS_SPIE)
    );
}

/* プロセス */

struct process procs[PROCS_MAX];

// コンテキストスイッチを実行する.
//  コンテキストスイッチ = 各プロセスに紐づくスタック領域にレジスタの状態を保存・復元する.
__attribute__((naked)) void switch_context(uint32_t *prev_sp,
                                           uint32_t *next_sp) {
    __asm__ __volatile__(
        // 実行中プロセスのスタックへレジスタを保存
        "addi sp, sp, -13 * 4\n"
        "sw ra,  0  * 4(sp)\n"
        "sw s0,  1  * 4(sp)\n"
        "sw s1,  2  * 4(sp)\n"
        "sw s2,  3  * 4(sp)\n"
        "sw s3,  4  * 4(sp)\n"
        "sw s4,  5  * 4(sp)\n"
        "sw s5,  6  * 4(sp)\n"
        "sw s6,  7  * 4(sp)\n"
        "sw s7,  8  * 4(sp)\n"
        "sw s8,  9  * 4(sp)\n"
        "sw s9,  10 * 4(sp)\n"
        "sw s10, 11 * 4(sp)\n"
        "sw s11, 12 * 4(sp)\n"

        // スタックポインタの切り替え
        "sw sp, (a0)\n"
        "lw sp, (a1)\n"

        // 次のプロセスのスタックからレジスタを復元
        "lw ra,  0  * 4(sp)\n"
        "lw s0,  1  * 4(sp)\n"
        "lw s1,  2  * 4(sp)\n"
        "lw s2,  3  * 4(sp)\n"
        "lw s3,  4  * 4(sp)\n"
        "lw s4,  5  * 4(sp)\n"
        "lw s5,  6  * 4(sp)\n"
        "lw s6,  7  * 4(sp)\n"
        "lw s7,  8  * 4(sp)\n"
        "lw s8,  9  * 4(sp)\n"
        "lw s9,  10 * 4(sp)\n"
        "lw s10, 11 * 4(sp)\n"
        "lw s11, 12 * 4(sp)\n"
        "addi sp, sp, 13 * 4\n"
        "ret\n"
    );
}

extern char __kernel_base[];

// プロセスの初期化処理:
//      image: 実行イメージの先頭ポインタ
//      image_size: 実行イメージのサイズ
struct process *create_process(const void *image, size_t image_size) {
    // 空いているプロセス管理構造体を探す
    struct process *proc = NULL;
    int i;
    for (i = 0; i < PROCS_MAX; i++) {
        if (procs[i].state == PROC_UNUSED) {
            proc = &procs[i];
            break;
        }
    }

    if (!proc)
        PANIC("no free process slots");

    // switch_context() で復帰できるように、スタックに呼び出し先保存レジスタを積む
    uint32_t *sp = (uint32_t *) &proc->stack[sizeof(proc->stack)];
    *--sp = 0;                      // s11
    *--sp = 0;                      // s10
    *--sp = 0;                      // s9
    *--sp = 0;                      // s8
    *--sp = 0;                      // s7
    *--sp = 0;                      // s6
    *--sp = 0;                      // s5
    *--sp = 0;                      // s4
    *--sp = 0;                      // s3
    *--sp = 0;                      // s2
    *--sp = 0;                      // s1
    *--sp = 0;                      // s0
    *--sp = (uint32_t) user_entry;  // ra // user_entry 関数に復帰するように設定

    uint32_t *page_table = (uint32_t *) alloc_pages(1);

    // カーネルのプロセス用にページテーブルを設定
    // (カーネルでは仮想アドレス = 物理アドレス としてアクセスさせる)
    // ? 値は固定だが, 毎回ページテーブルを作成している.
    for (paddr_t paddr = (paddr_t) __kernel_base;
         paddr < (paddr_t) __free_ram_end; paddr += PAGE_SIZE)
        map_page(page_table, paddr, paddr, PAGE_R | PAGE_W | PAGE_X);

    // ユーザーのページをマッピングする
    // -> イメージサイズ分のページを確保して, イメージデータをコピーし, ページテーブルにマッピングする 
    for (uint32_t off = 0; off < image_size; off += PAGE_SIZE) {
        paddr_t page_paddr = alloc_pages(1);

        // コピーするデータがページサイズより小さい場合を考慮
        // https://github.com/nuta/operating-system-in-1000-lines/pull/27
        size_t remaining = image_size - off;
        size_t copy_size = PAGE_SIZE <= remaining ? PAGE_SIZE : remaining;

        // 確保したページにデータをコピー
        memcpy((void *) page_paddr, image + off, copy_size);

        // ページテーブルにマッピング
        map_page(page_table, USER_BASE + off, page_paddr,
                 PAGE_U | PAGE_R | PAGE_W | PAGE_X);
    }

    // 各フィールドを初期化
    proc->pid = i + 1;
    proc->state = PROC_RUNNABLE;
    proc->sp = (uint32_t) sp;
    proc->page_table = page_table;
    return proc;
}


// スケジューラの実装: 
//      次に実行可能なプロセスを探して実行する
struct process *current_proc; // 現在実行中のプロセス
struct process *idle_proc;    // アイドルプロセス = 実行可能なプロセスがない場合に実行されるプロセス (PID 0) (kernel_main で初期化)
void yield(void) {
    // 実行可能なプロセスを探す
    struct process *next = idle_proc;
    for (int i = 0; i < PROCS_MAX; i++) {
        struct process *proc = &procs[(current_proc->pid + i) % PROCS_MAX];
        if (proc->state == PROC_RUNNABLE && proc->pid > 0) {
            next = proc;
            break;
        }
    }

    // 現在実行中のプロセス以外に、実行可能なプロセスがない。 => 戻って処理を続行する
    if (next == current_proc)
        return;

    // プロセスを切り替える前に, 実行先プロセスの sp を sscratch に設定する
    __asm__ __volatile__(
        "sfence.vma\n" // TLB をクリアする命令 (ページテーブルを切り替える前に実行する)
        "csrw satp, %[satp]\n" // satp レジスタにページテーブルのアドレスをセット => ページングが有効化される
        "sfence.vma\n" // TLB をクリアする命令 (ページテーブルを切り替えた後に実行する)
        "csrw sscratch, %[sscratch]\n"
        :
        // 行末のカンマを忘れずに！
        : [satp] "r" (SATP_SV32 | ((uint32_t) next->page_table / PAGE_SIZE)),
          [sscratch] "r" ((uint32_t) &next->stack[sizeof(next->stack)])
    );

    // コンテキストスイッチ
    struct process *prev = current_proc;
    current_proc = next;
    switch_context(&prev->sp, &next->sp);
}

// プロセス検証用の遅延関数
void delay(void) {
    int DELAY_COUNT = 30000000;
    for (int i = 0; i < DELAY_COUNT; i++)
        __asm__ __volatile__("nop"); // 何もしない命令
}

struct process *proc_a;
struct process *proc_b;
void proc_a_entry(void) {
    printf("starting process A\n");
    while (1) {
        putchar('A');
        // switch_context(&proc_a->sp, &proc_b->sp); // 手動で プロセスA から B へスイッチ
        yield(); // スケジューラに制御を渡す
        delay();
    }
}
void proc_b_entry(void) {
    printf("starting process B\n");
    while (1) {
        putchar('B');
        // switch_context(&proc_b->sp, &proc_a->sp);
        yield(); // スケジューラに制御を渡す
        delay();
    }
}


/*
 * kernel_main: カーネルのメイン関数
 */
void kernel_main(void) {
    // BSS セクションをゼロクリア
    memset(__bss, 0, (size_t)__bss_end - (size_t)__bss);
    // 例外発生時のエントリポイントを設定
    WRITE_CSR(stvec, (uint32_t)kernel_entry);

    // アイドルプロセスを作成する
    idle_proc = create_process(NULL, 0);
    idle_proc->pid = 0;
    current_proc = idle_proc;

    // putchar, printf を使いコンソールに文字を出力
    const char *str = "\nHello, World!\n";
    for (const char *p = str; *p != '\0'; p++) {
        putchar(*p);
    }
    printf("1 + 2 = %d, %x\n", 1 + 2, 0x1234abcd);
    
    // alloc_pages のテスト
    paddr_t paddr0 = alloc_pages(2);
    paddr_t paddr1 = alloc_pages(1);
    printf("alloc_pages test: paddr0=0x%x, paddr1=0x%x\n", paddr0, paddr1);

    // プロセス (ユーザーアプリケーション) を作成して実行
    create_process(_binary_shell_bin_start, (size_t) _binary_shell_bin_size);
    yield(); // スケジューラを起動してプロセス A を開始
    
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