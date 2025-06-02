/*
 * カスタムデータソースの実装例（シンプル版）
 * ファイルとメモリバッファからの読み取りを行うデータソースの実装
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../ostrutil/oregexp.h"

// テスト用のサンプルファイルを作成
void create_test_file() {
    FILE *fp = fopen("test_data.txt", "w");
    if (fp) {
        fprintf(fp, "Hello world\n");
        fprintf(fp, "This is line 2\n");
        fprintf(fp, "Pattern matching test\n");
        fprintf(fp, "Number: 12345\n");
        fprintf(fp, "Email: test@example.com\n");
        fclose(fp);
    }
}

// ファイル読み取りテスト
void test_file_reading() {
    printf("=== File Reading Test ===\n");
    
    FILE *fp = fopen("test_data.txt", "r");
    if (fp) {
        char buffer[1024];
        int line_number = 1;
        
        while (fgets(buffer, sizeof(buffer), fp) != NULL) {
            // 改行文字を削除
            size_t len = strlen(buffer);
            if (len > 0 && buffer[len - 1] == '\n') {
                buffer[len - 1] = '\0';
            }
            
            printf("Line %d: %s\n", line_number, buffer);
            
            // 各行で数字のパターンマッチング
            INT_PTR start, len_match;
            int result = oregexp("[0-9]+", buffer, &start, &len_match);
            
            if (result == OREGEXP_FOUND) {
                printf("  -> Found number at position %d, length %d: ", (int)start, (int)len_match);
                for (int i = 0; i < len_match; i++) {
                    printf("%c", buffer[start + i]);
                }
                printf("\n");
            }
            
            line_number++;
        }
        
        fclose(fp);
    } else {
        printf("Failed to open test file\n");
    }
    printf("\n");
}

// メモリバッファテスト
void test_buffer_processing() {
    printf("=== Buffer Processing Test ===\n");
    
    const char* test_lines[] = {
        "First line",
        "Second line with email@test.com",
        "Third line",
        "Number 98765",
        "Final line"
    };
    
    int num_lines = sizeof(test_lines) / sizeof(test_lines[0]);
    
    for (int i = 0; i < num_lines; i++) {
        printf("Line %d: %s\n", i + 1, test_lines[i]);
        
        // 各行でメールアドレスのパターンマッチング
        INT_PTR start, len;
        int result = oregexp("[a-zA-Z0-9]+@[a-zA-Z0-9]+\\.[a-zA-Z]+", test_lines[i], &start, &len);
        
        if (result == OREGEXP_FOUND) {
            printf("  -> Found email at position %d, length %d: ", (int)start, (int)len);
            for (int j = 0; j < len; j++) {
                printf("%c", test_lines[i][start + j]);
            }
            printf("\n");
        }
        
        // 数字のパターンマッチング
        result = oregexp("[0-9]+", test_lines[i], &start, &len);
        if (result == OREGEXP_FOUND) {
            printf("  -> Found number at position %d, length %d: ", (int)start, (int)len);
            for (int j = 0; j < len; j++) {
                printf("%c", test_lines[i][start + j]);
            }
            printf("\n");
        }
    }
    printf("\n");
}

// 複数パターンテスト
void test_multiple_patterns() {
    printf("=== Multiple Pattern Test ===\n");
    
    const char* patterns[] = {
        "[0-9]+",           // 数字
        "[a-zA-Z]+@[a-zA-Z0-9]+\\.[a-zA-Z]+", // メールアドレス
        "[Hh]ello",         // Hello/hello
        "[Tt]est"           // Test/test
    };
    
    const char* pattern_names[] = {
        "Number",
        "Email",
        "Hello",
        "Test"
    };
    
    const char* test_text = "Hello world! This is a test. Number: 12345, Email: user@example.com";
    
    printf("Test text: %s\n\n", test_text);
    
    int num_patterns = sizeof(patterns) / sizeof(patterns[0]);
    
    for (int i = 0; i < num_patterns; i++) {
        printf("Pattern '%s' (%s):\n", patterns[i], pattern_names[i]);
        
        INT_PTR start, len;
        int result = oregexp(patterns[i], test_text, &start, &len);
        
        if (result == OREGEXP_FOUND) {
            printf("  Found match at position %d, length %d: ", (int)start, (int)len);
            for (int j = 0; j < len; j++) {
                printf("%c", test_text[start + j]);
            }
            printf("\n");
        } else {
            printf("  No match found\n");
        }
        printf("\n");
    }
}

// 文字列置換テスト
void test_string_replacement() {
    printf("=== String Replacement Test ===\n");
    
    // 簡単な置換テスト
    const char* test_text = "Replace 123 and 456 with numbers";
    printf("Original text: %s\n", test_text);
    
    // oregexpライブラリの置換機能を使用
    char* result = oreg_replace_simple("[0-9]+", test_text, "NUM", 1, 0);
    if (result) {
        printf("After replacement: %s\n", result);
        free(result);
    } else {
        printf("Replacement failed\n");
    }
    printf("\n");
}

// メイン関数
int main() {
    printf("=== Custom Data Source Example (Simple Version) ===\n\n");
    
    // テスト用ファイルを作成
    create_test_file();
    
    // 各種テストを実行
    test_file_reading();
    test_buffer_processing();
    test_multiple_patterns();
    test_string_replacement();
    
    // テストファイルを削除
    remove("test_data.txt");
    
    printf("All tests completed successfully!\n");
    return 0;
}
