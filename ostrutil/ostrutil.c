/*
 * Copyright (c) 2025, Atsushi Ogawa
 * All rights reserved.
 *
 * This software is licensed under the BSD 2-Clause License.
 * See the LICENSE file for details.
 */


 /*----------------------------------------------------------------------
  strutil.c
  �����񏈗�
----------------------------------------------------------------------*/
/* Designed by A.Ogawa                                                */
/* Programed by A.Ogawa                                               */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "ostrutil.h"
#include "mbutil.h"
#include "get_char.h"
#include "str_inline.h"

/*----------------------------------------------------------------------
  �s���̉��s�R�[�h��sepa�Ŏw�肵���������폜����
----------------------------------------------------------------------*/
void ostr_chomp(TCHAR *buf, TCHAR sepa)
{
	TCHAR    *p1, *p2;

	if(buf == NULL || buf[0] == '\0') return;

	p2 = NULL;
	for(p1 = buf; *p1 != '\0'; p1 += get_char_len(p1)) {
		if(*p1 == '\n' || *p1 == '\r' || *p1 == sepa) {
			if(p2 == NULL) p2 = p1;
		} else {
			p2 = NULL;
		}
	}
	if(p2 != NULL) *p2 = '\0';
}

/*----------------------------------------------------------------------
  ���[�̃X�y�[�X���폜
----------------------------------------------------------------------*/
void ostr_trim(TCHAR *buf)
{
	int		i;
	TCHAR	*p1, *p2;

	if(buf == NULL || _tcslen(buf) == 0) return;

	/* �O���̃X�y�[�X���폜 */
	p1 = buf;
	for(i = 0; *p1 == ' '; i++, p1 += get_char_len(p1)) {
		;
	}
	memmove(buf, buf + i, _tcslen(buf) + 1 - i);

	if(_tcslen(buf) == 0) return;

	/* ����̃X�y�[�X���폜 */
	p2 = NULL;
	for(p1 = buf; *p1 != '\0'; p1 += get_char_len(p1)) {
		if(*p1 == ' ') {
			if(p2 == NULL) p2 = p1;
		} else {
			p2 = NULL;
		}
	}
	if(p2 != NULL) *p2 = '\0';
}

/*----------------------------------------------------------------------
  ���[�̃X�y�[�X���폜
----------------------------------------------------------------------*/
void ostr_trimleft(TCHAR *buf)
{
	int		i;
	TCHAR	*p1;

	if(buf == NULL || _tcslen(buf) == 0) return;

	/* �O���̃X�y�[�X���폜 */
	p1 = buf;
	for(i = 0; *p1 == ' '; i++, p1 += get_char_len(p1)) {
		;
	}
	memmove(buf, buf + i, _tcslen(buf) + 1 - i);
}

/*----------------------------------------------------------------------
  �������sepa�ŕ�������
----------------------------------------------------------------------*/
const TCHAR *ostr_split(const TCHAR *buf, TCHAR *buf2, TCHAR sepa)
{
	if(buf == NULL) {
		*buf2 = '\0';
		return NULL;
	}

	if(*buf == '\"') {
		buf++;
		for(;; buf++, buf2++) {
			if(*buf == '\0') break;
			if(*buf == '\"') {
				buf++;
				if(*buf != '\"') break;
			}
			*buf2 = *buf;
		}
	} else {
		for(;; buf++, buf2++) {
			if(*buf == '\0') break;
			if(*buf == sepa) break;
			*buf2 = *buf;
		}
	}
	*buf2 = '\0';

	if(*buf == '\0') return NULL;
	return buf + 1;
}

/*----------------------------------------------------------------------
  �����̒u��(c1 �� c2�ɒu��)
----------------------------------------------------------------------*/
void ostr_char_replace(TCHAR *s, const TCHAR c1, const TCHAR c2)
{
	for( ; *s; s += get_char_len(s) ) {
		if(*s == c1) *s = c2;
	}
}

/*----------------------------------------------------------------------
  �������𐔂���
----------------------------------------------------------------------*/
int ostr_str_char_cnt(const TCHAR *p, const TCHAR *end)
{
	int		cnt = 0;
	for(; *p != '\0' && (end == NULL || p < end); p += get_char_len(p)) {
		cnt++;
	}
	return cnt;
}

/*----------------------------------------------------------------------
  �����𐔂���
----------------------------------------------------------------------*/
int ostr_str_cnt(const TCHAR *p, unsigned int ch)
{
	int		cnt = 0;

	for(; *p != '\0'; p += get_char_len(p)) {
		if(*p == (TCHAR)ch) cnt++;
	}

	return cnt;
}

/*----------------------------------------------------------------------
  �����𐔂���
----------------------------------------------------------------------*/
int ostr_str_cnt2(const TCHAR *p, const TCHAR *end, unsigned int ch)
{
	int		cnt = 0;

	for(; *p != '\0' && p < end; p += get_char_len(p)) {
		if(*p == (TCHAR)ch) cnt++;
	}

	return cnt;
}

/*----------------------------------------------------------------------
  strncmp
----------------------------------------------------------------------*/
int ostr_strcmp_nocase(const TCHAR *p1, const TCHAR *p2)
{
	int		i;
	unsigned int	ch1, ch2;

	for(i = 0;; i++) {
		ch1 = get_char_nocase(p1);
		ch2 = get_char_nocase(p2);
		if(ch1 != ch2) return ch1 - ch2;
		if(ch1 == '\0') break;

		p1 += get_char_len(p1);
		p2 += get_char_len(p2);
	}
	return 0;
}

int ostr_strncmp_nocase(const TCHAR *p1, const TCHAR *p2, int len)
{
	int		i;
	unsigned int	ch1, ch2;

	for(i = 0; i < len; i++) {
		ch1 = get_char_nocase(p1);
		ch2 = get_char_nocase(p2);
		if(ch1 != ch2) return ch1 - ch2;

		p1 += get_char_len(p1);
		p2 += get_char_len(p2);
	}
	return 0;
}

int ostr_get_cmplen_nocase(const TCHAR *p1, const TCHAR *p2, int len)
{
	int		i;
	unsigned int	ch1, ch2;
	int		char_len;

	for(i = 0; i < len;) {
		ch1 = get_char_nocase(p1);
		ch2 = get_char_nocase(p2);
		if(ch1 != ch2) return i;

		char_len = get_char_len(p1);
		p1 += char_len;
		p2 += char_len;
		i += char_len;
	}

	return i;
}

//
// ���������܂܂�邩�`�F�b�N����
//
int ostr_is_contain_lower(const TCHAR *str)
{
	if(str == NULL) return 0;

	for(; *str != '\0'; str++) {
		if(is_lead_byte(*str)) {
			str++;
			if(*str == '\0') break;
			continue;
		}
		if(inline_islower(*str)) return 1;
	}

	return 0;
}

//
// �啶�����܂܂�邩�`�F�b�N����
//
int ostr_is_contain_upper(const TCHAR *str)
{
	if(str == NULL) return 0;

	for(; *str != '\0'; str++) {
		if(is_lead_byte(*str)) {
			str++;
			if(*str == '\0') break;
			continue;
		}
		if(inline_isupper(*str)) return 1;
	}

	return 0;
}


//
// �����񂪐������`�F�b�N����
//
int ostr_is_digit_only(const TCHAR *str)
{
	if(str == NULL) return 0;

	for(; *str != '\0'; str++) {
		if(is_lead_byte(*str)) return 0;
		if(!inline_isdigit(*str)) return 0;
	}

	return 1;
}

//
// ������ASCII���`�F�b�N����
//
int ostr_is_ascii_only(const TCHAR *str)
{
	if(str == NULL) return 0;

	for(; *str != '\0'; str++) {
		if(*str > 0x80) return 0;
	}

	return 1;
}

//
// ������̕\���������߂� (�����̃X�y�[�X�͊܂߂Ȃ�)
//
int ostr_first_line_len_no_last_space(const TCHAR *str)
{
	int		len = 0;
	int		last_space_pos = -1;

	for(; *str != '\0'; str += get_char_len(str)) {
		if(*str == ' ') {
			if(last_space_pos == -1) last_space_pos = len;
		} else {
			last_space_pos = -1;
		}
		if(*str == '\n') {
			len++;
			return len;
		}
		len += get_char_len(str);
	}
	if(last_space_pos != -1) return last_space_pos;
	return len;
}

//
// �e�L�X�g�f�[�^�𕪊�
//
static TCHAR *ostr_get_sepa_data(const TCHAR *p, TCHAR *buf, 
	int buf_size, unsigned int sepa)
{
	unsigned int	ch;
	int		char_len;

	int		mode = 0;
	if(*p == '"') {
		p++;
		mode = 1;
	}

	for(;;) {
		if(*p == '\0') break;

		ch = get_char(p);
		if(mode == 0 && (ch == sepa || ch == '\r' || ch == '\n')) break;

		if(mode == 1 && ch == '"') {
			p += get_char_len2(ch);
			ch = get_char(p);
			if(ch != '"') break;
		}

		char_len = get_char_len2(ch);
		p += char_len;

		if(buf != NULL && buf_size > char_len) {
			buf = put_char(buf, ch);
			buf_size -= char_len;
		}
	}

	if(*p == '\r' && *(p + 1) == '\n') p++;

	if(buf != NULL) *buf = '\0';
	return (TCHAR *)p;
}

//
// Excel�����Paste�f�[�^�𕪊�����
//
TCHAR *ostr_get_tsv_data(const TCHAR *p, TCHAR *buf, 
	int buf_size)
{
	return ostr_get_sepa_data(p, buf, buf_size, '\t');
}

//
// csv�f�[�^�𕪊�����
//
TCHAR *ostr_get_csv_data(const TCHAR *p, TCHAR *buf, 
	int buf_size)
{
	return ostr_get_sepa_data(p, buf, buf_size, ',');
}

//
// Excel�����Paste�f�[�^�̃Z���̐��𐔂���
//
int ostr_get_tsv_col_cnt(const TCHAR *p)
{
	int		cnt = 0;

	for(;;) {
		cnt++;
		p = ostr_get_tsv_data(p, NULL, 0);
		if(*p != '\t') break;
		p++;
	}

	return cnt;
}

//
// �����񂪐��l���`�F�b�N����
//
int ostr_str_isnum(const TCHAR *str)
{
	char first_char_flg = 0;
	char dot_flg = 0;

	for(; *str != '\0';) {
		if(is_lead_byte(*str)) return 0;

		switch(*str) {
		case ' ':
			break;
		case '-':
			if(first_char_flg) return 0;
			first_char_flg = 1;
			break;
		case '.':
			if(dot_flg) return 0;
			dot_flg = 1;
			break;
		default:
			if(!inline_isdigit(*str)) return 0;
			break;
		}

		str++;
	}

	return 1;
}

//
// tab text��format����
//
#pragma intrinsic(memcpy)
void ostr_tabbed_text_format(const TCHAR *p1, TCHAR *buf, 
	int tabstop, int tab_flg)
{
	const TCHAR	*p2 = p1;
	INT_PTR		x, tab;

	for(x = 0;;) {
		for(; *p2; p2++) if(*p2 == '\t') break;
		if(!(*p2)) {
			// '\0'�܂ŃR�s�[����
			memcpy((buf + x), p1, (p2 - p1 + 1) * sizeof(TCHAR));
			break;
		}

		if(p2 > p1) {
			memcpy((buf + x), p1, (p2 - p1) * sizeof(TCHAR));
			x = x + (p2 - p1);
		}

		tab = tabstop - (x % tabstop);

		if(tab_flg) {
			buf[x] = '\t';
			x++;
			tab--;
		}
		for(; tab; tab--, x++) {
			buf[x] = ' ';
		}

		p2++;
		p1 = p2;
	}
}
#pragma function(memcpy)

//
// tab text��size���v�Z����
//
INT_PTR ostr_calc_tabbed_text_size(const TCHAR *pstr, int tabstop)
{
	return ostr_calc_tabbed_text_size_n(pstr, tabstop, -1);
}

INT_PTR ostr_calc_tabbed_text_size_n(const TCHAR *pstr, int tabstop, int x)
{
	const TCHAR	*pstr2 = pstr;
	INT_PTR		disp_size = 0;
	int		i;

	if(x < 0) x = INT_MAX;

	for(i = 0; i < x ; i++) {
		if(!(*pstr)) break;

		if(*pstr == '\t') {
			disp_size += pstr - pstr2;
			disp_size = disp_size + tabstop - (disp_size % tabstop);

			pstr++;
			pstr2 = pstr;
		} else {
			pstr++;
		}
	}
	disp_size += pstr - pstr2;

	return disp_size;
}

//
// CSV�f�[�^���o��
//
int ostr_csv_fputs(FILE *stream, TCHAR *string, TCHAR sepa)
{
	if(string == NULL) return EOF;

	if(putc('\"', stream) == EOF) return EOF;
	for(;*string != '\0'; string++) {
		if(putc(*string, stream) == EOF) return EOF;
		if(*string == '\"') {
			if(putc('\"', stream) == EOF) return EOF;
		}
	}
	if(putc('\"', stream) == EOF) return EOF;

	putc(sepa, stream);

	return 1;
}

// atof��wchar�o�[�W����
double _ttofbak(const TCHAR *p)
{
#ifdef _UNICODE
	char buf[1024];
	wcstombs(buf, p, sizeof(buf));
	return atof(buf);
#else
	return atof(p);
#endif
}

int get_scale(const TCHAR *value)
{
	const TCHAR *p = value;

	for(; *p != '\0'; p++) {
		if(*p == '.') {
			return (int)_tcslen(p + 1);
		}
	}
	return 0;
}


//
// Oracle�̃I�u�W�F�N�g���ŁA�_�u���N�H�[�g���K�v���`�F�b�N����
//
int ostr_need_object_name_quote_for_oracle(const TCHAR *object_name)
{
	// �啶���p����, _, $, #, �}���`�o�C�g�����ȊO��quote�Ώۂɂ���
	const TCHAR *p = object_name;

	for(; *p != '\0'; p += get_char_len(p)) {
		unsigned int ch = get_char(p);

		// �S�p�p�����͔��p�ɂ���
		if(ch >= L'�I' && ch <= L'�p') {
			ch -= L'�I' - L'!';
		}

		// �啶��, ����, ASCII
		if(inline_isupper(ch) || inline_isdigit(ch) || ch >= 0x80) continue;

		// [_$#.]
		if(ch == '_' || ch == '#' || ch == '$' || ch == '.') continue;

		// ���̑���quote
		return 1;
	}

	return 0;
}
