# DIY-Keypad
## Overview

Raspberry Pi Picoを使ったマイコンボードです。

## Files

・8x8font.h : Font file.

・diykeypad.ino : Source file(for Arduino).

・diykeypad_sch.png : Schematic.

・gattai_b_h4tan3.stl : AA Battery mount(for 3D printer).

・gattai_b_h4tan4.stl : AAA Battery mount(for 3D printer).

・pico_cover.stl : Raspberry Pi Pico cover(for 3D printer).

## Parts

・Raspberry Pi Pico/W。

・ディスプレイ。AE-ATM0177B3A / AE-ATM0130B3 / SSD1306のいずれか1つが必要です。

・タクトスイッチ x 16。

・スライドスイッチ。

・ユニバーサル基板。

・電池ボックス。

## How to Build

開発環境はArduino IDE。ボード設定はArduino-Pico(Earle F.philhower氏開発)を使用しています。

ソースコードの「DISP_TYPE」「DISP_ROTALE」を修正します。

「USE_xxxx」の値を変更することでハードウェアの拡張が可能です。
