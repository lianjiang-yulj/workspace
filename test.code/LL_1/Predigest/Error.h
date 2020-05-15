// Error.h: interface for the CError class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ERROR_H__FA49E5E4_4000_4C6C_9F81_AAE3C941B8A7__INCLUDED_)
#define AFX_ERROR_H__FA49E5E4_4000_4C6C_9F81_AAE3C941B8A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CError : public CException  
{
public:
	CError(const CString& errorMsg);
	CError(const CError& other);
	CError();
	CString GetErrorMessage();//得到出错消息
	virtual ~CError();

protected:
	CString m_strErrorMsg;//描述出错信息
};

#endif // !defined(AFX_ERROR_H__FA49E5E4_4000_4C6C_9F81_AAE3C941B8A7__INCLUDED_)
