# YALR

[![GitHub license](https://img.shields.io/github/license/duzhaokun123/YALR?style=flat-square)](https://github.com/duzhaokun123/YALR/blob/main/LICENSE)
![Windhawk Mod](https://img.shields.io/badge/Windhawk-Mod-1e1e1e?style=flat-square)


Yet Another [Locale_Remulator](https://github.com/InWILL/Locale_Remulator)

本 Mod 未在(也不会在) Windhawk Mod 仓库发布

> [!TIP]
> 
> YALR 是对 [Locale_Remulator](https://github.com/InWILL/Locale_Remulator) 的重写
> 
> 而 Locale_Remulator 是对 [Locale_Emulator](https://github.com/xupefei/Locale-Emulator) 的重写
> 
> 而 Locale_Emulator 又是 **Yet Another** System Region and Language Simulator



## 功能

- [x] 自动注入
- [ ] 右键菜单
- [x] 语言区域修改
- [ ] 时区修改
- [ ] 编码转换(乱码修复)

## 安装

1. 安装 [Windhawk](https://windhawk.net/) 这是一个适用于 Windows x86/x64 的全局注入和 Hook 框架
2. 启用`Windhawk`的`开发着选项`
3. 新建 Mod
4. 如原样复制[yalr.cpp](yalr.cpp)并替换**全部**代码
5. 点击`Compile Mod`编译 Mod
6. 点击`Exit Editing Mode`退出编辑模式
7. 使用

## 配置

`目标`输入进程名或完整路径 (其实这东西有问题 是按字符串结尾匹配的 也就是说 bar.exe 会匹配到 foo-bar.exe)

`LCID`输入要修改成的 LCID `0`不修改 常见语言与`LCID`对照见[下表](#LCID)

`CodePage`输入要修改成的 CodePage `0`不修改

## LCID

| LCID | 语言 |
| --- | --- |
| 1033 | en |
| 2052 | zh-CN |
| 1028 | zh-TW |
| 1041 | ja |

## 问题

- 对明显的全局注入感到不适
  - 在`高级设置`中启用`Ignore mod inclusion/exclusion lists`并自行修改`自定义进程包含列表`