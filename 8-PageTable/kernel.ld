ENTRY(boot) /* エントリーポイントは boot 関数 */

SECTIONS {
    . = 0x80200000; /* ベースとなるメモリアドレス . は現在のアドレスを表す変数のようなもの */
    __kernel_base = .; /* カーネルが使用するページ領域 */

    /* 宣言した順に配置される */
    /* text : 関数コード領域 */
    .text :{
        KEEP(*(.text.boot)) /* .text.boot セクションが常に先頭 */
        *(.text .text.*) /* 全ファイルの中で、.text か .text. で始まるセクション */
    }

    /* rodata : 定数データ (Read only) */
    .rodata : ALIGN(4) {
        *(.rodata .rodata.*)
    }

    /* data : Read/Write 可能なデータ */
    .data : ALIGN(4) {
        *(.data .data.*)
    }

    /* bss : 初期値がゼロのR/W 可能データ */
    .bss : ALIGN(4) {
        __bss = .; /* プログラム上の __bss というシンボルに 現在のアドレスをセットする */
        *(.bss .bss.* .sbss .sbss.*)
        __bss_end = .;
    }

    . = ALIGN(4);
    . += 128 * 1024;
    __stack_top = .;  /* スタック領域を 128KB 確保 */

    . = ALIGN(4096);
    __free_ram = .;
    . += 64 * 1024 * 1024; /* ヒープ領域に 64MB 確保 */
    __free_ram_end = .;
}
