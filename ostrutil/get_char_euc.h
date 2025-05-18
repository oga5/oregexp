#ifdef  __cplusplus
extern "C" {
#endif

#ifndef WIN32
	#ifndef __inline
	#define __inline
	#endif
#endif

typedef unsigned char TCHAR;
/*
typedef char TCHAR;
*/

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

#define LARGE_A     0xa3c1
#define LARGE_Z     0xa3da
#define SMALL_A     0xa3e1
#define SMALL_Z     0xa3fa

__inline static unsigned int inline_ismblower(unsigned int ch)
{
	return (ch <= SMALL_Z && ch >= SMALL_A);
}

__inline static unsigned int inline_ismbupper(unsigned int ch)
{
	return (ch <= LARGE_Z && ch >= LARGE_A);
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

__inline static int is_lead_byte(TCHAR ch)
{
	return ((ch & 0x80) != 0);
}

__inline static int get_char_len(const TCHAR *p)
{
	return is_lead_byte(*p) + 1;
}

__inline static int is_lead_byte2(const TCHAR *mbstr, unsigned int count)
{
	unsigned int cnt;
	for(cnt = 0; *mbstr != '\0'; mbstr++, cnt++) {
		if(is_lead_byte(*mbstr)) {
			if(cnt == count) return 1;
			mbstr++;
			cnt++;
			if(cnt == count) return 0;
			if(*mbstr == '\0') break;
		} else if(cnt == count) {
			return 0;
		}
	}
	return 0;
}

__inline static int get_char_len2(unsigned int ch)
{
	return ((ch > 0x00ff) ? 2 : 1);
}

__inline static unsigned int get_char(const TCHAR *p)
{
	if(!is_lead_byte(*p)) return *p;
	return (((*p) & 0xff) << 8) + ((*(p+1)) & 0xff);
}

__inline static unsigned int get_char_nocase(const TCHAR *p)
{
	return inline_tolower_mb(get_char(p));
}

__inline static TCHAR *put_char(TCHAR *p, unsigned int ch)
{
	if(get_char_len2(ch) == 1) {
		*p = ch;
		p++;
	} else {
		*p = (ch & 0xff00) >> 8;
		p++;
		*p = (ch & 0xff);
		p++;
	}
	return p;
}

__inline static const TCHAR *get_prev_str(const TCHAR *p)
{
    // 1バイト前が2バイト目なら2バイト戻る、そうでなければ1バイト戻る
    return (p > p - 1 && is_lead_byte(*(p - 2)) && !is_lead_byte(*(p - 1))) ? p - 2 : p - 1;
}

__inline static int get_prev_char_len(const TCHAR *p)
{
    const TCHAR *q = get_prev_str(p);
    return p - q;
}

__inline static int is_mbstr_char_boundary(const TCHAR *p_start, const TCHAR *p)
{
    if(p_start == p) return 1;  // 文字列の先頭は常に有効
    
    // 直前のバイトが先頭バイトの場合、現在位置は2バイト目（無効）
    if(p > p_start && is_lead_byte(*(p - 1))) {
        // 直前が先頭バイトなら、現在位置は2バイト目の位置
        return 0;
    }
    
    return 1;  // それ以外は有効な文字境界
}

#ifdef  __cplusplus
}
#endif
