# 目标文件里面有什么

* 使用 `riscv64-linux-gnu-gcc` 编译 `simple_section.c` 获得 `simple_section.o` 文件。
* 使用 `objdump -h simple_section.o` 查看 ELF 文件的各个段的基本信息。

> [!NOTE]
> 比较 `diff <(objdump -h simple_section.o) <(riscv64-linux-gnu-objdump -h simple_section.o)`
>

* 使用 `riscv64-linux-gnu-objdump -sd simple_section.o`
* 使用 `riscv64-linux-gnu-readelf -h simple_section.o` 查看 ELF 文件头部
* 使用 `riscv64-linux-gnu-readelf -S simple_section.o` 查看 ELF 段表
* 使用 `riscv64-linux-gnu-nm simple_section.o` 查看符号
* 使用 `riscv64-linux-gnu-readelf -s simple_section.o` 查看 ELF 符号表
