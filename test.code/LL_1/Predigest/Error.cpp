// Error.cpp: implementation of the CError class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Predigest.h"
#include "Error.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CError::CError()
{
	m_strErrorMsg = _T("");
}

CError::CError(const CError& other)
{
//	memcpy(this, &other, sizeof(CError));
	m_strErrorMsg = other.m_strErrorMsg;
}

CError::CError(const CString& errorMsg)
{
	m_strErrorMsg = errorMsg;
}

CError::~CError()
{

}

CString CError::GetErrorMessage()
{
	return m_strErrorMsg;
}