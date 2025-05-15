/*
 * 基本的なパターンマッチングの例
 */
#include <stdio.h>
#include "../ostrutil/oregexp.h"

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
