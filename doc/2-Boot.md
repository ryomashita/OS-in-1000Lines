# 02. Boot

https://operating-system-in-1000-lines.vercel.app/ja/04-boot

## OpenSBI の起動

まずは QEMU virt を起動してみる.

1. `run-base.sh` スクリプトを実行する
    => QEMU が起動し, OpenSBI のログが表示される
2. `Ctrl+a` を押してから `c` を押す
   => QEMU のモニタ画面に切り替わる
3. QEMU モニタ上で `q` と入力
   => QEMU が終了する

## 最小カーネルの起動

1. `kernel.ld` と `kernel.c` および `run-kernel.sh` を作成する
2. `run-kernel.sh` スクリプトを実行する
    => QEMU が起動し, OpenSBI のログが表示される
3. QEMU モニタ画面に切り替える (`Ctrl+a` から `c`)
   - `stop` で実行停止, `cont` で再開.

- QEMU モニタ画面上で `info registers` と入力する
    => `pc` レジスタ = 現在実行している命令アドレス、を確認する
- `llvm-objdump -d kernel.elf` コマンドで `kernel.elf` の逆アセンブルを表示し, `pc` レジスタのアドレスに対応する命令を確認する
    => `kernel_main` 関数のループに到達していることが確認できる
- `kernel.map` ファイルを確認し, セクションの配置を確認する
    => スタック領域 (__stack_top) や BSS セクション (__bss, __bss_end) のアドレスを確認できる
- `llvm-nm kernel.elf` コマンドでシンボル一覧を表示し, シンボルのアドレスを確認する

### nm の実行結果

```plaintext
00000000 N .Lline_table_start0  # N はデバッグシンボル
80200054 B __bss                # B は BSS セクション
80200054 B __bss_end
80220054 B __stack_top
80200000 T boot                 # T はテキストセクション (コード)
80200024 T kernel_main
80200010 T memset
```
