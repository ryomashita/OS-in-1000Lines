# RISC-V メモ

RISC-V 関連の用語や命令についてまとめる.

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
