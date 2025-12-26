# 01. Setup

See https://operating-system-in-1000-lines.vercel.app/ja/01-setting-up-development-environment

## Setting up the Development Environment

### 1. Install the required packages:

```bash
sudo apt update && sudo apt install -y clang llvm lld qemu-system-riscv32 curl
```

### 2. Install OpenSBI

```bash
cd <your-working-directory>
curl -LO https://github.com/qemu/qemu/raw/v8.0.4/pc-bios/opensbi-riscv32-generic-fw_dynamic.bin
```

[OpenSBI](https://github.com/riscv-software-src/opensbi)
RISC-V Open Source Supervisor Binary Interface (OpenSBI)
RISC-V CPU における SEE (Supervisor Execution Environment) と OS カーネルの間のインターフェース仕様がSBI.
OpenSBI はこの SBI の実装を提供するプロジェクト.

- [RISC-VのOpenSBIでHelloWorld](https://zenn.dev/tetsu_koba/articles/d0a25ea5035b3e)
- [OpenSBI Deep Dive](https://riscv.org/wp-content/uploads/2024/12/13.30-RISCV_OpenSBI_Deep_Dive_v5.pdf)

