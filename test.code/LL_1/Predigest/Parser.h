// Parser.h: interface for the CParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARSER_H__C281D149_A3D9_4BE1_89D8_2B2E94AB39FB__INCLUDED_)
#define AFX_PARSER_H__C281D149_A3D9_4BE1_89D8_2B2E94AB39FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable : 4290)

class CError;
class CParser  
{
public:
	CString DoParser(const CString& originalString);//�ⲿ���ýӿڣ�ʵ�ֶ�����Ĵ����л���
	CParser();
	virtual ~CParser();

protected:
	CString Expression() throw(CError);//��---���ʽ����
	CString Term() throw(CError);//��---���ʽ����
	CString Factor() throw(CError);//������---����
	void Match(char token) throw(CError);//�ж���һ���ַ��ǲ���������
	BOOL IsLetter(const char token);//�ж������ַ��Ƿ�����ĸ

protected:
	CString m_strOriginalString;//����û�������ַ���
	int m_nCurPosition;//��ʶ��ǰ����ɨ�����봮�еĵڼ����ַ�
	static const char OR;//��ʾ����---+��
	static const char AND;//��ʾ����---*��
};

#endif // !defined(AFX_PARSER_H__C281D149_A3D9_4BE1_89D8_2B2E94AB39FB__INCLUDED_)
