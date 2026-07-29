# yhbos-calendar

**版本 1.0**

基于 Qt6 的桌面日历应用，支持公历与农历双日历显示。

## 功能

- 公历/农历双日历显示
- 月份/年份切换，日期跳转（YYYY-MM-DD）
- 24 节气标注（来自 liblunar-date）
- 农历传统节日标注（春节、元宵、端午、中秋等）
- 阳历节日跟随系统 locale（通过 KF6Holidays）
- 多语言界面（中文 / English / Русский）—— 在设置菜单中切换
- 底部信息栏显示农历月日、八字、生肖、星座、节日

## 依赖

- Qt6 (Widgets)
- liblunar-date (>= 3.0)
- glib-2.0 / gobject-2.0
- KF6Holidays
- CMake (>= 3.16)
- C++17 编译器

## 构建

```bash
mkdir build && cd build
cmake ..
make
```

生成 Debian 包：

```bash
cmake -S . -B build && cmake --build build && cd build && cpack -G DEB
```

## 运行

```bash
./build/yhbos-calendar
```

## 作者

wiwyil2tr &lt;wiwyil2tr@ya.ru&gt;

## 许可

GNU General Public License v2 或更高版本。
