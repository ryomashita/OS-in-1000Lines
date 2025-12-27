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

ユーザーランド:
OS に含まれる要素の中で、カーネル以外の要素.
(例: ユーザーが作成したプログラム, シェル, カーネル権限を必要としないライブラリなど)

### 仮想化関連

virtio:
準仮想化を実現するためのフレームワーク.
仮想マシンとホストOS間でデータをやり取りするための仕組み.
https://zenn.dev/junjunjunjun/articles/27ede76931cc85

セクタ:
ブロックデバイスの最小単位のデータサイズ.
通常 512 バイトまたは 4096 バイト.

## アセンブリ命令の書き方

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

## Virtio 入門

https://1000os.seiya.me/ja/15-virtio-blk#virtio入門
virtio Specification: https://docs.oasis-open.org/virtio/virtio/v1.1/csprd01/virtio-v1.1-csprd01.html

virtio はゲストOSとハイパーバイザ間のデバイスIOを実現するための標準フレームワーク.

### virtqueue

virtio デバイス (virtio で仮想化されたデバイス) は、ホストOS/ゲストOSで共有するリングバッファ (virtqueue) を介してデータをやり取りする.
virtqueue は以下の3つの部分で構成される:
- Discriptor Chain: 処理要求を表す単方向リスト.
  - 複数のディスクリプタを一度に要求できるので、飛び飛びのメモリアクセスや異なる属性もまとめて処理できる.
- Available Ring: ゲストOSのドライバの処理要求を格納する
- Used Ring: ホストOSのドライバの処理完了を格納する

1. ゲストOSのドライバ: Descriptor Chain に処理要求を追加し、Available Ring にインデックスを書き込む.
2. ホストOSのドライバ: Available Ring から処理要求を取得し、処理を実行. 処理が完了したら Used Ring にインデックスを書き込む.
3. ゲストOSのドライバ: Used Ring から処理完了を取得し、結果を処理する.
