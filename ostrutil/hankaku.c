/*
 * Copyright (c) 2025, Atsushi Ogawa
 * All rights reserved.
 *
 * This software is licensed under the BSD 2-Clause License.
 * See the LICENSE file for details.
 */


#include <string.h>
#ifdef WIN32
#include <mbstring.h>
#endif

#include "mbutil.h"
#include "get_char.h"
#include "str_inline.h"

static int __HankakuToZenkaku(const TCHAR *str, TCHAR *buf,
	int b_alpha, int b_kata)
{
	static TCHAR* convTbl = _T("�B�u�v�A�E���@�B�D�F�H�������b�[�A�C�E�G�I�J�L�N�P�R�T�V�X�Z�\�^�`�c�e�g�i�j�k�l�m�n�q�t�w�z�}�~���������������������������J�K");
	static TCHAR* pszDakuSet = _T("��������������������");
	static TCHAR* dakuConvTbl = _T("�K�M�O�Q�S�U�W�Y�[�]�_�a�d�f�h�o�r�u�x�{");
	static TCHAR* pszYouSet = _T("�����");
	static TCHAR* dakuYouTbl = _T("�p�s�v�y�|");

	TCHAR *pbuf = buf;
	TCHAR *cp;
	unsigned int ch;
	int	flg = 0;

	for(; *str != '\0';) {
		ch = get_char(str);

		// ���p�p��
		if(b_alpha && ch >= 0x20 && ch <= 0x7E) {
			flg = 1;
			if(ch == ' ') {
				ch = L'�@';
			} else if(ch == '\\') {
				ch = L'��';
			} else if(ch == '~') {
				ch = L'�P';
			} else {
				// '!' - '}'
				ch += L'�I' - L'!';
			}
		}

		// ���p�J�^�J�i
		if(b_kata && ch >= 0xFF61 && ch <= 0xFF9F) {
			flg = 1;

			if(get_char(str + get_char_len(str)) == L'�' && (cp = inline_strchr(pszDakuSet, *str)) != NULL) {
				ch = dakuConvTbl[cp - pszDakuSet];
				str += get_char_len(str);
			} else if(get_char(str + get_char_len(str)) == L'�' && (cp = inline_strchr(pszYouSet, *str)) != NULL) {
				ch = dakuYouTbl[cp - pszYouSet];
				str += get_char_len(str);
			} else {
				ch = convTbl[ch - 0xFF61];
			}
		}

		pbuf = put_char(pbuf, ch);
		str += get_char_len(str);
	}

	*pbuf = '\0';
	return flg;
}

// ���p�������S�p������ɕϊ�����
int HankakuToZenkaku(const TCHAR *str, TCHAR *buf)
{
	return __HankakuToZenkaku(str, buf, 1, 1);
}

int HankakuToZenkaku2(const TCHAR *str, TCHAR *buf,
	int b_alpha, int b_kata)
{
	return __HankakuToZenkaku(str, buf, b_alpha, b_kata);
}

#define GET_CHAR(p) (*p) + *(p + 1)

static int __ZenkakuToHankaku(const TCHAR *str, TCHAR *buf,
	int b_alpha, int b_kata)
{
	static TCHAR* zenKataSet = _T("�B�u�v�A�E���@�B�D�F�H�������b�[�A�C�E�G�I�J�L�N�P�R�T�V�X�Z�\�^�`�c�e�g�i�j�k�l�m�n�q�t�w�z�}�~���������������������������J�K");
	static TCHAR* zenKataConvTbl = _T("���������������������������������������������������������������");

	static TCHAR* zenDakuSet = _T("�K�M�O�Q�S�U�W�Y�[�]�_�a�d�f�h�o�r�u�x�{");
	static TCHAR* zenDakuConvTbl = _T("��������������������");

	static TCHAR* zenYouSet = _T("�p�s�v�y�|");
	static TCHAR* zenYouConvTbl = _T("�����");

	static TCHAR* markSet = _T("�@���P�h�f�e");
	static TCHAR* markConvTbl = _T(" \\~\"'`");

	TCHAR *pbuf = buf;
	TCHAR *cp;
	unsigned int ch;
	int	flg = 0;

	for(; *str != '\0';) {
		ch = get_char(str);

		// �S�p�p��
		if(b_alpha) {
			if(ch >= L'�I' && ch <= L'�p') {
				flg = 1;
				ch -= L'�I' - L'!';
			} else if((cp = inline_strchr(markSet, *str)) != NULL) {
				flg = 1;
				ch = markConvTbl[cp - markSet];
			}
		}

		// �S�p�J�^�J�i
		if(b_kata) {
			if((cp = inline_strchr(zenKataSet, *str)) != NULL) {
				flg = 1;
				ch = zenKataConvTbl[cp - zenKataSet];
			} else if((cp = inline_strchr(zenDakuSet, *str)) != NULL) {
				flg = 1;
				ch = zenDakuConvTbl[cp - zenDakuSet];
				pbuf = put_char(pbuf, ch);
				ch = L'�';
			} else if((cp = inline_strchr(zenYouSet, *str)) != NULL) {
				flg = 1;
				ch = zenYouConvTbl[cp - zenYouSet];
				pbuf = put_char(pbuf, ch);
				ch = L'�';
			}
		}

		pbuf = put_char(pbuf, ch);
		str += get_char_len(str);
	}

	*pbuf = '\0';
	return flg;
}

// ���p�������S�p������ɕϊ�����
int ZenkakuToHankaku(const TCHAR *str, TCHAR *buf)
{
	return __ZenkakuToHankaku(str, buf, 1, 1);
}

int ZenkakuToHankaku2(const TCHAR *str, TCHAR *buf,
	int b_alpha, int b_kata)
{
	return __ZenkakuToHankaku(str, buf, b_alpha, b_kata);
}
