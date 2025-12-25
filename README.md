# OS-in-1000Lines

https://github.com/nuta/operating-system-in-1000-lines


- CPU: 32bit RISC-V
- ハードウェア: QEMU virt

## Glossary

### QEMU

virt:
https://www.qemu.org/docs/master/system/riscv/virt.html
QEMU で用意されている、仮想的な汎用マシン.
最小限、標準的なハード構成が備わっている.
実在ボード (Rasspberry Pi) をエミュレートしたければ、virt ではなく対応するマシンを選択する必要がある.

### RISC-V 関連用語

M-mode: (Machine mode)
RISV-V の動作モードの1つ.
OpenSBI が動作するモード.

S-mode: (Supervisor mode)
RISV-V の動作モードの1つ.
OS カーネルが動作するモード.

U-mode: (User mode)
RISV-V の動作モードの1つ.
ユーザープログラムが動作するモード.

CSR: (Control and Status Register)
CPU の動作設定を格納するレジスタ.
特権命令でのみアクセス可能.

SBI: (Supervisor Binary Interface)
S-mode (OS カーネル) と M-mode (OpenSBI) 間のインターフェース仕様.
<-> U-mode と S-mode 間のインターフェースはシステムコール.
[SBI仕様](https://github.com/riscv-non-isa/riscv-sbi-doc/releases/tag/v3.0)
OpenSBI は SBI の実装例.


### OS 関連

マルチタスク:
複数のタスク (プロセス/スレッド) を同時に実行する仕組み.

例外ハンドラ:
CPU の例外 (割り込み/トラップ) 発生時に実行される関数.
CPU 単独では解決できないので, OS カーネルが処理を担当する.

ページング:
プロセスごとに独立した仮想アドレス空間を提供する仕組み.

システムコール:
ユーザープログラムが OS カーネルの機能を利用するためのインターフェース.

デバイスドライバ:
OS カーネルがハードウェアデバイスを制御するためのソフトウェア.

ファイルシステム:
ディスクなどのストレージデバイス上にデータを整理・管理する仕組み. 
(例: ext4, FAT32)

シェル:
利用者がコマンドを入力して OS に指示を出すためのインターフェース.

## RISV-V 命令

### 特権命令

`csrr rd, csr` : CSR の内容を rd レジスタに読み込む.
`csrw csr, rs` : rs レジスタの内容を CSR に書き込む.
`csrrw rd, csr, rs` : CSR の内容を rd レジスタに読み込み, rs レジスタの内容を CSR に書き込む.
`sret` : トラップハンドラからの復帰命令.
`sfence.vma` : TLB (Translation Lookaside Buffer) をクリアする.

### アセンブリ命令の書き方

gcc の独自拡張機能として、C コード内にアセンブリ命令を埋め込むことができる.(Inline Assembly)

```c
__asm__ __volatile__ ("asembly instructions" : <output operands> : <input operands> : <clobbered registers>);
```

- アセンブリ命令はダブルクォーテーションで囲む.
  - 複数の命令を書くときは改行 `\n` で区切る.
- 出力オペランド/入力オペランド: アセンブリの入出力を C の変数に対応付ける.
- 破壊されるレジスタ: アセンブリ命令で変更される可能性のあるレジスタを指定する.
  - これを指定したレジスタは、コンパイラがレジスタの内容を自動で保存・復元する.
- 

例1:
```c
uint32_t value;
__asm__ __volatile__("csrr %0, sepc" : "=r"(value));
```

- 意味: `csrr %0, sepc` : CSR `sepc` の内容を変数 `value` に読み込む.
- `%0`, `%1`, ... : 出力オペランド/入力オペランドの番号を表すプレースホルダ.
  - ここでは `value` を指す.
- 出力オペランド: `=r` : 汎用レジスタに格納することを意味する.
  - `=` : アセンブリで変更される.
  - `r` : いずれかの汎用レジスタを使う.
  - `(value)` は変数 value に結果を格納

例2:
```c
__asm__ __volatile__("csrw sscratch, %0" : : "r"(123));
```

- 意味: `csrw sscratch, %0` : CSR `sscratch` に値 `123` を書き込む.

このアセンブリは以下に展開される:
```asm
li a0, 123         # 汎用レジスタ a0 に即値 123 を読み込む
csrw sscratch, a0  # CSR sscratch に a0 の内容を書き込む
```


