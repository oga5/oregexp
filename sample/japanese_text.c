/*
 * 日本語文字列の処理の例
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../ostrutil/oregexp.h"

int main() {
    // 日本語文字列を検索
    const TCHAR *pattern = "[あ-お]+[かきく]*";
    const TCHAR *str = "あいうえおかきく";
    INT_PTR start, len;
    
    int result = oregexp(pattern, str, &start, &len);
    if (result == OREGEXP_FOUND) {
        printf("Found Japanese match at position %d, length %d\n", (int)start, (int)len);
    } else {
        printf("No Japanese match found\n");
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
    } else {
        printf("No mixed-width match found\n");
    }
    
    oreg_free(reg);
    return 0;
}
