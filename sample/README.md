# oregexp サンプルコード

このディレクトリには、oregexp正規表現ライブラリを使用するためのサンプルコードが含まれています。各サンプルは、ライブラリの特定の機能を示しています。

## サンプル一覧

### basic_match.c

基本的なパターンマッチングを示すシンプルな例です。`oregexp()` 関数を使って文字列内のパターンを検索する方法を示しています。

```
$ ./basic_match
Found match at position 1, length 4
```

### capture_groups.c

正規表現内のキャプチャグループを使用して、マッチした部分文字列を抽出する方法を示します。

```
$ ./capture_groups
Full match: 123-abc
Group 1: 123
Group 2: abc
```

### string_replace.c

文字列内のパターンマッチを別の文字列に置換する方法を示します。置換文字列内でバックリファレンス（$1, $2など）を使用することができます。

```
$ ./string_replace
Original: 123 and 456
Result: number(123) and number(456)
```

### callback_replace.c

コールバック関数を使用してより高度な置換処理を行う方法を示します。各マッチに対してカスタム処理を実行できます。

```
$ ./callback_replace
Original: 123 and 456
Result: doubled(246) and doubled(912)
```

### japanese_text.c

日本語文字列の処理方法と、全角/半角を区別しないマッチングの機能を示します。

```
$ ./japanese_text
Found Japanese match at position 0, length 12
Found mixed-width match at position 3, length 3
```

## ビルド方法

サンプルをビルドするには、プロジェクトのルートディレクトリで以下のコマンドを実行してください：

```
$ make samples
```

または個別のサンプルをビルドするには：

```
$ make sample/basic_match
$ make sample/capture_groups
$ make sample/string_replace
$ make sample/callback_replace
$ make sample/japanese_text
```

## 注意事項

- サンプルコードは教育目的で提供されています。実際のアプリケーションでは、エラー処理やメモリ管理などの詳細に注意してください。
- これらのサンプルは基本的な使用法を示すものです。より高度な機能については、API_REFERENCE.mdを参照してください。
