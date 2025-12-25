#!/bin/bash
# 最小設定で QEMU を起動するスクリプト

set -xue
# x : コマンド実行を表示
# u : 未定義変数の使用をエラーにする
# e : コマンドがエラーを返したらスクリプトを終了

# QEMU バイナリのパス
QEMU="qemu-system-riscv32"

# QEMU virt マシンを起動
$QEMU -machine virt -bios default -nographic -serial mon:stdio --no-reboot

# -machine virt : 仮想マシンのタイプを指定
# -bios default : デフォルトのBIOS (OpenSBI) を使用
# -nographic : ウィンドウ無しで起動
# -serial mon:stdio : 仮想マシンのシリアル出力を、QEMU の標準入出力に接続
# --no-reboot : 仮想マシンがクラッシュやシャットダウンしたとき、停止させる (デバッグ時に停止箇所を確認しやすい)
