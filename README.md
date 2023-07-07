# DIY-Keypad
## Overview

汎用的なマイコンボードを自作するプロジェクトです。

![diykeypad1](https://github.com/nicotakuya/DIY-Keypad/assets/5597377/bb161106-6f67-42ac-91a7-05f0ece821bf)

## Files

・8x8font.h : Font file.

・diykeypad.ino : Source file(for Arduino).

・diykeypad_sch.png : Schematic.

・gattai_b_h4tan3.stl : AA Battery mount(for 3D printer).

・gattai_b_h4tan4.stl : AAA Battery mount(for 3D printer).

・pico_cover.stl : Raspberry Pi Pico cover(for 3D printer).

## Parts

部品は全て秋月電子通商で入手可能です。

・Raspberry Pi Pico/W。

・ディスプレイ。AE-ATM0177B3A / AE-ATM0130B3 / SSD1306のうち、いずれか1つが必要です。

・タクトスイッチ x 16。

・スライドスイッチ。

・ユニバーサル基板。

・電池ボックス。

## How to Build

開発環境はArduino IDE。ボード設定はArduino-Pico(Earle F.philhower氏開発)を使用しています。

ソースコードの「DISP_TYPE」を変更すると、対応するディスプレイを変えることができます。

「DISP_ROTALE」を変更すると、90度単位で表示内容を回転させることができます。

「USE_xxxx」の値を変更することでハードウェアの拡張が可能です。

![diykeypad2](https://github.com/nicotakuya/DIY-Keypad/assets/5597377/a044840a-88f4-4b52-9e44-504904a59823)

![diykeypad3](https://github.com/nicotakuya/DIY-Keypad/assets/5597377/9c18563f-ab75-4c61-ac9c-52dfe676f512)
