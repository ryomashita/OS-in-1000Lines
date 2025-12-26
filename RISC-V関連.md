# RISC-V メモ

RISC-V 関連の用語や命令についてまとめる.

##RISC-V 関連用語

M-mode: (Machine mode)
RISV-V の動作モードの1つ.
OpenSBI が動作するモード. (一番CPUに近いモード)

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

## 命令

mv rd, rs
    rd = rs; (move)
    addi rd, rs, 0 と等価なので, 実際には存在しない疑似命令.

sw rs, offset(base)
    *(base + offset) = rs; (store word)

sret
    S-mode の例外・割り込み処理から、元のコンテキストに復帰する命令.
    当然 S-mode でのみ実行可能な特権命令.

csrr rd, csr
    rd = csr; (CSR read)
csrw csr, rs
    csr = rs; (CSR write)

unimp
    未実装命令を意図的に呼び出す命令.
    例外 (Illegal Instruction) を発生させるために使用する.

特権命令:
`csrr rd, csr` : CSR の内容を rd レジスタに読み込む.
`csrw csr, rs` : rs レジスタの内容を CSR に書き込む.
`csrrw rd, csr, rs` : CSR の内容を rd レジスタに読み込み, rs レジスタの内容を CSR に書き込む.
`sret` : トラップハンドラからの復帰命令.
`sfence.vma` : TLB (Translation Lookaside Buffer) をクリアする.

## ページング機構

本書では RISV-V の Sv32 モードを利用している。

### Sv32 モード

- 2レベルのページテーブルを使用.
- 32bit 仮想アドレス空間を:
  - 10bit (ページテーブル1段目のインデックス) : VPN[1]
  - 10bit (ページテーブル2段目のインデックス) : VPN[0]
  - 12bit (ページ内オフセット) : page offset
  - に分割する.

### `satp` レジスタ: (Supervisor Address Translation and Protection)

S-mode で使われる CSR レジスタの1つ. フィールドは以下の３つ.
- ページング方式: Sv32/Sv39/Sv48 などの指定
- ASID: Address Space Identifier (アドレス空間識別子)
  - TLB を毎回クリアせず、ASID で TLB を切り替えることが可能.
- ページテーブルの物理アドレス


