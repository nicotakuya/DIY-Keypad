# DIY Keypad
![diykeypad1](https://github.com/nicotakuya/DIY-Keypad/assets/5597377/bb161106-6f67-42ac-91a7-05f0ece821bf)

## Overview
汎用的なマイコンボードを自作するプロジェクトです。

## Files

・8x8font.h : Font file. https://github.com/nicotakuya/8pixelfont

・diykeypad.ino : Source file(for Arduino-Pico).

・diykeypad_sch.png : Schematic.

・gattai_b_h4tan3.stl : AA Battery x2 mount(for 3D printer).

・gattai_b_h4tan4.stl : AAA Battery x2 mount(for 3D printer).

・pico_cover.stl : Raspberry Pi Pico cover(for 3D printer).

## Parts

部品は全て秋月電子通商で入手可能です。

・Raspberry Pi Pico/W。

・ディスプレイ。AE-ATM0177B3A / AE-ATM0130B3 / SSD1306のうち、いずれか1つが必要です。

・タクトスイッチ x 16個。

・スライドスイッチ。

・Bタイプ ユニバーサル基板。

・電池ボックス。

## How to Build

開発環境はArduino IDE。ボード設定はArduino-Pico(Earle F.philhower氏開発)を使用しています。

ソースコードの「DISP_TYPE」の値を変更すると、表示するディスプレイを変えることができます。

「DISP_ROTALE」を変更すると、90度単位で表示内容を回転させることができます。

「USE_xxxx」の値を変更することでハードウェアの拡張が可能です。

## How to use

テンキーのレイアウトは以下の通り。

【7】【8】【9】【-】

【4】【5】【6】【-】

【1】【2】【3】【-】

【0】【-】【-】【ENTER】

メニューを選択は「4」「6」または「2」「8」キー。決定は「ENTER」キーです。決定すると次のデモを行います。

・"LAND"：3Dっぽい表示デモ。

・"CHARA"：文字を表示する。

・"LINE"：ラインの描画。

・"RTC"：RTCから日付と時間を読み取って表示。

・"AUDIO"：スピーカーからサウンド出力。

・"MOTOR"：振動モーターを動かす。

・"FIGHT"：スペースインベーダーっぽい表示デモ。

・"VECTOL"：ベクトル表示デモ。

・"LIFE"：ライフゲーム。

・"BALL"：ボールが画面を反射する。

・"VMETER"：グラフ表示のデモ。

![diykeypad2](https://github.com/nicotakuya/DIY-Keypad/assets/5597377/a044840a-88f4-4b52-9e44-504904a59823)

![diykeypad3](https://github.com/nicotakuya/DIY-Keypad/assets/5597377/9c18563f-ab75-4c61-ac9c-52dfe676f512)

![screenshot](https://github.com/nicotakuya/DIY-Keypad/assets/5597377/e5f63d3f-7e52-49a0-a79a-b5239607ec0b)
