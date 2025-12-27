#!/bin/bash
# kernel.elf (kernel.c をコンパイルしたもの) を QEMU で起動するスクリプト

set -xue

QEMU="qemu-system-riscv32"
OBJCOPY="llvm-objcopy"

# Clang の実行パス
CC="clang"
CFLAGS="-std=c11 -O2 -g3 -Wall -Wextra --target=riscv32-unknown-elf -fuse-ld=lld -fno-stack-protector -ffreestanding -nostdlib"

# ユーザープログラムをビルド
$CC $CFLAGS -Wl,-Tuser.ld -Wl,-Map=shell.map -o shell.elf shell.c user.c common.c
# shell.elf を raw binary (実際にメモリにロードされる形式) に変換し
# alloc=メモリ上に配置する, contents=内容を含む => 本来 raw binary に含まれない BSS セクションを出力に含める
$OBJCOPY --set-section-flags .bss=alloc,contents -O binary shell.elf shell.bin
# raw binary を C言語のオブジェクトファイルに変換
$OBJCOPY -Ibinary -Oelf32-littleriscv shell.bin shell.bin.o

# kernel.c をビルド
$CC $CFLAGS -Wl,-Tkernel.ld -Wl,-Map=kernel.map -o kernel.elf \
    kernel.c common.c shell.bin.o
# fuse-ld=lld : LLVM のリンカを使用
# -Wl,-Tkernel.ld : カスタムのリンカスクリプトを使用する (-Wl はオプションをリンカに渡すため)
# -Wl,-Map=kernel.map : リンカマップファイルを生成する


# QEMU で kernel.elf を起動
$QEMU -machine virt -bios default -nographic -serial mon:stdio --no-reboot -kernel kernel.elf
