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
	CString DoParser(const CString& originalString);//外部调用接口，实现对输入的串进行化简
	CParser();
	virtual ~CParser();

protected:
	CString Expression() throw(CError);//或---表达式分析
	CString Term() throw(CError);//与---表达式分析
	CString Factor() throw(CError);//与因子---分析
	void Match(char token) throw(CError);//判断下一个字符是不是期望的
	BOOL IsLetter(const char token);//判断输入字符是否是字母

protected:
	CString m_strOriginalString;//存放用户输入的字符串
	int m_nCurPosition;//标识当前正在扫描输入串中的第几个字符
	static const char OR;//表示“或---+”
	static const char AND;//表示“与---*”
};

#endif // !defined(AFX_PARSER_H__C281D149_A3D9_4BE1_89D8_2B2E94AB39FB__INCLUDED_)
