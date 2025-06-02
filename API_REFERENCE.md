# oregexp API リファレンス

このドキュメントはoregexp正規表現ライブラリの公開APIについて説明します。oregexpは日本語（EUC-JP、UTF-8）をサポートした正規表現エンジンで、Perl5互換の正規表現構文を提供します。

## 目次

- [定数](#定数)
- [データ型](#データ型)
- [コンパイル関数](#コンパイル関数)
- [実行関数](#実行関数)
- [マッチ情報取得関数](#マッチ情報取得関数)
- [置換関数](#置換関数)
- [ユーティリティ関数](#ユーティリティ関数)
- [日本語処理](#日本語処理)
- [使用例](#使用例)
- [実装ノート](#実装ノート)
- [高度な使用法](#高度な使用法)

## 定数

### 戻り値の定数

```c
#define OREGEXP_FOUND                    1   // パターンマッチが見つかった
#define OREGEXP_NOT_FOUND                0   // パターンマッチが見つからなかった
#define OREGEXP_COMP_ERR                -1   // コンパイルエラー（パターン構文エラーなど）
#define OREGEXP_STACK_OVERFLOW          -2   // スタックオーバーフロー（複雑な正規表現で発生する可能性あり）
#define OREGEXP_FATAL_ERROR            -10   // 致命的なエラー
#define OREGEXP_REPLACE_OK               1   // 置換成功
#define OREGEXP_REPLACE_BUF_SIZE_ERR    -4   // 置換バッファサイズエラー（バッファが小さすぎる場合）
```

### オプションフラグ

```c
#define OREGEXP_OPT_IGNORE_CASE         (0x01 << 0)  // 大文字小文字を区別しない（例：a と A をマッチ）
#define OREGEXP_OPT_IGNORE_WIDTH_ASCII  (0x01 << 1)  // ASCII文字の全角/半角を区別しない（例：A と Ａ をマッチ）
```

## データ型

### HREG_DATA

正規表現パターンをコンパイルした結果を保持するハンドル。NFA（非決定性有限オートマトン）の状態やバックリファレンス情報などが含まれています。

```c
typedef struct oreg_data_st *HREG_DATA;
```

### OREG_POINT

テキスト内の位置を表す構造体。2次元（行・列）の位置情報を保持します。

```c
typedef struct oreg_data_point_st OREG_POINT;
struct oreg_data_point_st {
    INT_PTR     row;  // 行位置（0ベース）
    INT_PTR     col;  // 列位置（0ベース）
};
```

### HREG_DATASRC / OREG_DATASRC

テキストデータソースを表す構造体。様々な形式のテキストデータへのアクセスを抽象化します。
文字列、ファイル、メモリバッファなど異なる種類のデータソースに対して統一的なインターフェースを提供します。

この仕組みにより、oregexpは非連続のメモリ領域でテキストを管理するシステムでも効率的に動作できます。
例えば、テキストエディタでよく使用されるgapped buffer、piece table、rope構造などの非連続メモリ構造でも、
適切な関数ポインタを実装することで、連続領域にテキストをコピーすることなく正規表現検索を実行できます。

```c
typedef struct oreg_datasrc_st *HREG_DATASRC;
typedef struct oreg_datasrc_st OREG_DATASRC;
struct oreg_datasrc_st {
    const void *src;  // 実際のデータへのポインタ
    const TCHAR *(*get_char_buf)(const void *src, OREG_POINT *pt);  // 指定位置の文字を取得
    const TCHAR *(*get_row_buf_len)(const void *src, INT_PTR row, size_t *len);  // 行バッファと長さを取得
    const TCHAR *(*next_char)(const void *src, OREG_POINT *pt);  // 次の文字に進む
    const TCHAR *(*prev_char)(const void *src, OREG_POINT *pt);  // 前の文字に戻る
    const TCHAR *(*next_row)(const void *src, OREG_POINT *pt);   // 次の行に進む
    INT_PTR (*get_len)(const void *src, OREG_POINT *start_pt, OREG_POINT *end_pt);  // 2点間の長さを取得
    
    // データソース中の位置判定用関数ポインタ
    int (*is_row_first)(const void *src, OREG_POINT *pt);  // 行の先頭か判定
    int (*is_selected_first)(const void *src, OREG_POINT *pt);  // 選択範囲の先頭か判定
    int (*is_selected_last)(const void *src, OREG_POINT *pt);   // 選択範囲の末尾か判定
    int (*is_selected_row_first)(const void *src, OREG_POINT *pt);  // 選択範囲の行の先頭か判定
    int (*is_selected_row_last)(const void *src, OREG_POINT *pt);   // 選択範囲の行の末尾か判定
};
```

## コンパイル関数

### oreg_comp / oreg_comp2

正規表現パターンをコンパイルします。Perl5互換の正規表現構文をサポートしています。

```c
HREG_DATA oreg_comp(const TCHAR *pattern, int i_flg);
HREG_DATA oreg_comp2(const TCHAR *pattern, int regexp_opt);
```

**パラメータ:**
- `pattern`: コンパイルする正規表現パターン（Perl5互換の正規表現構文）
- `i_flg`: 大文字小文字を区別しない場合は1、それ以外は0（`oreg_comp`のみ）
- `regexp_opt`: 正規表現オプションフラグの組み合わせ（OREGEXP_OPT_*）

**戻り値:**
- 成功時: コンパイル結果のハンドル（HREG_DATA）
- 失敗時: NULL（パターンの構文エラーや、メモリ確保失敗時など）

### oreg_comp_str / oreg_comp_str2

文字列検索用にパターンをコンパイルします。正規表現メタ文字を使わない単純な文字列マッチングに最適化されています。
Boyer-Mooreアルゴリズムを内部で使用するため、通常の正規表現エンジンより高速です。

```c
HREG_DATA oreg_comp_str(const TCHAR *pattern, int i_flg);
HREG_DATA oreg_comp_str2(const TCHAR *pattern, int regexp_opt);
```

**パラメータ:**
- `pattern`: 検索する文字列パターン（正規表現メタ文字は使用不可）
- `i_flg`: 大文字小文字を区別しない場合は1、それ以外は0（`oreg_comp_str`のみ）
- `regexp_opt`: 正規表現オプションフラグの組み合わせ（OREGEXP_OPT_*）

**戻り値:**
- 成功時: コンパイル結果のハンドル（HREG_DATA）
- 失敗時: NULL（メモリ確保失敗時など）

## 実行関数

### oreg_exec / oreg_exec2

コンパイルされた正規表現パターンでマッチングを実行します。検索範囲と結果を詳細に制御できる汎用インターフェースです。

```c
int oreg_exec(HREG_DATA reg_data, HREG_DATASRC data_src,
    OREG_POINT *search_start, OREG_POINT *search_end,
    OREG_POINT *result_start, OREG_POINT *result_end, int loop_flg);
int oreg_exec2(HREG_DATA reg_data, HREG_DATASRC data_src);
```

**パラメータ:**
- `reg_data`: コンパイルされた正規表現ハンドル
- `data_src`: 検索対象データソース（`oreg_make_str_datasrc`などで準備）
- `search_start`: 検索開始位置（`oreg_exec`のみ）
- `search_end`: 検索終了位置（`oreg_exec`のみ、{-1, -1}を指定すると文字列の終端まで）
- `result_start`: マッチング開始位置を受け取る変数へのポインタ（`oreg_exec`のみ）
- `result_end`: マッチング終了位置を受け取る変数へのポインタ（`oreg_exec`のみ）
- `loop_flg`: 繰り返し検索フラグ（1:全体を検索、0:指定範囲のみ）（`oreg_exec`のみ）

**戻り値:**
- `OREGEXP_FOUND`: マッチングが見つかった
- `OREGEXP_NOT_FOUND`: マッチングが見つからなかった
- `OREGEXP_COMP_ERR`: コンパイルエラー
- `OREGEXP_STACK_OVERFLOW`: スタックオーバーフロー
- `OREGEXP_FATAL_ERROR`: 致命的なエラー

### oreg_exec_str / oreg_exec_str2

単純な文字列に対して正規表現マッチングを実行します。`oreg_exec`のシンプルなラッパーで、文字列のみを扱う場合に便利です。

```c
int oreg_exec_str(HREG_DATA reg_data, const TCHAR *str, INT_PTR search_start_col, 
                 INT_PTR *result_start_col, INT_PTR *len);
int oreg_exec_str2(HREG_DATA reg_data, const TCHAR *str);
```

**パラメータ:**
- `reg_data`: コンパイルされた正規表現ハンドル
- `str`: 検索対象文字列
- `search_start_col`: 検索開始列（文字位置、0ベース）（`oreg_exec_str`のみ）
- `result_start_col`: マッチング開始列を受け取る変数へのポインタ（`oreg_exec_str`のみ）
- `len`: マッチング長を受け取る変数へのポインタ（`oreg_exec_str`のみ）

**戻り値:**
- `OREGEXP_FOUND`: マッチングが見つかった
- `OREGEXP_NOT_FOUND`: マッチングが見つからなかった
- `OREGEXP_COMP_ERR`: コンパイルエラー
- `OREGEXP_STACK_OVERFLOW`: スタックオーバーフロー
- `OREGEXP_FATAL_ERROR`: 致命的なエラー

## マッチ情報取得関数

### oreg_get_match_data

マッチングしたテキストの位置情報を取得します。`oreg_exec`または`oreg_exec_str`で正規表現検索が成功した後に呼び出します。

```c
void oreg_get_match_data(HREG_DATA reg_data,
    OREG_POINT *pstart_pt, OREG_POINT *pend_pt);
```

**パラメータ:**
- `reg_data`: コンパイルされた正規表現ハンドル
- `pstart_pt`: マッチ開始位置を格納するポインタ
- `pend_pt`: マッチ終了位置を格納するポインタ

### oreg_get_backref_cnt

バックリファレンス（キャプチャグループ）の数を取得します。正規表現中の`()`の数を返します。

```c
int oreg_get_backref_cnt(HREG_DATA reg_data);
```

**パラメータ:**
- `reg_data`: コンパイルされた正規表現ハンドル

**戻り値:**
- バックリファレンス（キャプチャグループ）の数

### oreg_get_backref_data

指定したインデックスのバックリファレンス（キャプチャグループ）の位置情報を取得します。
インデックスは1から始まります（Perl互換）。

```c
void oreg_get_backref_data(HREG_DATA reg_data, int idx,
    OREG_POINT *pstart_pt, OREG_POINT *pend_pt);
```

**パラメータ:**
- `reg_data`: コンパイルされた正規表現ハンドル
- `idx`: キャプチャグループのインデックス（1から始まる）
- `pstart_pt`: キャプチャグループの開始位置を格納するポインタ
- `pend_pt`: キャプチャグループの終了位置を格納するポインタ

### oreg_get_backref_str

指定したインデックスのバックリファレンス（キャプチャグループ）の文字列を取得します。
インデックスは1から始まります（Perl互換）。

```c
INT_PTR oreg_get_backref_str(HREG_DATASRC data_src, HREG_DATA reg_data, int idx, TCHAR *buf);
```

**パラメータ:**
- `data_src`: データソース
- `reg_data`: コンパイルされた正規表現ハンドル
- `idx`: キャプチャグループのインデックス（1から始まる）
- `buf`: 結果を格納するバッファ

**戻り値:**
- キャプチャグループの文字列長

## 置換関数

### oreg_make_replace_str

マッチした文字列を置換パターンに基づいて置換します。置換パターンではバックリファレンス（\1, \2など）や特殊シーケンス（&, \`, \'）が使用できます。

```c
INT_PTR oreg_make_replace_str(HREG_DATASRC data_src,
    OREG_POINT *start_pt, OREG_POINT *end_pt,
    const TCHAR *rep_str, HREG_DATA reg_data, TCHAR *result);
```

**パラメータ:**
- `data_src`: データソース
- `start_pt`: マッチ開始位置
- `end_pt`: マッチ終了位置
- `rep_str`: 置換パターン
- `reg_data`: コンパイルされた正規表現ハンドル
- `result`: 結果を格納するバッファ（NULLの場合、必要なバッファサイズを返す）

**戻り値:**
- 置換後の文字列長（NUL終端を含む）

**置換パターン特殊文字:**
- `&`: マッチした全体の文字列
- `\n`: n番目のキャプチャグループの内容（例：`\1`）
- <code>\`</code>: マッチした位置より前の文字列
- `\'`: マッチした位置より後の文字列
- `\+`: 最後にマッチしたキャプチャグループ

### oreg_replace_str

正規表現パターンにマッチした部分を置換します。内部で`oreg_make_replace_str`を使用しています。

```c
TCHAR *oreg_replace_str(HREG_DATA reg_data, const TCHAR *str,
    const TCHAR *replace_str, int all_flg,
    TCHAR *result_buf, int result_buf_size);
```

**パラメータ:**
- `reg_data`: コンパイルされた正規表現ハンドル
- `str`: 対象文字列
- `replace_str`: 置換パターン
- `all_flg`: すべてのマッチを置換する場合は1、最初のマッチのみの場合は0
- `result_buf`: 結果を格納するバッファ（NULL可、内部でメモリ確保）
- `result_buf_size`: 結果バッファのサイズ（result_bufがNULLでない場合に使用）

**戻り値:**
- 成功時: 置換後の文字列へのポインタ（result_bufまたは内部で確保したバッファ）
- 失敗時: NULL（メモリ確保失敗時など）

### oreg_replace_str_cb

コールバック関数を使用して置換を行います。各マッチごとにコールバック関数が呼び出され、置換文字列を動的に生成できます。

```c
typedef TCHAR *(*oreg_replace_callback_func)(
    void *callback_arg, HREG_DATA reg_data);
    
TCHAR *oreg_replace_str_cb(
    HREG_DATA reg_data, const TCHAR *str,
    oreg_replace_callback_func callback_func, void *callback_arg, int all_flg,
    TCHAR *result_buf, int result_buf_size);
```

**パラメータ:**
- `reg_data`: コンパイルされた正規表現ハンドル
- `str`: 対象文字列
- `callback_func`: 置換文字列を生成するコールバック関数
- `callback_arg`: コールバック関数に渡される追加データ
- `all_flg`: すべてのマッチを置換する場合は1、最初のマッチのみの場合は0
- `result_buf`: 結果を格納するバッファ（NULL可、内部でメモリ確保）
- `result_buf_size`: 結果バッファのサイズ（result_bufがNULLでない場合に使用）

**戻り値:**
- 成功時: 置換後の文字列へのポインタ（result_bufまたは内部で確保したバッファ）
- 失敗時: NULL（メモリ確保失敗時など）

## ユーティリティ関数

### oregexp

シンプルな正規表現マッチング関数。パターンのコンパイルと実行を一度に行います。
内部で`oreg_comp`と`oreg_exec_str`を呼び出しています。

```c
int oregexp(const TCHAR *pattern, const TCHAR *str, INT_PTR *start, INT_PTR *len);
```

**パラメータ:**
- `pattern`: 正規表現パターン
- `str`: 検索対象文字列
- `start`: マッチ開始位置を格納するポインタ
- `len`: マッチ長を格納するポインタ

**戻り値:**
- `OREGEXP_FOUND`: マッチングが見つかった
- `OREGEXP_NOT_FOUND`: マッチングが見つからなかった
- `OREGEXP_COMP_ERR`: コンパイルエラー

### oregexp_lwr / oregexp_lwr2

`oregexp`の拡張版で、オプションフラグを指定可能です。

```c
int oregexp_lwr(const TCHAR *pattern, const TCHAR *str, INT_PTR *start, INT_PTR *len, int i_flg);
int oregexp_lwr2(const TCHAR *pattern, const TCHAR *str, INT_PTR *start, INT_PTR *len, int regexp_opt);
```

**パラメータ:**
- `pattern`: 正規表現パターン
- `str`: 検索対象文字列
- `start`: マッチ開始位置を格納するポインタ
- `len`: マッチ長を格納するポインタ
- `i_flg`: 大文字小文字を区別しない場合は1、それ以外は0（`oregexp_lwr`のみ）
- `regexp_opt`: 正規表現オプションフラグ（`oregexp_lwr2`のみ）

### oregexp_str_lwr / oregexp_str_lwr2

文字列検索用の最適化されたマッチング関数。内部で`oreg_comp_str`を使用するため、純粋な文字列検索の場合に高速です。

```c
int oregexp_str_lwr(const TCHAR *pattern, const TCHAR *str, INT_PTR *start, INT_PTR *len, int i_flg);
int oregexp_str_lwr2(const TCHAR *pattern, const TCHAR *str, INT_PTR *start, INT_PTR *len, int regexp_opt);
```

**パラメータ:**
- `pattern`: 検索文字列（正規表現メタ文字は使用不可）
- `str`: 検索対象文字列
- `start`: マッチ開始位置を格納するポインタ
- `len`: マッチ長を格納するポインタ
- `i_flg`: 大文字小文字を区別しない場合は1、それ以外は0（`oregexp_str_lwr`のみ）
- `regexp_opt`: 正規表現オプションフラグ（`oregexp_str_lwr2`のみ）

### oregexp_print_nfa

デバッグ用：コンパイルされたNFA（非決定性有限オートマトン）を標準出力に表示します。

```c
int oregexp_print_nfa(const TCHAR *pattern);
```

**パラメータ:**
- `pattern`: 正規表現パターン

**戻り値:**
- 成功時：0
- 失敗時：エラーコード

### oreg_free

コンパイルされた正規表現パターンを解放します。ハンドルが不要になったら必ず呼び出してメモリリークを防止します。

```c
void oreg_free(HREG_DATA reg_data);
```

**パラメータ:**
- `reg_data`: 解放する正規表現ハンドル

## 日本語処理

oregexpは日本語文字セットのサポートを特徴としており、以下の文字エンコーディングに対応しています：

### 対応エンコーディング

- **EUC-JP**: 主に日本語Unix/Linuxシステムで使用されるエンコーディング
- **UTF-8**: Unicode文字の標準的なエンコーディング

### 文字種の判別と操作

正規表現内では、以下のように日本語文字に対応した文字クラスや演算子を利用できます：

- `[あ-お]`: ひらがな「あ」から「お」までの範囲
- `[ぁ-ん]`: 全てのひらがな
- `[ァ-ン]`: 全てのカタカナ
- `[亜-熙]`: 漢字の範囲指定も可能

### 全角半角の区別

`OREGEXP_OPT_IGNORE_WIDTH_ASCII`オプションを使用すると、ASCII文字の全角と半角を区別せずにマッチングできます。例えば：

- `A` は `Ａ`（全角）にもマッチする
- `123` は `１２３`（全角）にもマッチする

### パターンフラグ

正規表現パターン内では以下の特殊フラグも使用できます：

- `(?w)`: パターン内でASCII文字の全角/半角を区別しない

例：`(?w)A+` は `AAA`, `ＡＡＡ`, `AＡA` などにマッチします。

### 日本語関連ユーティリティ関数

正規表現エンジンの内部で使用されている日本語文字処理用の補助関数：

- `get_char_utf8.h`: UTF-8エンコーディングでの文字処理
- `get_char_euc.h`: EUC-JPエンコーディングでの文字処理
- `check_kanji.h`: 漢字判定関連の処理

## 使用例

### 基本的なパターンマッチング

最も簡単な正規表現マッチングの例です：

```c
#include "oregexp.h"
#include <stdio.h>

int main() {
    INT_PTR start, len;
    int result = oregexp("[a-z]+", "Hello world", &start, &len);
    
    if (result == OREGEXP_FOUND) {
        printf("Found match at position %d, length %d\n", (int)start, (int)len);
    } else {
        printf("No match found\n");
    }
    
    return 0;
}
```

### キャプチャグループを使ったマッチング

正規表現でキャプチャグループ（カッコで囲まれた部分）を使用する例：

```c
#include "oregexp.h"
#include <stdio.h>
#include <string.h>

int main() {
    const TCHAR *pattern = "([0-9]+)-([a-z]+)";
    const TCHAR *str = "123-abc 456-def";
    TCHAR buf[256];
    
    HREG_DATA reg = oreg_comp(pattern, 0);
    if (!reg) {
        printf("Compile error\n");
        return 1;
    }
    
    OREG_DATASRC src;
    oreg_make_str_datasrc(&src, str);
    
    if (oreg_exec2(reg, &src) == OREGEXP_FOUND) {
        // マッチした全体の文字列を取得
        OREG_POINT start, end;
        oreg_get_match_data(reg, &start, &end);
        
        // 最初のキャプチャグループ
        oreg_get_backref_str(&src, reg, 1, buf);
        printf("Group 1: %s\n", buf);  // "123"
        
        // 2番目のキャプチャグループ
        oreg_get_backref_str(&src, reg, 2, buf);
        printf("Group 2: %s\n", buf);  // "abc"
    }
    
    oreg_free(reg);
    return 0;
}
```

### 文字列置換

正規表現パターンに一致した部分を置換する例：

```c
#include "oregexp.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    const TCHAR *pattern = "([0-9]+)";
    const TCHAR *str = "123 and 456";
    const TCHAR *replace = "number($1)";
    TCHAR result[256];
    
    HREG_DATA reg = oreg_comp(pattern, 0);
    if (!reg) {
        printf("Compile error\n");
        return 1;
    }
    
    oreg_replace_str(reg, str, replace, 1, result, sizeof(result)/sizeof(TCHAR));
    printf("Result: %s\n", result);  // "number(123) and number(456)"
    
    oreg_free(reg);
    return 0;
}
```

### コールバック関数を使った置換

より高度な置換処理を行う場合は、コールバック関数を使用できます：

```c
#include "oregexp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TCHAR* replace_callback(void *arg, HREG_DATA reg_data) {
    static TCHAR buf[256];
    TCHAR num[32];
    OREG_DATASRC *src = (OREG_DATASRC *)arg;
    
    // キャプチャグループ1を取得
    oreg_get_backref_str(src, reg_data, 1, num);
    
    // 数値を2倍にして文字列を作成
    int value = atoi(num);
    sprintf(buf, "doubled(%d)", value * 2);
    
    return buf;
}

int main() {
    const TCHAR *pattern = "([0-9]+)";
    const TCHAR *str = "123 and 456";
    TCHAR result[256];
    
    HREG_DATA reg = oreg_comp(pattern, 0);
    if (!reg) {
        printf("Compile error\n");
        return 1;
    }
    
    OREG_DATASRC src;
    oreg_make_str_datasrc(&src, str);
    
    oreg_replace_str_cb(reg, str, replace_callback, &src, 1, result, sizeof(result)/sizeof(TCHAR));
    printf("Result: %s\n", result);  // "doubled(246) and doubled(912)"
    
    oreg_free(reg);
    return 0;
}
```

### 日本語文字列の処理

日本語文字列の検索と置換の例：

```c
#include "oregexp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // 日本語文字列を検索
    const TCHAR *pattern = "[あ-お]+[かきく]*";
    const TCHAR *str = "あいうえおかきく";
    INT_PTR start, len;
    
    int result = oregexp(pattern, str, &start, &len);
    if (result == OREGEXP_FOUND) {
        printf("Found Japanese match at position %d, length %d\n", (int)start, (int)len);
    }
    
    // 全角半角を区別しない検索
    const TCHAR *ascii_pattern = "ABC";
    const TCHAR *mixed_str = "abcＡＢＣdef";
    
    HREG_DATA reg = oreg_comp2(ascii_pattern, OREGEXP_OPT_IGNORE_CASE | OREGEXP_OPT_IGNORE_WIDTH_ASCII);
    if (!reg) {
        printf("Compile error\n");
        return 1;
    }
    
    OREG_DATASRC src;
    oreg_make_str_datasrc(&src, mixed_str);
    
    OREG_POINT search_start = {0, 0};
    OREG_POINT search_end = {-1, -1};  // 文字列の終端まで
    OREG_POINT result_start, result_end;
    
    if (oreg_exec(reg, &src, &search_start, &search_end, &result_start, &result_end, 0) == OREGEXP_FOUND) {
        printf("Found mixed-width match at position %d, length %d\n", 
               (int)result_start.col, (int)(result_end.col - result_start.col));
    }
    
    oreg_free(reg);
    return 0;
}
```

## 実装ノート

oregexpの内部実装に関する技術的な詳細です。

### 正規表現エンジンの仕組み

oregexpは非決定性有限オートマトン（NFA）ベースの正規表現エンジンを実装しています。NFAは正規表現パターンを有限オートマトンに変換し、それを使ってマッチングを行います。

1. **コンパイルフェーズ**：
   - 正規表現パターンを字句解析（`lexer.c`）
   - パターンをNFA形式に変換（`parser.c`）
   - NFAの最適化（可能であればボイヤー・ムーア法の適用など）

2. **実行フェーズ**：
   - バックトラッキングによるマッチング（`regexp.c`）
   - 文字列操作とマッチ処理（`regexp_str.c`）
   - 置換処理（`replace.c`）

### 文字セット処理

日本語文字セットのサポートは以下のモジュールで実装されています：

- **EUC-JP処理** (`get_char_euc.h`):
  - EUC-JP エンコードでの日本語文字の取得と変換
  - 日本語の文字幅と位置情報の処理

- **UTF-8処理** (`get_char_utf8.h`):
  - UTF-8 エンコードでの日本語文字の取得と変換
  - Unicode対応の文字処理

- **漢字判定** (`check_kanji.c`, `check_kanji.h`):
  - 文字コード自動判別
  - 漢字、ひらがな、カタカナの識別

### 最適化技術

oregexpでは以下のような最適化技術を採用しています：

1. **パターン最適化**：
   - 単純文字列検索の場合は高速なボイヤー・ムーア法を使用
   - 先頭文字のチェックによる早期スキップ

2. **メモリ管理**：
   - 効率的なバッファ管理
   - 動的メモリ確保の最小化

3. **バックトラッキングの制御**：
   - アトミックグループのサポート
   - 先読み・後読み判定の最適化

### 移植性と互換性

- ANSI C準拠のコードで、さまざまなプラットフォームへの移植が容易
- Perl5互換の正規表現構文をサポート（先読み、後読み、条件式など）
- Unicode対応で、多言語での利用が可能

## 高度な使用法

より複雑な正規表現機能とその使い方について説明します。

### 先読み・後読みアサーション

oregexpは肯定/否定の先読みと後読みアサーションをサポートしています：

```c
// 肯定先読み: "abc"の後に"123"が続く場合に"abc"にマッチ
const TCHAR *pattern1 = "abc(?=123)";

// 否定先読み: "abc"の後に"123"が続かない場合に"abc"にマッチ
const TCHAR *pattern2 = "abc(?!123)";

// 肯定後読み: "123"の前に"abc"がある場合に"123"にマッチ
const TCHAR *pattern3 = "(?<=abc)123";

// 否定後読み: "123"の前に"abc"がない場合に"123"にマッチ
const TCHAR *pattern4 = "(?<!abc)123";
```

### 条件付きマッチング

条件式を使用した複雑なパターンマッチングも可能です：

```c
// キャプチャグループ1がマッチした場合と失敗した場合で異なるパターンを使用
const TCHAR *pattern = "(A)?B(?(1)C|D)";
// このパターンは "ABC" または "BD" にマッチします
```

### アトミックグループ

一度マッチしたらバックトラックしないアトミックグループの例：

```c
// 通常のグループは必要に応じてバックトラックします
const TCHAR *normal = "(a+)b";  // "aaaab" にマッチ

// アトミックグループはバックトラックしません
const TCHAR *atomic = "(?>a+)b";  // "aaaab" にマッチしない
```

### 再帰的パターン

再帰的なパターンを使用して入れ子構造を処理する例：

```c
// カッコの内容を再帰的にマッチ
const TCHAR *recursive = "\\((?:[^()]|(?R))*\\)";
// "((a))", "(a(b)c)", "()" などの文字列にマッチ
```

### Unicodeプロパティ

UTF-8モードでは、Unicodeプロパティも使用できます：

```c
// すべての日本語の漢字にマッチ
const TCHAR *kanji_pattern = "\\p{Han}+";

// すべてのひらがなにマッチ
const TCHAR *hiragana_pattern = "\\p{Hiragana}+";

// すべての英数字にマッチ
const TCHAR *alphanumeric_pattern = "\\p{Alnum}+";

// 特定のUnicodeブロックに属する文字にマッチ
const TCHAR *block_pattern = "[\\p{Hiragana}\\p{Katakana}]+";
```

### 名前付きキャプチャグループ

名前付きキャプチャグループを使用して可読性を高める例：

```c
// 名前付きキャプチャグループを使用
const TCHAR *named = "(?<year>\\d{4})-(?<month>\\d{2})-(?<day>\\d{2})";

// 名前付きバックリファレンスを使用
const TCHAR *named_ref = "(?<char>a)\\k<char>+";  // "aa", "aaa" などにマッチ
```

### オブジェクト指向スタイルの使用

C++でのラッパークラスを使用した例（参考実装）：

```cpp
class RegExp {
public:
    RegExp(const TCHAR* pattern, int flags = 0) {
        m_regData = oreg_comp2(pattern, flags);
    }
    
    ~RegExp() {
        if (m_regData) oreg_free(m_regData);
    }
    
    bool match(const TCHAR* str, int* start = nullptr, int* len = nullptr) {
        INT_PTR _start, _len;
        int result = oreg_exec_str(m_regData, str, 0, &_start, &_len);
        
        if (start) *start = static_cast<int>(_start);
        if (len) *len = static_cast<int>(_len);
        
        return result == OREGEXP_FOUND;
    }
    
    std::string replace(const TCHAR* str, const TCHAR* replacement, bool all = true) {
        TCHAR result[1024];
        oreg_replace_str(m_regData, str, replacement, all ? 1 : 0, result, sizeof(result)/sizeof(TCHAR));
        return result;
    }

private:
    HREG_DATA m_regData;
};
```

### カスタムデータソースの実装

OREG_DATASRCを使用して、非連続メモリ構造でもoregexpを使用できます。
以下は、gapped bufferやpiece tableなどの非連続メモリでテキストを管理するエディタでの実装例です。

#### Gapped Bufferの実装例

Gapped bufferは、テキストエディタでよく使用される効率的なテキスト表現方法です：

```c
typedef struct gapped_buffer_st {
    TCHAR *buffer;      // 実際のテキストバッファ
    size_t buf_size;    // バッファサイズ
    size_t gap_start;   // ギャップ開始位置
    size_t gap_end;     // ギャップ終了位置
    size_t text_len;    // 実際のテキスト長
} GappedBuffer;

// Gapped buffer用のORED_DATASRC関数の実装
static const TCHAR *get_char_buf_gapped(const void *src, OREG_POINT *pt) {
    GappedBuffer *gb = (GappedBuffer *)src;
    size_t pos = pt->col;
    
    if (pos < gb->gap_start) {
        return gb->buffer + pos;
    } else {
        // ギャップを跨いでアクセス
        pos += (gb->gap_end - gb->gap_start);
        if (pos >= gb->buf_size) {
            static const TCHAR null_char = '\0';
            return &null_char;
        }
        return gb->buffer + pos;
    }
}

static const TCHAR *get_row_buf_gapped_len(const void *src, INT_PTR row, size_t *len) {
    GappedBuffer *gb = (GappedBuffer *)src;
    if (row > 0) return NULL;  // 単一行のみサポート
    
    *len = gb->text_len;
    return gb->buffer;  // 実際の実装では行の開始位置を返す
}

static const TCHAR *next_char_gapped(const void *src, OREG_POINT *pt) {
    GappedBuffer *gb = (GappedBuffer *)src;
    size_t pos = pt->col;
    
    if (pos < gb->gap_start) {
        pt->col++;
        if (pt->col == gb->gap_start) {
            // ギャップをスキップ
            pt->col = gb->gap_end;
        }
    } else {
        pt->col++;
    }
    
    return get_char_buf_gapped(src, pt);
}

static const TCHAR *prev_char_gapped(const void *src, OREG_POINT *pt) {
    GappedBuffer *gb = (GappedBuffer *)src;
    
    if (pt->col == 0) {
        return NULL;
    }
    
    pt->col--;
    if (pt->col == gb->gap_start && gb->gap_start > 0) {
        // ギャップの直前に戻る
        pt->col = gb->gap_start - 1;
    }
    
    return get_char_buf_gapped(src, pt);
}

static INT_PTR get_len_gapped(const void *src, OREG_POINT *start_pt, OREG_POINT *end_pt) {
    GappedBuffer *gb = (GappedBuffer *)src;
    size_t start_pos = start_pt->col;
    size_t end_pos = end_pt->col;
    size_t len = 0;
    
    // ギャップを考慮した長さ計算
    if (start_pos < gb->gap_start && end_pos <= gb->gap_start) {
        len = end_pos - start_pos;
    } else if (start_pos >= gb->gap_start && end_pos >= gb->gap_start) {
        len = end_pos - start_pos;
    } else {
        // ギャップを跨ぐ場合
        len = (gb->gap_start - start_pos) + (end_pos - gb->gap_start);
    }
    
    return len;
}

// OREG_DATASRCの初期化
void oreg_make_gapped_datasrc(OREG_DATASRC *data_src, GappedBuffer *gb) {
    memset(data_src, 0, sizeof(OREG_DATASRC));
    data_src->src = gb;
    data_src->get_char_buf = get_char_buf_gapped;
    data_src->get_row_buf_len = get_row_buf_gapped_len;
    data_src->next_char = next_char_gapped;
    data_src->prev_char = prev_char_gapped;
    data_src->get_len = get_len_gapped;
    // その他の関数ポインタも必要に応じて設定
}

// 使用例
int search_in_gapped_buffer(GappedBuffer *gb, const TCHAR *pattern) {
    HREG_DATA reg = oreg_comp(pattern, 0);
    if (!reg) return 0;
    
    OREG_DATASRC src;
    oreg_make_gapped_datasrc(&src, gb);
    
    OREG_POINT search_start = {0, 0};
    OREG_POINT search_end = {-1, -1};
    OREG_POINT result_start, result_end;
    
    int result = oreg_exec(reg, &src, &search_start, &search_end, 
                          &result_start, &result_end, 0);
    
    oreg_free(reg);
    return result == OREGEXP_FOUND;
}
```

#### Piece Tableの実装例

Piece tableは、Microsoft Wordなどで使用される効率的なテキスト編集構造です：

```c
typedef struct piece_st {
    int is_original;     // 元のバッファかどうか
    size_t start;        // ピース内の開始位置
    size_t length;       // ピースの長さ
    struct piece_st *next;
} Piece;

typedef struct piece_table_st {
    TCHAR *original;     // 元のテキストバッファ
    TCHAR *added;        // 追加されたテキストバッファ
    size_t added_size;
    Piece *pieces;       // ピースのリスト
    size_t total_length; // 総テキスト長
} PieceTable;

// Piece table用の関数実装
static const TCHAR *get_char_buf_piece(const void *src, OREG_POINT *pt) {
    PieceTable *pt_table = (PieceTable *)src;
    size_t pos = pt->col;
    size_t current_pos = 0;
    
    for (Piece *piece = pt_table->pieces; piece; piece = piece->next) {
        if (pos >= current_pos && pos < current_pos + piece->length) {
            size_t offset = pos - current_pos;
            if (piece->is_original) {
                return pt_table->original + piece->start + offset;
            } else {
                return pt_table->added + piece->start + offset;
            }
        }
        current_pos += piece->length;
    }
    
    static const TCHAR null_char = '\0';
    return &null_char;
}

// その他の関数も同様に実装...

void oreg_make_piece_datasrc(OREG_DATASRC *data_src, PieceTable *pt) {
    memset(data_src, 0, sizeof(OREG_DATASRC));
    data_src->src = pt;
    data_src->get_char_buf = get_char_buf_piece;
    // 他の関数ポインタを設定...
}
```

これらの実装により、oregexpは連続したメモリ配置に依存せず、様々なテキスト管理構造で
効率的に動作できます。テキストエディタの内部データ構造に関係なく、統一された
正規表現インターフェースを提供できるのがORED_DATASRCの大きな利点です。
