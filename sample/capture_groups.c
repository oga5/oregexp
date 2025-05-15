/*
 * キャプチャグループを使ったマッチングの例
 */
#include <stdio.h>
#include <string.h>
#include "../ostrutil/oregexp.h"

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
        
        // start.colからend.colまでの文字を表示
        printf("Full match: ");
        for (int i = start.col; i < end.col; i++) {
            printf("%c", str[i]);
        }
        printf("\n");
        
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
