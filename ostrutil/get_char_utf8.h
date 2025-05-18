/*
 * Copyright (c) 2025, Atsushi Ogawa
 * All rights reserved.
 *
 * This software is licensed under the BSD 2-Clause License.
 * See the LICENSE file for details.
 */


#ifdef  __cplusplus
extern "C" {
#endif

#ifndef WIN32
	#ifndef __inline
	#define __inline
	#endif
#endif

#ifndef WIN32
typedef unsigned char TCHAR;
/*
typedef char TCHAR;
*/
typedef unsigned int UINT_PTR;

#define _T(str)		str
#define _tcschr		strchr
#define _tcslen		strlen
#define _tcscmp		strcmp
#define _tcsncmp	strncmp
#define _tcscat		strcat
#define _tcsstr		strstr
#define _tcscpy		strcpy
#define _tcsncpy	strncpy
#define _tcsdup		strdup
#define _stscanf	sscanf
#define _tprintf	printf
#define _stprintf	sprintf
#define _ttoi		atoi
#define _tcsftime	strftime

#define _tfopen		fopen
#define _fgetts		fgets
#define _fputts		fputs
#define _taccess	access
#define _tstat		stat

#endif

#define LARGE_A			L'Ａ'
#define LARGE_Z			L'Ｚ'
#define SMALL_A			L'ａ'
#define SMALL_Z			L'ｚ'
#define ZENKAKU_SPACE	L'　'

__inline static unsigned int inline_ismblower(unsigned int ch)
{
	return (ch <= SMALL_Z && ch >= SMALL_A);
}

__inline static unsigned int inline_ismbupper(unsigned int ch)
{
	return (ch <= LARGE_Z && ch >= LARGE_A);
}

__inline static unsigned int inline_tolower(unsigned int ch)
{
	if(ch <= 'Z' && ch >= 'A') return ch + ('a' - 'A');
	return ch;
}

__inline static unsigned int inline_toupper(unsigned int ch)
{
	if(ch <= 'z' && ch >= 'a') return ch - ('a' - 'A');
	return ch;
}

__inline static unsigned int inline_tolower_mb(unsigned int ch)
{
	if(ch <= 0x80) {
		return inline_tolower(ch);
	} else if(inline_ismbupper(ch)) {
		return ch + (SMALL_A - LARGE_A);
	}
	return ch;
}

__inline static unsigned int inline_toupper_mb(unsigned int ch)
{
	if(ch <= 0x80) {
		return inline_toupper(ch);
	} else if(inline_ismblower(ch)) {
		return ch - (SMALL_A - LARGE_A);
	}
	return ch;
}

/* UTF-8文字のバイト長を取得する関数 */
__inline static int utf8_char_length(unsigned char first_byte)
{
    if (first_byte < 0x80) return 1;        // ASCII (0xxxxxxx)
    if ((first_byte & 0xE0) == 0xC0) return 2; // 2バイト文字 (110xxxxx)
    if ((first_byte & 0xF0) == 0xE0) return 3; // 3バイト文字 (1110xxxx)
    if ((first_byte & 0xF8) == 0xF0) return 4; // 4バイト文字 (11110xxx)
    return 1; // 不正なUTF-8バイト
}

/* UTF-8文字列の1文字のバイト長を取得 */
__inline static int get_char_len(const TCHAR *p)
{
    return utf8_char_length(*p);
}

/* 文字コードポイントからUTF-8での必要バイト長を計算 */
__inline static int get_char_len2(unsigned int ch)
{
    if (ch < 0x80) return 1;
    if (ch < 0x800) return 2;
    if (ch < 0x10000) return 3;
    if (ch < 0x110000) return 4;
    return 3; // 不正な値の場合はUTF-8で3バイト文字として扱う
}

/* 先頭バイトが新しい文字の先頭かどうか判定 */
__inline static int is_lead_byte(TCHAR ch)
{
    return ((ch & 0xC0) != 0x80); // 0x80-0xBFは継続バイト
}

/* 指定位置のバイトが文字の先頭かどうか判定 */
__inline static int is_lead_byte2(const TCHAR *mbstr, UINT_PTR count)
{
    return is_lead_byte(mbstr[count]);
}

/* UTF-8文字列から1つのコードポイントを取得 */
__inline static unsigned int get_char(const TCHAR *p)
{
    unsigned char c = *p;
    if (c < 0x80) return c; // ASCII
    
    int len = utf8_char_length(c);
    unsigned int ch = 0;
    
    switch (len) {
        case 2:
            ch = ((c & 0x1F) << 6) | (*(p+1) & 0x3F);
            break;
        case 3:
            ch = ((c & 0x0F) << 12) | ((*(p+1) & 0x3F) << 6) | (*(p+2) & 0x3F);
            break;
        case 4:
            ch = ((c & 0x07) << 18) | ((*(p+1) & 0x3F) << 12) | 
                ((*(p+2) & 0x3F) << 6) | (*(p+3) & 0x3F);
            break;
        default:
            return c; // 不正なUTF-8シーケンス
    }
    
    return ch;
}

__inline static unsigned int get_char_nocase(const TCHAR *p)
{
	return inline_tolower_mb(get_char(p));
}

__inline static TCHAR *put_char(TCHAR *p, unsigned int ch)
{
    if (ch < 0x80) {
        *p++ = (TCHAR)ch;
    }
    else if (ch < 0x800) {
        *p++ = (TCHAR)(0xC0 | (ch >> 6));
        *p++ = (TCHAR)(0x80 | (ch & 0x3F));
    }
    else if (ch < 0x10000) {
        *p++ = (TCHAR)(0xE0 | (ch >> 12));
        *p++ = (TCHAR)(0x80 | ((ch >> 6) & 0x3F));
        *p++ = (TCHAR)(0x80 | (ch & 0x3F));
    }
    else if (ch < 0x110000) {
        *p++ = (TCHAR)(0xF0 | (ch >> 18));
        *p++ = (TCHAR)(0x80 | ((ch >> 12) & 0x3F));
        *p++ = (TCHAR)(0x80 | ((ch >> 6) & 0x3F));
        *p++ = (TCHAR)(0x80 | (ch & 0x3F));
    }
    return p;
}

__inline static const TCHAR *get_prev_str(const TCHAR *p)
{
    const TCHAR *q = p - 1;
    while (q >= p - 4 && ((*q & 0xC0) == 0x80)) {
        --q;
    }
    return q;
}

__inline static unsigned int get_ascii_char(unsigned int ch)
{
	if(inline_ismblower(ch)) {
		return ch - (SMALL_A - 'a');
	}
	if(inline_ismbupper(ch)) {
		return ch - (LARGE_A - 'A');
	}

	return ch;
}

#ifdef  __cplusplus
}
#endif
