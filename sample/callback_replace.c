/*
 * コールバック関数を使った置換の例
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../ostrutil/oregexp.h"

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
    printf("Original: %s\n", str);
    printf("Result: %s\n", result);  // "doubled(246) and doubled(912)"
    
    oreg_free(reg);
    return 0;
}
