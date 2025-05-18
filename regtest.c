/*
 * Copyright (c) 2025, Atsushi Ogawa
 * All rights reserved.
 *
 * This software is licensed under the BSD License.
 * See the LICENSE_BSD file for details.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include "local.h"
#include "get_char.h"

int oregexp_lwr_main(const TCHAR *pattern, const TCHAR *str, int search_start_col,
	int *start, int *len, int i_flg, int simple_str_flg);

struct lexer_test_st {
	TCHAR	*data;
	int		data_len;
	int		result;
	LexWord	lex_word;
	int		src_line;
};

struct lexer_test_st2 {
	TCHAR	*data;
	int		word_cnt;
	TCHAR	*result[5];
	int		src_line;
};

struct compile_test_data_st {
	TCHAR	*pattern;
	int		result;
	int		src_line;
};

struct test_data_st {
	TCHAR	*data;
	TCHAR	*pattern;
	int		result;
	int		start;
	int		len;
	int		regexp_opt;
	int		search_start_col;
	int		src_line;
};

struct replace_st {
	TCHAR	*data;
	TCHAR	*pattern;
	TCHAR	*replace_pattern;
	TCHAR	*result_str;
	int		all_flg;
	int		src_line;
};

struct perl_re_test_st {
	TCHAR	*pattern;
	TCHAR	*data;
	int		result;
	TCHAR	*replace_pattern;
	TCHAR	*replace_str;
	int		txt_line;
	int		src_line;
};

#ifdef OSTRUTIL_EUC
#include "regtest_euc.h"
#include "test/perl_re_tests_euc.h"
#include "test/org_re_tests_euc.h"
#endif
#ifdef OSTRUTIL_UTF8
#include "regtest_utf8.h"
#include "test/perl_re_tests_utf8.h"
#include "test/org_re_tests_utf8.h"
#endif

static int cmp_lex_word(LexWord *word1, LexWord *word2)
{
	if(word1->type != word2->type) return 1;
	if(word1->len != word2->len) return 1;
	if(strncmp(word1->data, word2->data, word1->len) != 0) return 1;
	return 0;
}

static int lexer_test()
{
	LexWord		lex_word;
	int			i;
	int			result;
	int			test_cnt = sizeof(lexer_test_data)/sizeof(lexer_test_data[0]);
	int			fail = 0;

	printf("=======================================================================================\n");
	printf("lexter_test(): ");

	for(i = 0; i < test_cnt; i++) {
		result = lexer(lexer_test_data[i].data, lexer_test_data[i].data_len, &lex_word);
		if(result != lexer_test_data[i].result) {
			if(fail == 0) printf("\n");
			fail++;
			printf("--- fail(line: %d): lexer() error. \n", lexer_test_data[i].src_line);
			continue;
		}
		if(result != 0) {
			continue;
		}
		if(cmp_lex_word(&lex_word, &lexer_test_data[i].lex_word) != 0) {
			if(fail == 0) printf("\n");
			fail++;
			printf("--- fail(line: %d): %d:%d, %d:%d, %.*s:%.*s\n",
				lexer_test_data[i].src_line,
				lex_word.type, lexer_test_data[i].lex_word.type,
				lex_word.len, lexer_test_data[i].lex_word.len,
				lex_word.len, lex_word.data,
				lexer_test_data[i].lex_word.len, lexer_test_data[i].lex_word.data);
			continue;
		}
	}

	printf("--- lexer_test: %d tests, %d failed\n", test_cnt, fail);

	return fail;
}

static int lexer_test2()
{
	TCHAR		*p;
	LexWord		lex_word;
	int			i, j;
	int			result;
	int			test_cnt = sizeof(lexer_test_data2)/sizeof(lexer_test_data2[0]);
	int			fail = 0;

	printf("=======================================================================================\n");
	printf("lexter_test2(): ");

	for(i = 0; i < test_cnt; i++) {
		p = lexer_test_data2[i].data;

		for(j = 0; *p != '\0'; j++) {
			result = lexer(p, -1, &lex_word);
			if(result != 0) {
				if(fail == 0) printf("\n");
				fail++;
				printf("fail!!: lexer_test2() error(1): %d-%d %s\n", i, j,
					lexer_test_data2[i].data);
				break;
			}
			if(strncmp(lex_word.data, lexer_test_data2[i].result[j], lex_word.len) != 0) {
				if(fail == 0) printf("\n");
				fail++;
				printf("fail!!: lexer_test2() error(2): %d-%d %s %.*s:%s\n", i, j,
					lexer_test_data2[i].data, lex_word.len, lex_word.data, lexer_test_data2[i].result[j]);
				break;
			}
			p = p + lex_word.len;
		}
	}

	printf("--- lexer_test2: %d tests, %d failed\n", test_cnt, fail);

	return fail;
}

static int compile_test(int idx)
{
	int		i = 0;
	int		result;
	int		fail = 0;
	int		test_cnt = sizeof(compile_test_data)/sizeof(compile_test_data[0]);
	OREG_DATA	*reg_data = NULL;

	printf("=======================================================================================\n");
	printf("compile_test(): ");

	for(i = 0; i < test_cnt; i++) {
		if(idx >= 0) i = idx;

		reg_data = oreg_comp(compile_test_data[i].pattern, 0);
		if(reg_data == NULL) {
			result = 0;
		} else {
			result = 1;
		}
		if(result != compile_test_data[i].result) {
			if(fail == 0) printf("\n");
			fail++;
			printf("--- fail(line: %d): %d:%d %s \n",
				compile_test_data[i].src_line, compile_test_data[i].result, result, compile_test_data[i].pattern);
		}
		if(idx >= 0) break;
	}

	printf("--- compile_test: %d tests, %d failed\n", test_cnt, fail);

	return fail;
}

static int regtest(TCHAR *str, TCHAR *pattern, int *start, int *len, int regexp_opt, int simple_flg, int search_start_col)
{
	if(simple_flg) {
		return oregexp_str_lwr2(pattern, str, start, len, regexp_opt);
	}
	return oregexp_lwr_main2(pattern, str, search_start_col, start, len, regexp_opt, simple_flg);
}

static int regexp_test_main(int idx, struct test_data_st *test_data, int test_cnt, int simple_flg, TCHAR *test_name)
{
	int		i;
	int		result;
	int		start;
	int		len;
	int		fail = 0;

	printf("=======================================================================================\n");
	printf("%s: ", test_name);
	
	for(i = 0; i < test_cnt; i++) {
		if(idx >= 0) i = idx;

		result = regtest(test_data[i].data, test_data[i].pattern, &start, &len, test_data[i].regexp_opt,
			simple_flg, test_data[i].search_start_col);

		if(result != test_data[i].result ||
			start != test_data[i].start ||
			len != test_data[i].len) {
			if(fail == 0) printf("\n");
			fail++;
			printf("--- fail(line: %d): result:%d, start:%d, len:%d \n",
				test_data[i].src_line, result, start, len);
			//printf("--- fail(%d): data:%s, pattern:%s result:%d, start:%d, len:%d \n",
			//	i, test_data[i].data, test_data[i].pattern, result, start, len);
		}

		if(idx >= 0) break;
	}

	printf("--- %s: %d tests, %d failed\n", test_name, test_cnt, fail);

	return fail;
}

static int simple_search_test(int idx)
{
	return regexp_test_main(idx, simple_search_test_data,
		sizeof(simple_search_test_data)/sizeof(simple_search_test_data[0]), 1, "simple_search");
}

static int regexp_search_test(int idx)
{
	return regexp_test_main(idx, regexp_test_data,
		sizeof(regexp_test_data)/sizeof(regexp_test_data[0]), 0, "regexp_search");
}

static int replace_test(int idx)
{
	int		i = 0;
	int		fail = 0;
	int		test_cnt = sizeof(replace_data)/sizeof(replace_data[0]);
	TCHAR	*result;
	TCHAR	result_buf[1024];
	OREG_DATA	*reg_data = NULL;

	printf("=======================================================================================\n");
	printf("replace_test(): ");

	for(i = 0; i < test_cnt; i++) {
		if(idx >= 0) i = idx;

		reg_data = oreg_comp(replace_data[i].pattern, 0);
		if(reg_data == NULL) {
			if(fail == 0) printf("\n");
			fail++;
			printf("--- fail(line: %d): compile error %s \n",
				replace_data[i].src_line, replace_data[i].pattern);
			if(idx >= 0) break;
			continue;
		}

		result = oreg_replace_str(reg_data, replace_data[i].data, replace_data[i].replace_pattern,
			replace_data[i].all_flg, result_buf, sizeof(result_buf));

		if(_tcscmp(result, replace_data[i].result_str) != 0) {
			if(fail == 0) printf("\n");
			fail++;
			printf("--- fail(line: %d): %s %s:%s \n",
				replace_data[i].src_line, replace_data[i].pattern,
				result, replace_data[i].result_str);
		}

		if(idx >= 0) break;
	}

	printf("--- replace_test: %d tests, %d failed\n", test_cnt, fail);

	return fail;
}

static void change_esc(TCHAR *p, TCHAR *buf)
{
	for(; *p != '\0'; p++) {
		if(get_char(p) == '\\') {
			p++;
			switch(*p) {
			case 'n':
				*buf = '\n';
				buf++;
				continue;
			case 't':
				*buf = '\t';
				buf++;
				continue;
			case 'b':
				*buf = '\\';
				buf++;
				*buf = 'b';
				buf++;
				continue;
			}
		}
		*buf = *p;
		*buf++;
	}
	*buf = '\0';
}


static int perl_re_test_main(int idx, struct perl_re_test_st *test_data, int test_cnt, TCHAR *test_name)
{
	int				i;
	int				result;
	TCHAR			str_buf[1024];
	TCHAR			str_buf2[1024];
	OREG_DATA		*reg_data = NULL;
	OREG_DATASRC	data_src;

	OREG_POINT		search_start = { 0, 0 };
	OREG_POINT		search_end = { -1, -1 };
	OREG_POINT		result_start = { 0, 0 };
	OREG_POINT		result_end = { 0, 0 };

	int				fail = 0;
	int				make_nfa_fail = 0;
	int				result_fail = 0;
	int				replace_fail = 0;

	printf("=======================================================================================\n");
	printf("%s: ", test_name);
	
	for(i = 0; i < test_cnt; i++) {
		if(idx >= 0) i = idx;

		reg_data = oreg_comp(test_data[i].pattern, 0);
		if(reg_data == NULL) {
			if(fail == 0) printf("\n");
			fail++;
			make_nfa_fail++;

			printf("--- fail(line: %d): compile error %s \n",
				test_data[i].txt_line, test_data[i].pattern);
			if(idx >= 0) break;
			continue;
		}

		change_esc(test_data[i].data, str_buf2);
		oreg_make_str_datasrc(&data_src, str_buf2);

		result = oreg_exec(reg_data, &data_src, &search_start, &search_end, &result_start, &result_end, 0);
		if(result != test_data[i].result) {
			if(fail == 0) printf("\n");
			fail++;
			result_fail++;

			printf("--- fail(line: %d): result error %s, %s, %c, %s, %s\n",
				test_data[i].txt_line, test_data[i].pattern, test_data[i].data,
				(test_data[i].result ? 'y' : 'n'),
				test_data[i].replace_pattern, test_data[i].replace_str);

			oreg_free(reg_data);
			if(idx >= 0) break;
			continue;
		}

		if(_tcscmp(test_data[i].replace_pattern, "-") != 0) {
			oreg_make_replace_str(&data_src, &result_start, &result_end, test_data[i].replace_pattern,
				reg_data, str_buf);
			if(_tcscmp(str_buf, test_data[i].replace_str) != 0) {
				if(fail == 0) printf("\n");
				fail++;
				result_fail++;

				printf("--- fail(line: %d): replace error %s, %s, %c, %s, %s %s\n",
					test_data[i].txt_line, test_data[i].pattern, test_data[i].data,
					(test_data[i].result ? 'y' : 'n'),
					test_data[i].replace_pattern, test_data[i].replace_str, str_buf);

				oreg_free(reg_data);
				if(idx >= 0) break;
				continue;
			}
		}

		oreg_free(reg_data);
		if(idx >= 0) break;
	}

	printf("--- %s: %d tests, %d failed\n", test_name, test_cnt, fail);

	return fail;
}

static int perl_re_test(int idx)
{
	return perl_re_test_main(idx, perl_re_test_data,
		sizeof(perl_re_test_data)/sizeof(perl_re_test_data[0]), "perl_re_test");
}

static int org_re_test(int idx)
{
	return perl_re_test_main(idx, org_re_test_data,
		sizeof(org_re_test_data)/sizeof(org_re_test_data[0]), "org_re_test");
}

int main(int argc, TCHAR **argv)
{
#ifndef WIN32
	setlocale(LC_ALL, "ja");
#endif

	lexer_test();
	lexer_test2();
	compile_test(-1);

	simple_search_test(-1);
	regexp_search_test(-1);

	replace_test(-1);

	perl_re_test(-1);
	org_re_test(-1);

	return 0;
}

