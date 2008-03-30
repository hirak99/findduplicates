// Find DuplicatesDlg.h : header file
//

#if !defined(AFX_FINDDUPLICATESDLG_H__AF39E746_2948_11D6_854D_B78C2665342E__INCLUDED_)
#define AFX_FINDDUPLICATESDLG_H__AF39E746_2948_11D6_854D_B78C2665342E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// FileInfo structure
struct FileInfo
{
	CString Name;
	DWORD nSize;
	FILETIME ftLastAccess;
	void RestoreTime();
	int nTag;
};

/////////////////////////////////////////////////////////////////////////////
// CFindDuplicatesDlg dialog

typedef CTypedPtrList<CPtrList,FileInfo*> CFilesPtrList;
class CFindDuplicatesDlg : public CDialog
{
// Construction
public:
	CFindDuplicatesDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CFindDuplicatesDlg)
	enum { IDD = IDD_FINDDUPLICATES_DIALOG };
	CProgressCtrl	m_prgCompare;
	CButton	m_chkFillScreen;
	CAnimateCtrl	m_Animate;
	CListCtrl	m_lstResults;
	CStatic	m_stInfo;
	CButton	m_btnRemoveFolder;
	CListCtrl	m_lstFolders;
	BOOL	m_chkSubdirs;
	BOOL	m_chkZeroFiles;
	BOOL	m_chkHidden;
	BOOL	m_chkRemoveFolders;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindDuplicatesDlg)
	public:
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void MoveOrDeleteTo(bool bMove);
	static int m_nDeniedFiles;	// Incremented by FilesAreSame() if can't open files
	static bool m_bThreadIsRunning,m_bThreadTerminateNow;
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CFindDuplicatesDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnAddFolder();
	afx_msg void OnRemoveFolder();
	afx_msg void OnEndlabeleditListFolders(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnStartScan();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnClickResults(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkResults(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnCancel();
	afx_msg void OnDeselectAll();
	afx_msg void OnAutoSelect();
	afx_msg void OnSelectInFolder();
	afx_msg void OnWhatIf();
	afx_msg void OnDelete();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFillScreen();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSaveResults();
	afx_msg void OnLoadResults();
	afx_msg void OnClearResults();
	afx_msg void OnItemchangedListFolders(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateList();
	afx_msg void ListFoldersOnKeydown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnStartSelective();
	afx_msg void OnHelpAbout();
	afx_msg void OnIntegrateShell();
	afx_msg void OnExportHTML();
	afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
	afx_msg void OnMoveFile();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL OnToolTipText(UINT id,NMHDR *pTTTStruct,LRESULT *pResult);
	int ClearErased();
	void CompareFiles1(CFilesPtrList &Files);
	int ScanSameSize(CFilesPtrList &Files1,CFilesPtrList &Files2,POSITION pos1,POSITION pos2);
	void CompareFiles2(CFilesPtrList &Files1,CFilesPtrList &Files2);
	bool IsSelected(const char *pFileName);
	struct CResults {
		CString Name;
		int nSize,nGroup;
		int nPos;
		bool bChecked;
	};
	CArray<CResults,CResults&> m_WhatIfList;
	bool m_bSelectiveCompare;
	void ScanNCompare();
	static int CALLBACK BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lp, LPARAM pData);
	char *BrowseFolder(bool bRememberFolder=true,char* szTitle="Choose a folder:");
	CString GetResultsSummary();
	static UINT Worker(LPVOID lParam);
	void AddFileToList(FileInfo *fi,int GroupNo);
	static bool FilesAreSame(FileInfo *F1,FileInfo *F2,bool DontTouch);
	void GetFiles(CTypedPtrList<CPtrList,FileInfo*> &Files,
		CString Folder,bool ZerosAlso,bool Recurse,bool Hidden);
	SYSTEMTIME m_TimeStarted;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDDUPLICATESDLG_H__AF39E746_2948_11D6_854D_B78C2665342E__INCLUDED_)
