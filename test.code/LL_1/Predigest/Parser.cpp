// Parser.cpp: implementation of the CParser class.
// download by http://www.codefans.net
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Predigest.h"
#include "Parser.h"
#include "error.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
const char CParser::AND = '*';
const char CParser::OR = '+';

CParser::CParser()
{
	m_nCurPosition = 0;
	m_strOriginalString = _T("");
}

CParser::~CParser()
{

}

CString CParser::DoParser(const CString &originalString)
{
	m_strOriginalString = originalString;//保存用户输入的字符串，准备分析
	m_nCurPosition = 0;
	CString preDigestionString = _T("");
	try
	{
		preDigestionString = Expression();//分析得到输入串的化简形式
	}
	catch(CError& error)
	{
		return error.GetErrorMessage();
	}
	//根据m_nCurPosition的值来判断用户的输入是否正常
	return ( m_nCurPosition == m_strOriginalString.GetLength() ) ? preDigestionString : CString("输入出错!");
}

CString CParser::Expression() throw(CError)
{
	CString temp = _T("");
	try
	{
		temp = Term();
		while( m_nCurPosition < m_strOriginalString.GetLength() && m_strOriginalString[m_nCurPosition] == OR )
		{
			temp += "+";
			Match(OR);
			temp += Term();
		}
	}
	catch(CError& error)
	{
		throw error;
	}
	return temp;
}

CString CParser::Term() throw(CError)
{
	CString temp = _T("");
	CString result = _T("");
	try
	{
		result = Factor();
		while( m_nCurPosition < m_strOriginalString.GetLength() && m_strOriginalString[m_nCurPosition] == AND )
		{
			CString L_Operand = result;
			Match(AND);
			CString R_Operand = Factor();
			//左右操作数进行叉乘，利用链表实现
			CList<CString, CString&> L_List, R_List;
			//将左右操作数以“+”进行分割，放入链表中
			while(TRUE)
			{
				int nPos = L_Operand.Find('+');
				if(nPos != -1)
				{
					L_List.AddTail(L_Operand.Left(nPos));
					L_Operand.Delete(0, nPos+1);
				}
				else
				{
					//表示已经到达表达式的最后一部份
					L_List.AddTail(L_Operand);
					break;
				}
			}//end_while
			while(TRUE)
			{
				int nPos = R_Operand.Find('+');
				if(nPos != -1)
				{
					R_List.AddTail(R_Operand.Left(nPos));
					R_Operand.Delete(0, nPos+1);
				}
				else
				{
					R_List.AddTail(R_Operand);
					break;
				}
			}//end_while
			//字符串按‘+’分割成功，现在进行叉乘
			temp.Empty();
			POSITION L_Pos = L_List.GetHeadPosition();
			while(L_Pos != NULL)
			{
				CString L = L_List.GetNext(L_Pos);
				POSITION R_Pos = R_List.GetHeadPosition();
				while(R_Pos != NULL)
				{
					CString R = R_List.GetNext(R_Pos);
					temp += L;
					temp += "*";
					temp += R;
					temp += "+";
				}//end_R_while
			}//end_L_while
			//去掉最后一个‘+’号
			temp.TrimRight('+');
			result = temp;
		}
	}
	catch(CError& error)
	{
		throw error;
	}
	return result;
}

CString CParser::Factor() throw(CError)
{
	CString temp = _T("");
	//如果下一个字符是单个字符，则格式化后返回，否则嵌套进行表达式的分析
	try
	{
		if( IsLetter(m_strOriginalString[m_nCurPosition]) )
		{
			temp.Format("%c", m_strOriginalString[m_nCurPosition]);
			Match(m_strOriginalString[m_nCurPosition]);
		}
		else if( m_strOriginalString[m_nCurPosition] == '[' )
		{
			//关键
			Match('[');
			temp = Expression();
			Match(']');
		}
		else
		{
			//出错
			CError error("输入出错!");
			throw error;
		}
	}
	catch(CError& error)
	{
		throw error;
	}
	return temp;
}

BOOL CParser::IsLetter(const char token)
{
	BOOL bLetter = FALSE;
	bLetter = (token >= 'a' && token <= 'z') || (token >= 'A' && token <= 'Z');
	return bLetter;
}

void CParser::Match(char token) throw(CError)
{
	if( m_nCurPosition < m_strOriginalString.GetLength() && m_strOriginalString[m_nCurPosition] == token )
	{
		//当前将扫描的字符与所期望的一致
		m_nCurPosition++;
		return;
	}
	else
	{
		throw CError("输入出错!");
		return;
	}
}