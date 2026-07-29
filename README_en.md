# yhbos-calendar

**Version 1.0**

A Qt6-based desktop calendar application with dual Gregorian/Lunar calendar display.

## Features

- Dual Gregorian/Lunar calendar display
- Month/Year navigation, date jump (YYYY-MM-DD)
- 24 Solar terms (from liblunar-date)
- Traditional Chinese lunar festivals (Spring Festival, Lantern Festival, Dragon Boat, Mid-Autumn, etc.)
- Solar holidays from system locale (via KF6Holidays)
- Multi-language UI (中文 / English / Русский) — switchable from Settings menu
- Info bar: lunar date, Bazi, zodiac, constellation, holidays

## Dependencies

- Qt6 (Widgets)
- liblunar-date (>= 3.0)
- glib-2.0 / gobject-2.0
- KF6Holidays
- CMake (>= 3.16)
- C++17 compiler

## Build

```bash
mkdir build && cd build
cmake ..
make
```

Or generate a Debian package:

```bash
cmake -S . -B build && cmake --build build && cd build && cpack -G DEB
```

## Run

```bash
./build/yhbos-calendar
```

## Author

wiwyil2tr &lt;wiwyil2tr@ya.ru&gt;

## License

GNU General Public License v2 or later.
