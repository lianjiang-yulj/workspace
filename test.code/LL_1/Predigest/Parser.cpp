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
	m_strOriginalString = originalString;//�����û�������ַ�����׼������
	m_nCurPosition = 0;
	CString preDigestionString = _T("");
	try
	{
		preDigestionString = Expression();//�����õ����봮�Ļ�����ʽ
	}
	catch(CError& error)
	{
		return error.GetErrorMessage();
	}
	//����m_nCurPosition��ֵ���ж��û��������Ƿ�����
	return ( m_nCurPosition == m_strOriginalString.GetLength() ) ? preDigestionString : CString("�������!");
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
			//���Ҳ��������в�ˣ���������ʵ��
			CList<CString, CString&> L_List, R_List;
			//�����Ҳ������ԡ�+�����зָ����������
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
					//��ʾ�Ѿ�������ʽ�����һ����
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
			//�ַ�������+���ָ�ɹ������ڽ��в��
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
			//ȥ�����һ����+����
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
	//�����һ���ַ��ǵ����ַ������ʽ���󷵻أ�����Ƕ�׽��б��ʽ�ķ���
	try
	{
		if( IsLetter(m_strOriginalString[m_nCurPosition]) )
		{
			temp.Format("%c", m_strOriginalString[m_nCurPosition]);
			Match(m_strOriginalString[m_nCurPosition]);
		}
		else if( m_strOriginalString[m_nCurPosition] == '[' )
		{
			//�ؼ�
			Match('[');
			temp = Expression();
			Match(']');
		}
		else
		{
			//����
			CError error("�������!");
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
		//��ǰ��ɨ����ַ�����������һ��
		m_nCurPosition++;
		return;
	}
	else
	{
		throw CError("�������!");
		return;
	}
}