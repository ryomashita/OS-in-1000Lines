# 5-ExceptionHandling

1. ソースを実装し, `run-kernel.sh` を実行します.
    => 例外が発生し、`PANIC` メッセージが表示されます.
2. QEMU コンソール上で `spec` を確認.
```
sepc     8020016a
```

3. `llvm-addr2line-14` コマンドで、`0x8020016a` の命令がどのソースコードに対応するか調べます.

```
❯ llvm-addr2line-14 -e kernel.elf 8020016a
/home/yamaryo/ws/OS-in-1000Lines/5-ExceptionHandling/kernel.c:150
```

=> `kernel.c` の 150 行目で例外が発生していることが分かります.

4. ソースコードを確認すると, 当該箇所が `unimp` 命令の呼び出し行であることを確認できる.
