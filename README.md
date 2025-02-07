# 终端代码页修改工具

> `windows`自带的`chcp`执行后总是刷屏，所以写了个不用刷屏的小工具来修改当前终端代码页。

```
用法：
  codepage [选项] [参数]

选项：
  get            获取当前终端代码页
  set [数字]     设置当前终端代码页
  -v, --version  显示版本信息
  -h, --help     显示帮助信息
  -q, --quiet    静默模式，不输出控制台信息
```

## 使用

```cmd
📁 ~\Desktop
❯ codepage set 936
成功：设置代码页为：936

📁 ~\Desktop
❯ codepage set 936
未修改：当前代码已设为：936

📁 ~\Desktop
❯ codepage get
当前终端代码页：936
```

## 编译

使用`mingw`+`cmake`+`ninja`进行编译

[👉点击下载`codepage.exe`](codepage.exe "✨鼠标右键另存为")