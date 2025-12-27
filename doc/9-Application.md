# 9-Application

https://1000os.seiya.me/ja/12-application

ユーザーランド（カーネル以外）のプログラムをビルドして実行する方法を説明する.

## 12. アプリケーション

1. 必要なコードを実装し、`run.sh` を実行する.
    - ユーザープログラム `shell.c` がビルドされ, `shell.bin.o` というオブジェクトファイルが生成される.

2. `shell.bin.o` を調べてみる.

```
❯ llvm-nm shell.bin.o 
000102d0 D _binary_shell_bin_end    # D: BSS セクション, 実行イメージの終端を指すシンボル.
000102d0 A _binary_shell_bin_size   # A: 絶対アドレスシンボル(リンカ側で変更できない値)
00000000 D _binary_shell_bin_start
```

このオブジェクトファイルは以下のようにCコード側で参照できる.

```c
extern char _binary_shell_bin_start[];
extern char _binary_shell_bin_size[];

void load_user_program(void) {
    uint8_t *shell_bin_start = (uint8_t *)_binary_shell_bin_start;
    int32_t shell_bin_size = (int32_t)_binary_shell_bin_size;

    // char shell_bin_start[] からサイズ shell_bin_size バイト分がファイル内容を指している.
}
```

`shell.bin` のサイズと `shell_bin_size` シンボルの値が一致していることを確認する.

```
❯ ls -Bl shell.bin
.rwxr-xr-x 66256 yamaryo 27 Dec 17:11 shell.bin
```

0x102d0 = 66256 バイト.


3. `shell.elf` を調べて、リンカースクリプトの設定どおりの配置を確認する

```
❯ llvm-objdump -d shell.elf

shell.elf:      file format elf32-littleriscv

Disassembly of section .text:

01000000 <start>:
 1000000: 37 05 01 01   lui     a0, 4112
 1000004: 13 05 05 2d   addi    a0, a0, 720
 1000008: 2a 81         mv      sp, a0
 ...
```

=> `.text.start` セクションがアドレス `0x10000000` に配置されていることが分かる.

## 13. ユーザーモード

QEMU モニター画面上で `info mem` コマンドを実行して、メモリマップを確認する.
1. 
```
(qemu) info mem
vaddr    paddr            size     attr
-------- ---------------- -------- -------
01000000 0000000080269000 00001000 rwxu---
01001000 000000008026b000 00010000 rwxu---
80200000 0000000080200000 00001000 rwx--a-
...
```

=> ユーザープログラムを配置する仮想アドレス `0x10000000` が、物理アドレス `0x80269000` にマッピングされていることが分かる.

物理アドレス `0x80269000`  の中身を確認する
```
(qemu) xp /32b 0x80269000
0000000080269000: 0x37 0x05 0x01 0x01 0x13 0x05 0x05 0x2d
0000000080269008: 0x2a 0x81 0x97 0x00 0x00 0x00 0xe7 0x80
0000000080269010: 0x20 0x01 0x97 0x00 0x00 0x00 0xe7 0x80
0000000080269018: 0xe0 0x00 0x00 0x00 0x01 0xa0 0x00 0x00
```

=> `shell.elf` の先頭バイト列と一致していることが分かる.

```
hexdump -C shell.bin | head
00000000  37 05 01 01 13 05 05 2d  2a 81 97 00 00 00 e7 80  |7......-*.......|
00000010  20 01 97 00 00 00 e7 80  e0 00 00 00 01 a0 00 00  | ...............|
00000020  01 a0 82 80 01 ce aa 86  03 87 05 00 7d 16 85 05  |............}...|
00000030  93 87 16 00 23 80 e6 00  be 86 7d f6 82 80 09 ca  |....#.....}.....|
...
```

2. 逆アセンブルした結果も一致することを確認する.
```
(qemu) xp /8i 0x80269000
0x80269000:  01010537          lui             a0,16842752
0x80269004:  2d050513          addi            a0,a0,720
0x80269008:  812a              mv              sp,a0
0x8026900a:  00000097          auipc           ra,0            # 0x8026900a
0x8026900e:  012080e7          jalr            ra,ra,18
0x80269012:  00000097          auipc           ra,0            # 0x80269012
0x80269016:  00e080e7          jalr            ra,ra,14
0x8026901a:  0000              illegal
```

```
❯ llvm-objdump -d shell.elf | head -n20

shell.elf:      file format elf32-littleriscv

Disassembly of section .text:

01000000 <start>:
 1000000: 37 05 01 01   lui     a0, 4112
 1000004: 13 05 05 2d   addi    a0, a0, 720
 1000008: 2a 81         mv      sp, a0
 100000a: 97 00 00 00   auipc   ra, 0
 100000e: e7 80 20 01   jalr    18(ra)
 1000012: 97 00 00 00   auipc   ra, 0
 1000016: e7 80 e0 00   jalr    14(ra)
 100001a: 00 00         unimp

0100001c <main>:
 100001c: 01 a0         j       0x100001c <main>
 100001e: 00 00         unimp

01000020 <exit>:
```

3. U-Mode への復帰を実装後、再度動作を確認する.

レジスタを確認 => `0x100001c` (ユーザープログラム用の仮想アドレス) に PC が設定されていることが分かる.
```
(qemu) info registers
 pc       0100001c
```

4. ユーザープログラム内で不正なメモリアクセスを試み、ページフォルトが発生することを確認する.

`*((volatile int *) 0x80200000) = 0x1234;` のように、カーネル用のページ (ページテーブルエントリに U フラグがない) アドレスに書き込みを行うコードを追加し、
再度実行する.

```
PANIC: kernel.c:143: Unexpected trap: scause=0x0000000f, stval=0x80200000, user_pc=0x01000026
```

