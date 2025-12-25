#!/bin/bash
# kernel.elf (kernel.c をコンパイルしたもの) を QEMU で起動するスクリプト

set -xue

QEMU="qemu-system-riscv32"

# Clang の実行パス
CC="clang"

# kernel.c をビルド
CFLAGS="-std=c11 -O2 -g3 -Wall -Wextra --target=riscv32-unknown-elf -fuse-ld=lld -fno-stack-protector -ffreestanding -nostdlib"
$CC $CFLAGS -Wl,-Tkernel.ld -Wl,-Map=kernel.map -o kernel.elf \
    kernel.c
# fuse-ld=lld : LLVM のリンカを使用
# -Wl,-Tkernel.ld : カスタムのリンカスクリプトを使用する (-Wl はオプションをリンカに渡すため)
# -Wl,-Map=kernel.map : リンカマップファイルを生成する


# QEMU で kernel.elf を起動
$QEMU -machine virt -bios default -nographic -serial mon:stdio --no-reboot -kernel kernel.elf
