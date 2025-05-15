# Windows環境でのビルド手順

oregexpライブラリとサンプルをWindows環境でビルドするには、以下の方法があります。

## WSL（Windows Subsystem for Linux）を使用する方法

WSLを使用すると、Windows上でLinux環境を実行できるため、Linux用のビルド手順をそのまま使用できます。

1. WSLをインストールする（Ubuntu推奨）
2. WSLターミナルを開く
3. 必要なパッケージをインストールする：
   ```
   sudo apt update
   sudo apt install build-essential make gcc
   ```
4. プロジェクトディレクトリに移動する：
   ```
   cd /mnt/d/data/github/oregexp  # パスは環境に合わせて調整
   ```
5. サンプルをビルドする：
   ```
   make samples
   ```

## MSYS2（MinGW）を使用する方法

MSYS2は、Windows上でUnixライクな開発環境を提供します。

1. [MSYS2公式サイト](https://www.msys2.org/)からインストーラをダウンロードしてインストールする
2. MSYS2端末（MSYS2 MinGW 64-bit）を開く
3. 必要なパッケージをインストールする：
   ```
   pacman -Syu
   pacman -S mingw-w64-x86_64-toolchain make
   ```
4. プロジェクトディレクトリに移動する
5. `Makefile`の先頭にある`CFLAGS`を以下のように修正する：
   ```
   CFLAGS=-g -Iostrutil -Wsign-compare -DTEST -D_DEBUG -DOSTRUTIL_EUC -DWIN32
   ```
6. サンプルをビルドする：
   ```
   make samples
   ```

## Visual Studio開発者コマンドプロンプトを使用する方法

Visual StudioのC/C++コンパイラ（cl.exe）を使用する場合：

1. Visual Studio開発者コマンドプロンプトを開く
2. プロジェクトディレクトリに移動する
3. 各サンプルを個別にビルドする：
   ```
   cl /I. /Iostrutil /DTEST /D_DEBUG /DOSTRUTIL_EUC /DWIN32 sample\basic_match.c ostrutil\ostrutil.lib
   cl /I. /Iostrutil /DTEST /D_DEBUG /DOSTRUTIL_EUC /DWIN32 sample\capture_groups.c ostrutil\ostrutil.lib
   cl /I. /Iostrutil /DTEST /D_DEBUG /DOSTRUTIL_EUC /DWIN32 sample\string_replace.c ostrutil\ostrutil.lib
   cl /I. /Iostrutil /DTEST /D_DEBUG /DOSTRUTIL_EUC /DWIN32 sample\callback_replace.c ostrutil\ostrutil.lib
   cl /I. /Iostrutil /DTEST /D_DEBUG /DOSTRUTIL_EUC /DWIN32 sample\japanese_text.c ostrutil\ostrutil.lib
   ```

注：Visual Studioを使用する場合は、ostrutil.libを事前にビルドする必要があります。
