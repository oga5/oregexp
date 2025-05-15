/*
 * 文字列置換の例
 */
#include <stdio.h>
#include <stdlib.h>
#include "../ostrutil/oregexp.h"

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
    printf("Original: %s\n", str);
    printf("Result: %s\n", result);  // "number(123) and number(456)"
    
    oreg_free(reg);
    return 0;
}
