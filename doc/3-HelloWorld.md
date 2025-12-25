# 03. Hello, World!

SBI の機能を呼び出し、デバッグコンソール上に文字を出力する.

## 操作

1. `3-HelloWorld` ディレクトリのコードを作成する.
   - kernel.c : カーネルのエントリポイント
   - common.c : 共通関数群 (putchar, printf)
2. `run-kernel.sh` スクリプトを実行し、QEMU 上でカーネルを動作させる.
   => デバッグコンソール上に "Hello, World!" と表示されれば成功.
