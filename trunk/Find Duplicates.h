// Find Duplicates.h : main header file for the FIND DUPLICATES application
//

#if !defined(AFX_FINDDUPLICATES_H__AF39E744_2948_11D6_854D_B78C2665342E__INCLUDED_)
#define AFX_FINDDUPLICATES_H__AF39E744_2948_11D6_854D_B78C2665342E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CFindDuplicatesApp:
// See Find Duplicates.cpp for the implementation of this class
//

class CFindDuplicatesApp : public CWinApp
{
public:
	CFindDuplicatesApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindDuplicatesApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CFindDuplicatesApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDDUPLICATES_H__AF39E744_2948_11D6_854D_B78C2665342E__INCLUDED_)
