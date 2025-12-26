# 6-MemoryAllocation

https://1000os.seiya.me/ja/09-memory-allocation

動的なメモリ割り当てを実装する.
ページ単位 (4KB) でメモリ割り当てを可能にする.

1. 実装を追加する

2. カーネルを実行すると、割り当てられたページの物理アドレスが表示される.

```
alloc_pages test: paddr0=0x80221000, paddr1=0x80223000
```

3. `nm` コマンドで、ヒープ領域 (`__free_ram` から `__free_ram_end` まで) が返されていることを確認できる.

```
❯ llvm-nm kernel.elf | grep "free"
80221000 B __free_ram
84221000 B __free_ram_end
```
