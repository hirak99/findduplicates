// Find DuplicatesDlg.cpp : implementation file
//

#include "stdafx.h"
#include <fstream.h>
#include "Find Duplicates.h"
#include "Find DuplicatesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Globals
inline char ToUpper(char c) {return (c>='a' && c<='z') ? c+('A'-'a') : c;}
bool BeginsWith(const char *Str2,const char *Str1)
{
	const char *i,*j;
	bool bResult=true;
	for (i=Str1,j=Str2; bResult && *i!=0 && *j!=0; i++,j++)
		if (ToUpper(*i)!=ToUpper(*j)) bResult=false;
	return (bResult && *i==0);
}
bool _fastcall FileExists(const char *File)
{
	WIN32_FIND_DATA wfd;
	ZeroMemory(&wfd,sizeof wfd);
	HANDLE Find=FindFirstFile(File,&wfd);
	if (Find!=INVALID_HANDLE_VALUE)
	{
		FindClose(Find);
		return true;
	}
	else return false;
}
CString PluralString(int n,CString name) {
	CString Msg; Msg.Format("%i",abs(n));
	for(int pos=3;Msg.GetLength()>pos;pos+=4)
		Msg=Msg.Left(Msg.GetLength()-pos)+","+Msg.Right(pos);
	if (n<0) Msg="-"+Msg;
	Msg=Msg+" "+name;
	if (n>1 || n<-1) Msg+="s";
	return Msg;
}

/////////////////////////////////////////////////////////////////////////////
// FileInfo structure

void FileInfo::RestoreTime()
{
	FILETIME c,w;
	HANDLE File=CreateFile(Name,GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL|FILE_FLAG_NO_BUFFERING,NULL);
	GetFileTime(File,&c,NULL,&w);
	SetFileTime(File,&c,&ftLastAccess,&w);
	CloseHandle(File);
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CEdit	m_edtInfo;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_EDITINFO, m_edtInfo);
	//}}AFX_DATA_MAP
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_edtInfo.SetWindowText("This comes alone, and as a part of 'My Uninstaller'. "
			"Please check geocities.com/hirak_99 for more information."
			"\r\n\r\nPS. My email addr is hirak99@gmail.com, I like nice emails. I don't dislike e-cheques or cash either ;o)");
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindDuplicatesDlg dialog

int CFindDuplicatesDlg::m_nDeniedFiles;
bool CFindDuplicatesDlg::m_bThreadIsRunning;
bool CFindDuplicatesDlg::m_bThreadTerminateNow;

CFindDuplicatesDlg::CFindDuplicatesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFindDuplicatesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFindDuplicatesDlg)
	m_chkSubdirs = TRUE;
	m_chkZeroFiles = FALSE;
	m_chkHidden = FALSE;
	m_chkRemoveFolders = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	EnableToolTips();
}

void CFindDuplicatesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindDuplicatesDlg)
	DDX_Control(pDX, IDPRG_COMPARE, m_prgCompare);
	DDX_Control(pDX, IDC_FILL_SCREEN, m_chkFillScreen);
	DDX_Control(pDX, IDC_ANIMATE_SCAN, m_Animate);
	DDX_Control(pDX, IDLST_RESULTS, m_lstResults);
	DDX_Control(pDX, IDST_INFO, m_stInfo);
	DDX_Control(pDX, IDC_REMOVE_FOLDER, m_btnRemoveFolder);
	DDX_Control(pDX, IDC_LIST_FOLDERS, m_lstFolders);
	DDX_Check(pDX, IDC_CHK_SUBDIRS, m_chkSubdirs);
	DDX_Check(pDX, IDC_CHK_ZEROFILES, m_chkZeroFiles);
	DDX_Check(pDX, IDC_CHK_HIDDEN, m_chkHidden);
	DDX_Check(pDX, IDC_CHK_REMOVEFOLDERS, m_chkRemoveFolders);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFindDuplicatesDlg, CDialog)
	//{{AFX_MSG_MAP(CFindDuplicatesDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_ADD_FOLDER, OnAddFolder)
	ON_BN_CLICKED(IDC_REMOVE_FOLDER, OnRemoveFolder)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_FOLDERS, OnEndlabeleditListFolders)
	ON_BN_CLICKED(IDC_START_SCAN, OnStartScan)
	ON_WM_DRAWITEM()
	ON_NOTIFY(NM_CLICK, IDLST_RESULTS, OnClickResults)
	ON_NOTIFY(NM_DBLCLK, IDLST_RESULTS, OnDblclkResults)
	ON_BN_CLICKED(IDC_DESELECT_ALL, OnDeselectAll)
	ON_BN_CLICKED(IDC_AUTO_SELECT, OnAutoSelect)
	ON_BN_CLICKED(IDC_SELECT_IN_FOLDER, OnSelectInFolder)
	ON_BN_CLICKED(IDC_WHAT_IF, OnWhatIf)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_FILL_SCREEN, OnFillScreen)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(IDPOP_SAVERESULTS, OnSaveResults)
	ON_COMMAND(IDPOP_LOADRESULTS, OnLoadResults)
	ON_COMMAND(IDPOP_CLEARRESULTS, OnClearResults)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_FOLDERS, OnItemchangedListFolders)
	ON_COMMAND(IDPOP_UPDATELIST, OnUpdateList)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_FOLDERS, ListFoldersOnKeydown)
	ON_BN_CLICKED(IDC_START_SELECTIVE, OnStartSelective)
	ON_COMMAND(ID_HELP_ABOUT, OnHelpAbout)
	ON_COMMAND(ID_INTEGRATE, OnIntegrateShell)
	ON_COMMAND(ID_EXPORTHTML, OnExportHTML)
	ON_WM_MENUSELECT()
	ON_BN_CLICKED(IDC_MOVE, OnMoveFile)
	ON_WM_DROPFILES()
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX(TTN_NEEDTEXT,0,OnToolTipText)	// from MSDN
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindDuplicatesDlg message handlers

BOOL CFindDuplicatesDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	LVCOLUMN lvc;
	ZeroMemory(&lvc,sizeof lvc);
	lvc.mask=LVCF_TEXT | LVCF_WIDTH;
	lvc.cx=200; lvc.pszText="Folder"; lvc.cchTextMax=6;
	m_lstFolders.InsertColumn(0,&lvc);
	m_lstFolders.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	lvc.cx=400; lvc.pszText="File"; lvc.cchTextMax=4;
	m_lstResults.InsertColumn(0,&lvc);
	lvc.cx=70; lvc.pszText="Size"; lvc.cchTextMax=4;
	lvc.mask=LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvc.fmt=LVCFMT_RIGHT;
	m_lstResults.InsertColumn(1,&lvc);
	m_lstResults.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES | LVS_EX_TRACKSELECT);
	DragAcceptFiles(TRUE);

 	// To check/uncheck the Integrate to Shell menu
 	HKEY hkey;
 	if (RegOpenKey(HKEY_CLASSES_ROOT,
 		"Folder\\Shell\\FindDuplicates\\Command",&hkey)==ERROR_SUCCESS)
 	{
 		RegCloseKey(hkey);
 		GetMenu()->CheckMenuItem(ID_INTEGRATE,MF_BYCOMMAND|MF_CHECKED);
 	}
 	else GetMenu()->CheckMenuItem(ID_INTEGRATE,MF_BYCOMMAND|MF_UNCHECKED);

	// Now to parse command line
	char *CmdLine=new char[strlen(GetCommandLine())+1];
	strcpy(CmdLine,GetCommandLine());
	char *start=CmdLine;
	int nCmdFileCount=0;
	while (*start!=0)
	{
		char *end=start;
		if (*end=='"')
			do end++; while(*end!='"' && *end!=0);
		else
			while (*end!=' ' && *end!=0) end++;
		char Temp=*end; *end=0;
		if (*start=='"') start++;
		if (nCmdFileCount++>0)
		{
			CString FileName=start;
			if (start[strlen(start)-1]!='\\') FileName=FileName+"\\";
			m_lstFolders.InsertItem(0,FileName);
		}
		*end=Temp; start=end;
		if (*start=='"') start++;
		while (*start==' ') start++;
	}
	delete CmdLine;
#ifdef _DEBUG
	m_lstFolders.InsertItem(0,"C:\\Arnab\\New Folder\\");
	nCmdFileCount++;
#endif
	if (nCmdFileCount>1)
	{
		GetDlgItem(IDC_START_SCAN)->EnableWindow();
		GetDlgItem(IDC_START_SELECTIVE)->EnableWindow();
		m_btnRemoveFolder.EnableWindow();
	}
	m_Animate.Open(IDR_AVI1);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFindDuplicatesDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFindDuplicatesDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CFindDuplicatesDlg::OnAddFolder() 
{
	// TODO: Add your control notification handler code here
	bool bAdded=true;
	if (GetKeyState(VK_SHIFT) & 0x10000)
	{
		m_lstFolders.InsertItem(0,"<new folder>");
		m_lstFolders.SetFocus();
		m_lstFolders.EditLabel(0);
	}
	else
	{
		char *szPath=BrowseFolder();
		if (*szPath!=0)
		{
			if (szPath[strlen(szPath)-1]!='\\')
				strcat(szPath,"\\");
			m_lstFolders.InsertItem(m_lstFolders.GetItemCount(),szPath);
		}
		else bAdded=false;
	}
	if (bAdded)
	{
		m_btnRemoveFolder.EnableWindow(true);
		GetDlgItem(IDC_START_SCAN)->EnableWindow(!m_bThreadIsRunning);
		GetDlgItem(IDC_START_SELECTIVE)->EnableWindow(!m_bThreadIsRunning);
	}
}

void CFindDuplicatesDlg::OnRemoveFolder() 
{
	// TODO: Add your control notification handler code here
	if (GetKeyState(VK_SHIFT) & 0x10000) m_lstFolders.DeleteAllItems();
	else while (true) {
		POSITION pos=m_lstFolders.GetFirstSelectedItemPosition();
		if (pos==NULL) break;
		m_lstFolders.DeleteItem(m_lstFolders.GetNextSelectedItem(pos));
	}
	if (m_lstFolders.GetItemCount()==0)
	{
		m_btnRemoveFolder.EnableWindow(false);
		GetDlgItem(IDC_START_SCAN)->EnableWindow(false);
		GetDlgItem(IDC_START_SELECTIVE)->EnableWindow(false);
	}
}

void CFindDuplicatesDlg::OnEndlabeleditListFolders(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	char *Text=pDispInfo->item.pszText;
	if (Text!=NULL && Text[0]!=0)
	{
		if (Text[strlen(Text)-1]!='\\')
			strcat(Text,"\\");
		*pResult = 1;
	}
}

void CFindDuplicatesDlg::OnStartScan()
{
	// TODO: Add your control notification handler code here
	m_bSelectiveCompare=false;
	GetDlgItem(IDC_LBL_RESULTS)->SetWindowText("Comparison &Results:");
	// For some reason, UpdateData does not seem to work from within the other threads!
	UpdateData(TRUE);
	AfxBeginThread(Worker,this);
}

void CFindDuplicatesDlg::GetFiles(CFilesPtrList &Files,
	CString Folder,bool ZerosAlso,bool Recurse,bool Hidden)
{
	WIN32_FIND_DATA wfd;
	ZeroMemory(&wfd,sizeof wfd);
	HANDLE Find=FindFirstFile(Folder+"*.*",&wfd);
	m_stInfo.SetWindowText(CString("Now scanning ")+Folder);
	if (Find!=INVALID_HANDLE_VALUE)
	{
		bool bSomeLargeFiles=false;
		do
		{
			if (wfd.cFileName[0]=='.') continue;
			if (!Hidden && (wfd.dwFileAttributes &
				(FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM)))
				continue;
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (Recurse)
				{
					GetFiles(Files,Folder+wfd.cFileName+"\\",ZerosAlso,Recurse,Hidden);
					m_stInfo.SetWindowText(CString("Now scanning ")+Folder);
				}
			}
			else 
			{
				if (wfd.nFileSizeHigh>0)
				{
					bSomeLargeFiles=true;
					continue;
				}
				if (!ZerosAlso && wfd.nFileSizeLow==0) continue;
				FileInfo *fi=new FileInfo;
				fi->Name=Folder+wfd.cFileName;
				fi->nSize=wfd.nFileSizeLow;
				fi->ftLastAccess=wfd.ftLastAccessTime;
				POSITION pos;
				for(pos=Files.GetTailPosition();pos!=NULL;Files.GetPrev(pos))
				{
					FileInfo *f2=Files.GetAt(pos);
					// The inequality sign below determines which files should be compared before
					if (fi->nSize < f2->nSize) break;
					if (fi->nSize == f2->nSize && strcmpi(fi->Name,f2->Name)>0) break;
				}
				if (pos==NULL) Files.AddHead(fi);
				else Files.InsertAfter(pos,fi);
			}
		} while (!m_bThreadTerminateNow && FindNextFile(Find,&wfd));
		FindClose(Find);
		if (bSomeLargeFiles) MessageBox("Some files were discarded because they are larger than 4GB.",
			"Warning",MB_OK|MB_ICONWARNING);
	}
}

bool CFindDuplicatesDlg::FilesAreSame(FileInfo *F1,FileInfo *F2,bool DontTouch)
{
	static char Buf1[512],Buf2[512];
	UINT nRead;
	bool bAreSame,bOpen1,bOpen2;
	CFile f1,f2;
	bOpen1 = (f1.Open(F1->Name,CFile::modeRead|CFile::typeBinary|CFile::shareDenyWrite)==TRUE);
	if (bOpen1)
		bOpen2 = (f2.Open(F2->Name,CFile::modeRead|CFile::typeBinary|CFile::shareDenyWrite)==TRUE);
	else bOpen2=false;

	if (bOpen1 && bOpen2)	// if both could be opened
	{
		bAreSame=true;
		do {
			nRead=f1.Read(Buf1,512);
			if (f2.Read(Buf2,512)!=nRead) bAreSame=false;
			else if(memcmp(Buf1,Buf2,nRead)) bAreSame=false;
		} while(nRead==512 && bAreSame && !m_bThreadTerminateNow);
	}
	else {bAreSame=false; m_nDeniedFiles++;}

	if (DontTouch)
	{
		if (bOpen1) {f1.Close(); F1->RestoreTime();}
		if (bOpen2) {f2.Close(); F2->RestoreTime();}
	}

	return bAreSame && !m_bThreadTerminateNow;
}

void CFindDuplicatesDlg::AddFileToList(FileInfo *fi,int GroupNo)
{
	char Buffer[11];	// Since support is given for size < 4GB
	int n=m_lstResults.InsertItem(m_lstResults.GetItemCount(),fi->Name);
	m_lstResults.SetItemData(n,GroupNo);
	itoa(fi->nSize,Buffer,10);
	m_lstResults.SetItem(n,1,LVIF_TEXT,Buffer,0,0,0,0);
	//m_lstResults.SetCheck(n,Check);
	if (m_stInfo.IsWindowVisible()) m_stInfo.SetWindowText(GetResultsSummary());
}

void CFindDuplicatesDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDI) 
{
	// TODO: Add your message handler code here and/or call default
	if (nIDCtl==IDLST_RESULTS)
	{
		RECT &rc=lpDI->rcItem;
		CDC *dc=CDC::FromHandle(lpDI->hDC);
		int nCol=(lpDI->itemData)%4;
		if (nCol==0) dc->FillRect(&rc,&CBrush(0xE0FFFF));
		else if (nCol==1) dc->FillRect(&rc,&CBrush(0xFFFFE0));
		else if (nCol==2) dc->FillRect(&rc,&CBrush(0xFFD0FF));
		else dc->FillRect(&rc,&CBrush(0xE0FFE0));
		COLORREF color;
		if (m_lstResults.GetCheck(lpDI->itemID)) color=0x7F7F7F;
		else color=0;
		if (lpDI->itemState & ODS_SELECTED) color+=0x800000;
		dc->SetTextColor(color);
		CRect Rect;
		CString Str=m_lstResults.GetItemText(lpDI->itemID,0);
		m_lstResults.GetSubItemRect(lpDI->itemID,0,LVIR_LABEL,Rect);
		Rect.left=5;
		dc->ExtTextOut(rc.left+5,rc.top,ETO_CLIPPED,Rect,Str,NULL);
		m_lstResults.GetSubItemRect(lpDI->itemID,1,LVIR_LABEL,Rect);
		Str=m_lstResults.GetItemText(lpDI->itemID,1);
		dc->ExtTextOut(Rect.right-m_lstResults.GetStringWidth(Str)-5,Rect.top,
			ETO_CLIPPED,Rect,Str,NULL);
		if (m_WhatIfList.GetSize()>0)
			if (int(lpDI->itemID+1)!=m_lstResults.GetItemCount() &&
				m_lstResults.GetItemData(lpDI->itemID+1)!=lpDI->itemData)
			{
				CPen Pen(PS_SOLID,1,RGB(128,128,128)),*pOldPen=dc->SelectObject(&Pen);
				dc->MoveTo(rc.left,rc.bottom-1);
				dc->LineTo(rc.right,rc.bottom-1);
				dc->SelectObject(pOldPen);
			}
	}
	else CDialog::OnDrawItem(nIDCtl, lpDI);
}

void CFindDuplicatesDlg::OnClickResults(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NMLISTVIEW *pNML=(NMLISTVIEW *)pNMHDR;
	if (pNML->iItem!=-1)
	{
		int n=pNML->iItem;
		UINT nGrp=m_lstResults.GetItemData(n);
		m_lstResults.SetCheck(n,!m_lstResults.GetCheck(n));
		while (n>=0 && m_lstResults.GetItemData(n)==nGrp) n--;
		bool AllChecked=true;
		for (n++; AllChecked && n<m_lstResults.GetItemCount() && m_lstResults.GetItemData(n)==nGrp; n++)
			if (m_lstResults.GetCheck(n)==FALSE) AllChecked=false;
		if (AllChecked) MessageBox("You have marked all files in the group for deletion!","Warning",MB_OK|MB_ICONWARNING);
		m_stInfo.SetWindowText(GetResultsSummary());
	}
	*pResult = 0;
}

void CFindDuplicatesDlg::OnDblclkResults(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnClickResults(pNMHDR,pResult);
}

UINT CFindDuplicatesDlg::Worker(LPVOID lParam)
{
	if (m_bThreadIsRunning) return 1;
	m_bThreadIsRunning=true; m_bThreadTerminateNow=false;
	CFindDuplicatesDlg *This=(CFindDuplicatesDlg *)lParam;
	This->m_Animate.Play(0,14,-1);
	This->m_Animate.ShowWindow(SW_SHOW);
	This->OnSize(SIZE_RESTORED,0,0);
	This->m_nDeniedFiles=0;
	This->ScanNCompare();
	This->m_Animate.ShowWindow(SW_HIDE);
	This->m_Animate.Stop();
	m_bThreadIsRunning=false;
	This->OnSize(SIZE_RESTORED,0,0);
	if (This->m_nDeniedFiles) 
		This->MessageBox("Some files could not be compared because they could not be opened.",
			"Warning",MB_OK|MB_ICONWARNING);
	return 0;
}

void CFindDuplicatesDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	if (!m_bThreadIsRunning) CDialog::OnCancel();
	else m_bThreadTerminateNow=true;
}

void CFindDuplicatesDlg::OnDeselectAll() 
{
	// TODO: Add your control notification handler code here
	int n=m_lstResults.GetItemCount();
	if (n>0)
	{
		for (int i=0; i<n; i++)
			m_lstResults.SetCheck(i,FALSE);
		m_stInfo.SetWindowText(GetResultsSummary());
	}
}

void CFindDuplicatesDlg::OnAutoSelect() 
{
	// Automatically selects duplicate files to remove
	int n=m_lstResults.GetItemCount();
	if (n>0)
	{
		int PrevGrp=-1,nFirstChecked=-1;
		for (int i=0; i<n; i++)
		{
			int Grp=m_lstResults.GetItemData(i);
			if (Grp!=PrevGrp)
			{
				if (nFirstChecked!=-1) m_lstResults.SetCheck(nFirstChecked,FALSE);
				PrevGrp=Grp;
				nFirstChecked=-1;
			}
			if (m_lstResults.GetCheck(i)==FALSE)
			{
				if (nFirstChecked==-1) nFirstChecked=i;
				m_lstResults.SetCheck(i,TRUE);
			}
		}
		if (nFirstChecked!=-1) m_lstResults.SetCheck(nFirstChecked,FALSE);
		m_stInfo.SetWindowText(GetResultsSummary());
	}
}

CString CFindDuplicatesDlg::GetResultsSummary()
{
	int nTotalLength=0,nTotalFiles=0,nTotalErasedL=0,nTotalErasedF=0;
	int PrevGrp=-1,nLength;
	for (int i=0; i<m_lstResults.GetItemCount(); i++)
	{
		int Grp=m_lstResults.GetItemData(i);
		if (Grp!=PrevGrp)
		{
			nLength=atoi(m_lstResults.GetItemText(i,1));
			PrevGrp=Grp;
		}
		else 
		{
			nTotalLength+=nLength;
			nTotalFiles++;
		}
		if (m_lstResults.GetCheck(i)) 
		{
			nTotalErasedL+=nLength;
			nTotalErasedF++;
		}
	}
	int nResultLength=nTotalLength-nTotalErasedL,nResultFiles=nTotalFiles-nTotalErasedF;
	CString Msg=CString("Wasted ")
		+PluralString(nTotalLength,"byte")+" in "
		+PluralString(nTotalFiles,"file")+" - "
		+PluralString(nTotalErasedL,"byte")+" in "
		+PluralString(nTotalErasedF,"file")+" = "
		+PluralString(nResultLength,"byte")+" in "
		+PluralString(nResultFiles,"file")+".";
	return Msg;
}

int CALLBACK CFindDuplicatesDlg::BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lp, LPARAM pData) 
{
	static TCHAR szDir[MAX_PATH];
	switch(uMsg) {
		case BFFM_INITIALIZED:
			if (*szDir!=0) 
			{
				// WParam is TRUE since you are passing a path.
				// It would be FALSE if you were passing a pidl.
				::SendMessage(hwnd,BFFM_SETSELECTION,TRUE,(LPARAM)szDir);
			}
			break;
		case BFFM_SELCHANGED: 
			{
				// Set the status window to the currently selected path.
				TCHAR newDir[MAX_PATH];
				SHGetPathFromIDList((LPITEMIDLIST) lp, newDir);
				if (*newDir!=0) strcpy(szDir,newDir);
			}
			break;
		default:
			break;
	}
	return 0;
}

#define BIF_USENEWUI 0x40
char *CFindDuplicatesDlg::BrowseFolder(bool bRememberFolder,char* szTitle)
{
	static char szPath[MAX_PATH];
	LPMALLOC pMalloc;
	if (SHGetMalloc(&pMalloc)==E_FAIL) *szPath=0;
	else
	{
		LPITEMIDLIST pidl;
		BROWSEINFO bi;
		ZeroMemory(&bi,sizeof bi);
		bi.hwndOwner=m_hWnd;
		bi.lpszTitle=szTitle;
		bi.ulFlags=BIF_RETURNONLYFSDIRS | BIF_EDITBOX;
		if (bRememberFolder) bi.lpfn=BrowseCallbackProc;
		else bi.ulFlags |= BIF_USENEWUI;
		pidl=SHBrowseForFolder(&bi);
		if (pidl)
		{
			SHGetPathFromIDList(pidl,szPath);
			pMalloc->Free(pidl);
		}
		else *szPath=0;
		pMalloc->Release();
	}
	return szPath;
}

void CFindDuplicatesDlg::ScanNCompare()
{
	CFilesPtrList Files,Files2;
	GetLocalTime(&m_TimeStarted);
	//SetClassLong(m_hWnd,GCL_HCURSOR,(LONG)LoadCursor(NULL,IDC_APPSTARTING));
	GetDlgItem(IDC_ADD_FOLDER)->EnableWindow(false);
	GetDlgItem(IDC_REMOVE_FOLDER)->EnableWindow(false);
	GetDlgItem(IDC_START_SCAN)->EnableWindow(false);
	GetDlgItem(IDC_START_SELECTIVE)->EnableWindow(false);
	GetDlgItem(IDC_WHAT_IF)->EnableWindow(false);
	GetDlgItem(IDC_DELETE)->EnableWindow(false);
	GetDlgItem(IDC_MOVE)->EnableWindow(false);
	GetDlgItem(IDC_CHK_SUBDIRS)->EnableWindow(false);
	GetDlgItem(IDC_CHK_ZEROFILES)->EnableWindow(false);
	GetDlgItem(IDC_CHK_HIDDEN)->EnableWindow(false);
	GetDlgItem(IDCANCEL)->SetWindowText("&Stop Scan");
	m_WhatIfList.RemoveAll();
	GetDlgItem(IDC_WHAT_IF)->SetWindowText("&What If");
	m_lstResults.DeleteAllItems();
	for (int i=0; !m_bThreadTerminateNow && i<m_lstFolders.GetItemCount(); i++)
	{
		CFilesPtrList *pWhich;
		CString FileName=m_lstFolders.GetItemText(i,0);
		if (i>0)
		{
			CString PrevName=m_lstFolders.GetItemText(i-1,0);
			if (BeginsWith(FileName,PrevName))
			{
				bool bPrevSel=(m_lstFolders.GetItemState(i-1,LVIS_SELECTED)!=0);
				m_lstFolders.SetItemState(i,bPrevSel?LVIS_SELECTED:0,LVIS_SELECTED);
				continue;
			}
		}
		if (m_bSelectiveCompare)
		{
			if (IsSelected(FileName)) pWhich=&Files2;
			else pWhich=&Files;
		}
		else pWhich=&Files;
		GetFiles(*pWhich,FileName,m_chkZeroFiles!=0,m_chkSubdirs!=0,m_chkHidden!=0);
	}
	if (!m_bSelectiveCompare)
	{
		GetDlgItem(IDC_ADD_FOLDER)->EnableWindow(true);
		GetDlgItem(IDC_REMOVE_FOLDER)->EnableWindow(true);
	}
	bool bDontCompare=false;
	if (m_bThreadTerminateNow)
	{
		m_Animate.Play(15,15,0);
		m_bThreadTerminateNow=false;
		m_stInfo.SetWindowText("Scanning stopped due to user interruption.");
		if (MessageBox("Scanning stopped in the middle due to user interruption!\n"
			"Generated list might not be a full list.",
			"Warning",MB_OKCANCEL|MB_ICONWARNING)==IDCANCEL)
			bDontCompare=true;
		else m_Animate.Play(0,14,-1);
	}
	if (!bDontCompare)
	{
		GetDlgItem(IDCANCEL)->SetWindowText("&Stop Compare");
		m_prgCompare.SetRange(0,1); m_prgCompare.SetPos(0);
		m_stInfo.ShowWindow(SW_HIDE);
		m_prgCompare.ShowWindow(SW_SHOW);
		if (!m_bSelectiveCompare) CompareFiles1(Files);
		else CompareFiles2(Files,Files2);
		if (m_bThreadTerminateNow)
		{
			m_Animate.Play(15,15,0);
			MessageBox("Comparing stopped in the middle due to user interruption!\n"
				"Generated list might not be a full list.","Warning",MB_OK|MB_ICONWARNING);
		}
		m_prgCompare.ShowWindow(SW_HIDE);
		m_stInfo.ShowWindow(SW_SHOW);
		CString Message;
		Message.Format("Scanned %s%i file%s. ",
			m_bThreadTerminateNow ? "some of ":"",
			Files.GetCount()+Files2.GetCount(),
			Files.GetCount()+Files2.GetCount()>1 ? "s":"");
		if (m_lstResults.GetItemCount()==0) m_stInfo.SetWindowText(Message+"No duplicate files found.");
		else m_stInfo.SetWindowText(Message+GetResultsSummary());
	}
	while (!Files.IsEmpty())
	{
		delete Files.GetHead();
		Files.RemoveHead();
	}
	while (!Files2.IsEmpty())
	{
		delete Files2.GetHead();
		Files2.RemoveHead();
	}
	GetDlgItem(IDC_START_SCAN)->EnableWindow(m_lstFolders.GetItemCount()>0);
	GetDlgItem(IDC_START_SELECTIVE)->EnableWindow(m_lstFolders.GetItemCount()>0);
	GetDlgItem(IDC_WHAT_IF)->EnableWindow(true);
	GetDlgItem(IDC_DELETE)->EnableWindow(true);
	GetDlgItem(IDC_MOVE)->EnableWindow(true);
	GetDlgItem(IDC_CHK_SUBDIRS)->EnableWindow(true);
	GetDlgItem(IDC_CHK_ZEROFILES)->EnableWindow(true);
	GetDlgItem(IDC_CHK_HIDDEN)->EnableWindow(true);
	GetDlgItem(IDCANCEL)->SetWindowText("&Close");
	if (m_bSelectiveCompare)
	{
		GetDlgItem(IDC_ADD_FOLDER)->EnableWindow(true);
		GetDlgItem(IDC_REMOVE_FOLDER)->EnableWindow(true);
		m_lstFolders.EnableWindow(true);
	}
	//SetClassLong(m_hWnd,GCL_HCURSOR,(LONG)LoadCursor(NULL,IDC_ARROW));
}

void CFindDuplicatesDlg::OnSelectInFolder() 
{
	// TODO: Add your control notification handler code here
	int n=m_lstResults.GetItemCount();
	bool bFromList=false;
	CString Folder;
	if (GetKeyState(VK_SHIFT) & 0x10000) bFromList=true;
	else Folder=BrowseFolder();
	if (bFromList || (n>0 && !Folder.IsEmpty()))
	{
		int PrevGrp=-1,nLastChecked;
		bool bAllChecked=false;
		for (int i=0; i<n; i++)
		{
			int Grp=m_lstResults.GetItemData(i);
			if (Grp!=PrevGrp)
			{
				if (bAllChecked==true && nLastChecked!=-1)
					m_lstResults.SetCheck(nLastChecked,FALSE);
				PrevGrp=Grp;
				bAllChecked=true;
				nLastChecked=-1;
			}
			CString Name=m_lstResults.GetItemText(i,0);
			bool bNeedToCheck=false;
			if (bFromList) bNeedToCheck=IsSelected(Name);
			else bNeedToCheck=BeginsWith(Name,Folder);
			if (bNeedToCheck)
			{
				m_lstResults.SetCheck(i,TRUE);
				nLastChecked=i;
			}
			if (m_lstResults.GetCheck(i)==FALSE) bAllChecked=false;
		}
		if (bAllChecked==true && nLastChecked!=-1)
			m_lstResults.SetCheck(nLastChecked,FALSE);
		m_stInfo.SetWindowText(GetResultsSummary());
	}
}

void CFindDuplicatesDlg::OnWhatIf() 
{
	// TODO: Add your control notification handler code here
	if (m_WhatIfList.GetSize()==0)
	{
		// Carry out "WhatIf"
		CResults result;
		int n=m_lstResults.GetItemCount();
		if (n==0) return;
		int nUnchecked,nPrevGrp=-1,nGrpStart;
		m_lstResults.SetRedraw(FALSE);
		for (int i=0; ; i++)
		{
			int nGrp=(i>=n)?-1:m_lstResults.GetItemData(i);
			if (nGrp!=nPrevGrp) 
			{
				if (nPrevGrp!=-1)
					for (int j=nGrpStart; j<i; j++)
						if (nUnchecked<=1 || m_lstResults.GetCheck(j))
						{
							result.Name=m_lstResults.GetItemText(j,0);
							result.nGroup=nPrevGrp;
							result.nPos=j;
							result.nSize=atoi(m_lstResults.GetItemText(j,1));
							result.bChecked=(m_lstResults.GetCheck(j)==TRUE);
							m_WhatIfList.Add(result);
							m_lstResults.DeleteItem(j);
							i--; j--; n--;
						}
				nPrevGrp=nGrp;
				nUnchecked=0; nGrpStart=i;
				if (i>=n) break;
			}
			if (m_lstResults.GetCheck(i)==FALSE) nUnchecked++;
		}
		m_lstResults.SetRedraw(TRUE);
		if (m_WhatIfList.GetSize()>0)
		{
			GetDlgItem(IDC_DELETE)->EnableWindow(false);
			GetDlgItem(IDC_MOVE)->EnableWindow(false);
			GetDlgItem(IDC_WHAT_IF)->SetWindowText("Un &Whatif");
			m_lstResults.Invalidate(FALSE);
		}
	}
	else
	{
		// Carry out "Un-WhatIf"
		m_lstResults.SetRedraw(FALSE);
		for (int i=m_WhatIfList.GetSize()-1; i>=0; i--)
		{
			CResults *result=m_WhatIfList.GetData()+i;
			char Buffer[11];	// Since support is given for size < 4GB
			itoa(result->nSize,Buffer,10);
			int n=m_lstResults.InsertItem(result->nPos,result->Name);
			m_lstResults.SetItemData(n,result->nGroup);
			m_lstResults.SetItem(n,1,LVIF_TEXT,Buffer,0,0,0,0);
			m_lstResults.SetCheck(n,result->bChecked);
		}
		m_WhatIfList.RemoveAll();
		GetDlgItem(IDC_DELETE)->EnableWindow(true);
		GetDlgItem(IDC_MOVE)->EnableWindow(true);
		GetDlgItem(IDC_WHAT_IF)->SetWindowText("&What If");
		m_lstResults.SetRedraw(TRUE);
		m_lstResults.Invalidate(FALSE);
	}
	m_stInfo.SetWindowText(GetResultsSummary());
}

void CFindDuplicatesDlg::OnDelete() 
{
	// TODO: Add your control notification handler code here
	MoveOrDeleteTo(false);
}

void CFindDuplicatesDlg::OnSize(UINT nType, int cx, int cy) 
{
	if (cx==0 || cy==0)
	{
		CRect rc;
		GetClientRect(&rc);
		cx=rc.right; cy=rc.bottom;
	}
	else CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	// Since UpdateData() does not work from another thread,,,
	bool m_bFillScreen=(m_chkFillScreen.GetState() & 1);
	int nTop=m_bFillScreen?11:135;
	int nLeft=cx-63; if (nLeft<115) nLeft=115;
	GetDlgItem(IDC_FILL_SCREEN)->MoveWindow(nLeft,nTop,53,16);
	GetDlgItem(IDC_LBL_RESULTS)->MoveWindow(11,nTop+3,nLeft-16,13);
	m_lstResults.MoveWindow(11,nTop+18,nLeft+42,cy-213+(m_bFillScreen?124:0));
	if (cy-56<nTop+23) nTop=nTop+23; else nTop=cy-56;
	if (m_bThreadIsRunning) m_Animate.MoveWindow(11,nTop,16,16);
	m_stInfo.MoveWindow(m_bThreadIsRunning?27:11,nTop,cx-(m_bThreadIsRunning?36:20),13);
	m_prgCompare.MoveWindow(28,nTop,cx-37,13);
	GetDlgItem(IDC_DESELECT_ALL)->MoveWindow(11,nTop+22,65,23);
	GetDlgItem(IDC_AUTO_SELECT)->MoveWindow(79,nTop+22,65,23);
	GetDlgItem(IDC_SELECT_IN_FOLDER)->MoveWindow(147,nTop+22,75,23);
	nLeft=cx-262; if (nLeft<235) nLeft=235;
	GetDlgItem(IDC_WHAT_IF)->MoveWindow(nLeft,nTop+22,65,23);
	GetDlgItem(IDC_DELETE)->MoveWindow(nLeft+68,nTop+22,65,23);
	GetDlgItem(IDC_MOVE)->MoveWindow(nLeft+134,nTop+22,50,23);
	GetDlgItem(IDCANCEL)->MoveWindow(nLeft+187,nTop+22,65,23);
	Invalidate(FALSE);
}

void CFindDuplicatesDlg::OnFillScreen() 
{
	// TODO: Add your control notification handler code here
	int CmdShow=(m_chkFillScreen.GetCheck() & 1)?SW_HIDE:SW_SHOW;
	m_lstFolders.ShowWindow(CmdShow);
	GetDlgItem(IDC_ADD_FOLDER)->ShowWindow(CmdShow);
	GetDlgItem(IDC_REMOVE_FOLDER)->ShowWindow(CmdShow);
	GetDlgItem(IDC_CHK_SUBDIRS)->ShowWindow(CmdShow);
	GetDlgItem(IDC_CHK_ZEROFILES)->ShowWindow(CmdShow);
	GetDlgItem(IDC_CHK_HIDDEN)->ShowWindow(CmdShow);
	GetDlgItem(IDC_CHK_REMOVEFOLDERS)->ShowWindow(CmdShow);
	GetDlgItem(IDC_START_SCAN)->ShowWindow(CmdShow);
	GetDlgItem(IDC_START_SELECTIVE)->ShowWindow(CmdShow);
	GetDlgItem(IDC_GRP_OPTIONS)->ShowWindow(CmdShow);
	OnSize(SIZE_RESTORED,0,0);
}

void CFindDuplicatesDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
	if (pWnd->m_hWnd==m_lstResults.m_hWnd)
	{
		CMenu menu;
		if (menu.LoadMenu(IDR_MENUPOPUP))
		{
			menu.EnableMenuItem(IDPOP_SAVERESULTS,MF_BYCOMMAND | 
				((m_bThreadIsRunning || m_lstResults.GetItemCount()==0 || m_WhatIfList.GetSize()) ? MF_GRAYED : MF_ENABLED));
			menu.EnableMenuItem(IDPOP_LOADRESULTS,MF_BYCOMMAND | 
				(m_bThreadIsRunning ? MF_GRAYED : MF_ENABLED));
			menu.EnableMenuItem(IDPOP_CLEARRESULTS,MF_BYCOMMAND | 
				((m_bThreadIsRunning || m_lstResults.GetItemCount()==0 || m_WhatIfList.GetSize()) ? MF_GRAYED : MF_ENABLED));
			menu.EnableMenuItem(IDPOP_UPDATELIST,MF_BYCOMMAND | 
				((m_bThreadIsRunning || m_lstResults.GetItemCount()==0 || m_WhatIfList.GetSize()) ? MF_GRAYED : MF_ENABLED));
			menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,
				point.x,point.y,AfxGetMainWnd());
		}
	}
}

void CFindDuplicatesDlg::OnSaveResults() 
{
	// TODO: Add your command handler code here
	CFileDialog savedlg(FALSE,"dup",NULL,
		OFN_HIDEREADONLY | OFN_NOREADONLYRETURN | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST,
		"Find Duplicates results (*.dup)|*.dup|All Files (*.*)|*.*||");
	if (savedlg.DoModal()==IDOK)
	{
		CFile file;
		if (file.Open(savedlg.GetFileName(),CFile::modeCreate | CFile::modeWrite))
		{
			SetCursor(LoadCursor(NULL,IDC_WAIT));
			int n=m_lstResults.GetItemCount();
			file.Write(&n,sizeof(n));
			for (int i=0; i<n; i++)
			{
				int nTemp;
				CString Name=m_lstResults.GetItemText(i,0);
				nTemp=Name.GetLength();
				file.Write(&nTemp,sizeof nTemp);
				file.Write((LPCSTR)Name,nTemp);
				nTemp=m_lstResults.GetItemData(i);
				file.Write(&nTemp,sizeof nTemp);
				nTemp=atoi(m_lstResults.GetItemText(i,1));
				file.Write(&nTemp,sizeof nTemp);
				nTemp=m_lstResults.GetCheck(i);
				file.Write(&nTemp,sizeof nTemp);
			}
			file.Write(&m_TimeStarted,sizeof(SYSTEMTIME));
			SetCursor(LoadCursor(NULL,IDC_ARROW));
		}
		else MessageBox("Unable to create specified file.","Error",MB_ICONSTOP | MB_OK);
	}
}

void CFindDuplicatesDlg::OnLoadResults() 
{
	// TODO: Add your command handler code here
	CFileDialog loaddlg(TRUE,"dup",NULL,
		OFN_FILEMUSTEXIST,
		"Find Duplicates results (*.dup)|*.dup|All Files (*.*)|*.*||");
	if (loaddlg.DoModal()==IDOK)
	{
		CFile file;
		if (file.Open(loaddlg.GetFileName(),CFile::modeRead))
		{
			SetCursor(LoadCursor(NULL,IDC_WAIT));
			int n;
			file.Read(&n,sizeof n);
			m_WhatIfList.RemoveAll();
			GetDlgItem(IDC_WHAT_IF)->SetWindowText("&What If");
			m_lstResults.DeleteAllItems();
			m_stInfo.ShowWindow(SW_HIDE);
			for (int i=0; i<n; i++)
			{
				int nTemp;
				file.Read(&nTemp,sizeof nTemp);
				char *pName=new char[nTemp+1];
				file.Read(pName,nTemp); pName[nTemp]=0;
				m_lstResults.InsertItem(i,pName);
				delete[] pName;
				file.Read(&nTemp,sizeof nTemp);
				m_lstResults.SetItemData(i,nTemp);
				char Size[11];
				file.Read(&nTemp,sizeof nTemp);
				itoa(nTemp,Size,10);
				m_lstResults.SetItemText(i,1,Size);
				file.Read(&nTemp,sizeof nTemp);
				m_lstResults.SetCheck(i,nTemp);
			}
			file.Read(&m_TimeStarted,sizeof(SYSTEMTIME));
			m_stInfo.ShowWindow(SW_SHOW);
			m_stInfo.SetWindowText(GetResultsSummary());
			GetDlgItem(IDC_LBL_RESULTS)->SetWindowText("Loaded &Results:");
			SetCursor(LoadCursor(NULL,IDC_ARROW));
		}
		else MessageBox("Unable to access specified file.","Error",MB_ICONSTOP | MB_OK);
	}
}

void CFindDuplicatesDlg::OnClearResults() 
{
	// TODO: Add your command handler code here
	GetDlgItem(IDC_LBL_RESULTS)->SetWindowText("Comparison &Results:");
	m_lstResults.DeleteAllItems();
	m_stInfo.SetWindowText("");
}

bool CFindDuplicatesDlg::IsSelected(const char *pFileName)
{
	bool bResult=false;
	int nFileNameLen=strlen(pFileName);
	char pItemText[MAX_PATH];
	POSITION pos=m_lstFolders.GetFirstSelectedItemPosition();
	while (!bResult && pos!=NULL) {
		int nItem=m_lstFolders.GetNextSelectedItem(pos);
		int nItemTextLen=m_lstFolders.GetItemText(nItem,0,pItemText,MAX_PATH);
		if (nItemTextLen<=nFileNameLen)
			bResult=BeginsWith(pFileName,pItemText);
		/*{
			char cPrev=pFileName[nItemTextLen];
			(char)pFileName[nItemTextLen]=0;
			if (strcmpi(pItemText,pFileName)==0) bResult=true;
			(char)pFileName[nItemTextLen]=cPrev;
		}*/
	}
	return bResult;
}

void CFindDuplicatesDlg::CompareFiles1(CFilesPtrList &Files)
{
	POSITION pos=Files.GetHeadPosition();
	int nGroups=-1,nItems=0;
	m_prgCompare.SetRange(0,Files.GetCount());
	while (!m_bThreadTerminateNow && pos!=NULL)
	{
		int Count=0;
		FileInfo *fi=Files.GetNext(pos);
		m_prgCompare.SetPos(++nItems);
		POSITION pos2=pos;
		while (!m_bThreadTerminateNow && pos2!=NULL)
		{
			FileInfo *fi2=Files.GetAt(pos2);
			if (fi->nSize!=fi2->nSize) break;
			if (FilesAreSame(fi,fi2,true))
			{
				if (Count==0) AddFileToList(fi,++nGroups);
				AddFileToList(fi2,nGroups);
				FileInfo *t=Files.GetAt(pos);
				Files.SetAt(pos,fi2); Files.SetAt(pos2,t);
				Files.GetNext(pos);
				m_prgCompare.SetPos(++nItems);
				Count++;
			}
			Files.GetNext(pos2);
		}
	}
}

int CFindDuplicatesDlg::ScanSameSize(CFilesPtrList &Files1,CFilesPtrList &Files2,
	POSITION pos1,POSITION pos2)
{
	// pos1 and pos2 should specify FileInfo's with same nSize in Files1 and Files2 respectively.
	//  This function then pulls together all files in Files2 which are same as file at pos1 in
	//  File1 and returns howmany are same.
	int Count=0;
	FileInfo *fi1=Files1.GetAt(pos1),*fi2;
	for (POSITION j=pos2; j!=NULL; Files2.GetNext(j))
	{
		fi2=Files2.GetAt(j);
		if (fi2->nSize!=fi1->nSize) break;
		if (FilesAreSame(fi1,fi2,true))
		{
			FileInfo *t=Files2.GetAt(pos2);
			Files2.SetAt(pos2,fi2); Files2.SetAt(j,t);
			Files2.GetNext(pos2); Count++;
		}
	}
	return Count;
}

void CFindDuplicatesDlg::CompareFiles2(CFilesPtrList &Files1,CFilesPtrList &Files2)
{
	int nGroups=0,nProgress=0;
	POSITION pos1=Files1.GetHeadPosition(),pos2=Files2.GetHeadPosition();
	m_prgCompare.SetRange(0,Files1.GetCount()+Files2.GetCount());
	while (pos1!=NULL && pos2!=NULL && !m_bThreadTerminateNow)
	{
		unsigned nSize1=Files1.GetAt(pos1)->nSize;
		unsigned nSize2=Files2.GetAt(pos2)->nSize;
		if (nSize1>nSize2) Files1.GetNext(pos1);
		else if (nSize1<nSize2) Files2.GetNext(pos2);
		else
		{
			int nSame2=ScanSameSize(Files1,Files2,pos1,pos2);
			if (nSame2==0) Files1.GetNext(pos1);
			else
			{
				POSITION t=pos1; Files1.GetNext(t);
				int nSame1=ScanSameSize(Files2,Files1,pos2,t)+1;
				nGroups++;
				for (int i=0; i<nSame1; i++)
				{
					FileInfo *fi1=Files1.GetNext(pos1);
					AddFileToList(fi1,nGroups);
				}
				for (i=0; i<nSame2; i++)
				{
					FileInfo *fi2=Files2.GetNext(pos2);
					AddFileToList(fi2,nGroups);
				}
				nProgress+=nSame1+nSame2-1;
			}
		}
		m_prgCompare.SetPos(++nProgress);
	}
}

void CFindDuplicatesDlg::OnItemchangedListFolders(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	static bool bInside=false;
	if (pNMListView->uChanged==LVIF_TEXT && !bInside)
	{
		bInside=true;
		for (int i=0; i<m_lstFolders.GetItemCount()-1; i++)
		{
			CString S1=m_lstFolders.GetItemText(i,0);
			for (int j=i+1; j<m_lstFolders.GetItemCount(); j++)
			{
				CString S2=m_lstFolders.GetItemText(j,0);
				if (strcmpi(S1,S2)>0)
				{
					m_lstFolders.SetItemText(j,0,S1);
					S1=S2;
				}
			}
			m_lstFolders.SetItemText(i,0,S1);
		}
		bInside=false;
	}
	*pResult = 0;
}

int CFindDuplicatesDlg::ClearErased()
{
	// Returns number of files deleted
	if (m_WhatIfList.GetSize()>0) return 0;	// This function requires OnWhatIf()
	int n=m_lstResults.GetItemCount(),nCountDel=0;
	bool bInfoVisible=(m_stInfo.IsWindowVisible()==TRUE);
	if (bInfoVisible) m_stInfo.ShowWindow(SW_HIDE);
	for (int i=0; i<n; i++)
	{
		m_lstResults.SetItemState(i,m_lstResults.GetCheck(i)?LVIS_CUT:0,LVIS_CUT);
		CString File=m_lstResults.GetItemText(i,0);
		bool bDeleted=!FileExists(File);
		m_lstResults.SetCheck(i,bDeleted);
		if (bDeleted) nCountDel++;
	}
	OnWhatIf(); m_WhatIfList.RemoveAll(); 
	GetDlgItem(IDC_DELETE)->EnableWindow();
	GetDlgItem(IDC_MOVE)->EnableWindow();
	GetDlgItem(IDC_WHAT_IF)->SetWindowText("&What If");
	n=m_lstResults.GetItemCount();
	int NewGrp=-1,PrevGrp=-1;
	for (i=0; i<n; i++)
	{
		int Grp=m_lstResults.GetItemData(i);
		if (Grp!=PrevGrp)
		{
			PrevGrp=Grp;
			NewGrp++;
		}
		m_lstResults.SetItemData(i,NewGrp);
		m_lstResults.SetCheck(i,m_lstResults.GetItemState(i,LVIS_CUT));
		m_lstResults.SetItemState(i,0,LVIS_CUT);
	}
	if (bInfoVisible) m_stInfo.ShowWindow(SW_SHOW);
	return nCountDel;
}

void CFindDuplicatesDlg::OnUpdateList() 
{
	// TODO: Add your command handler code here
	SetCursor(LoadCursor(NULL,IDC_WAIT));
	int nDeleted=ClearErased();
	CString Msg;
	if (nDeleted==0) Msg="All of the listed files still exist.";
	else Msg.Format("%i file%s erased since last check ",
		nDeleted,nDeleted<=1?" was":"s were");
	m_stInfo.SetWindowText(Msg);
	SetCursor(LoadCursor(NULL,IDC_ARROW));
}

void CFindDuplicatesDlg::ListFoldersOnKeydown(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	if (pLVKeyDow->wVKey==VK_F2)
	{
		POSITION pos=m_lstFolders.GetFirstSelectedItemPosition();
		if (pos!=NULL)
		{
			int nItem=m_lstFolders.GetNextSelectedItem(pos);
			m_lstFolders.EditLabel(nItem);
		}
	}
	else if (pLVKeyDow->wVKey==VK_DELETE)
		OnRemoveFolder();
	*pResult = 0;
}

void CFindDuplicatesDlg::WinHelp(DWORD dwData, UINT nCmd) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	// This is necessary to prevent trying to open .hlp file on F1
	//CDialog::WinHelp(dwData, nCmd);
}

void CFindDuplicatesDlg::OnStartSelective() 
{
	// TODO: Add your control notification handler code here
	bool SomeSel=false,SomeUnSel=false;
	for (int i=0; i<m_lstFolders.GetItemCount() && !(SomeSel && SomeUnSel); i++)
		if (m_lstFolders.GetItemState(i,LVIS_SELECTED)) SomeSel=true;
		else SomeUnSel=true;
	if (!SomeSel || !SomeUnSel)
	{
		CString Msg;
		Msg.Format("Unable to perform selective comparison (between selected and unselected folders) since all folders are %s.",
			!SomeUnSel ? "selected" : "unselected");
		MessageBox(Msg,"Warning",MB_OK|MB_ICONWARNING);
		return;
	}
	m_bSelectiveCompare=true;
	m_lstFolders.EnableWindow(false);
	GetDlgItem(IDC_ADD_FOLDER)->EnableWindow(false);
	GetDlgItem(IDC_REMOVE_FOLDER)->EnableWindow(false);
	GetDlgItem(IDC_LBL_RESULTS)->SetWindowText("Selective Comparison &Results:");
	// For some reason, UpdateData does not seem to work from within the other threads!
	UpdateData(TRUE);
	AfxBeginThread(Worker,this);
}

BOOL CFindDuplicatesDlg::OnToolTipText(UINT id, NMHDR *pNMHDR, LRESULT *pResult)
{
    TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;
    UINT nID =pNMHDR->idFrom;
    if (pTTT->uFlags & TTF_IDISHWND)
    {
        // idFrom is actually the HWND of the tool
        nID = ::GetDlgCtrlID((HWND)nID);
        if(nID)
        {
            pTTT->lpszText = MAKEINTRESOURCE(nID);
            pTTT->hinst = AfxGetResourceHandle();
			//CString msg;
			//msg.LoadString(nID);
			//m_stInfo.SetWindowText(msg);
            return(TRUE);
        }
	}
    return TRUE;
}

void CFindDuplicatesDlg::OnHelpAbout() 
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

void CFindDuplicatesDlg::OnIntegrateShell() 
{
	if (GetMenu()->GetMenuState(ID_INTEGRATE,MF_BYCOMMAND) & MF_CHECKED) {
		RegDeleteKey(HKEY_CLASSES_ROOT,"Folder\\Shell\\FindDuplicates\\Command");
		RegDeleteKey(HKEY_CLASSES_ROOT,"Folder\\Shell\\FindDuplicates");
 		GetMenu()->CheckMenuItem(ID_INTEGRATE,MF_BYCOMMAND|MF_UNCHECKED);
	}
	else {
		HKEY hkey,hkcmd;
		RegCreateKey(HKEY_CLASSES_ROOT,"Folder\\Shell\\FindDuplicates",&hkey);
		RegSetValueEx(hkey,"",0,REG_SZ,(CONST BYTE *)"Find Duplicates",16);
		RegCreateKey(hkey,"Command",&hkcmd);
		HMODULE thismodule=GetModuleHandle(NULL);
		char Command[MAX_PATH+7]; Command[0]='"';
		GetModuleFileName(thismodule,Command+1,MAX_PATH-1);
		strcat(Command,"\" \"%1\"");
		RegSetValueEx(hkcmd,"",0,REG_SZ,(CONST BYTE *)Command,strlen(Command));
		RegCloseKey(hkey); RegCloseKey(hkcmd);
 		GetMenu()->CheckMenuItem(ID_INTEGRATE,MF_BYCOMMAND|MF_CHECKED);
	}
}

void CFindDuplicatesDlg::OnExportHTML() 
{
	// TODO: Add your command handler code here
	const char *szMonths[]={"January","February","March","April","May","June",
		"July","August","September","October","November","December"};
	CFileDialog savedlg(FALSE,"html",NULL,
		OFN_HIDEREADONLY | OFN_NOREADONLYRETURN | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST,
		"HTML files (*.html;*.htm)|*.html;*.htm|All Files (*.*)|*.*||");
	if (savedlg.DoModal()==IDOK)
	{
		SetCursor(LoadCursor(NULL,IDC_WAIT));
		ofstream file(savedlg.GetFileName());
		file<<"<!doctype html public \"-//W3C//DTD HTML 4.0 Transitional//EN\">\n"
			"<html>\n"
			"<head>\n"
			"<title>Find Duplicates Results</title>\n"
			"</head>\n"
			"<body>\n"
			"<p>This comparison result was generated by <a href=\"http://www.geocities.com/hirak_99/goodies/finddups.html\">Find Duplicates</a>.";
		CString TimeStr;
		TimeStr.Format("<p>The following duplicate files were found on %02d-%s-%4d, at %02d:%02d.</p>\n",
			m_TimeStarted.wDay,szMonths[m_TimeStarted.wMonth-1],m_TimeStarted.wYear,
			m_TimeStarted.wHour,m_TimeStarted.wMinute);
		file<<TimeStr;
		file<<"<table bgcolor=#e0e0e0>\n"
			"<col align=left><col align=right>\n"
			"<tr><th>Filename</th><th>Size</th></tr>\n";
		int n=m_lstResults.GetItemCount();
		for (int i=0; i<n; i++)
		{
			int nCol=m_lstResults.GetItemData(i)%4;
			CString bgColor,fgColor;
			switch(nCol) {
				case 0: bgColor="#e0ffff"; break;
				case 1: bgColor="#ffffe0"; break;
				case 2: bgColor="#ffd0ff"; break;
				case 3: bgColor="#e0ffe0"; break;
			};
			if (m_lstResults.GetCheck(i)) fgColor="#7f7f7f"; else fgColor="#000000";
			file<<"<tr style=\"color:"<<fgColor<<"\" bgcolor="<<bgColor<<"><td>"<<m_lstResults.GetItemText(i,0)<<"</td>";
			file<<"<td>"<<m_lstResults.GetItemText(i,1)<<"</td></tr>";
			//nTemp=m_lstResults.GetCheck(i);
			//file.Write(&nTemp,sizeof nTemp);
		}
		file<<"</table>\n";
		file<<"<p>"<<GetResultsSummary()<<"</p>";
		file<<"</body>\n"
			"</html>\n";
		SetCursor(LoadCursor(NULL,IDC_ARROW));
	}
}

void CFindDuplicatesDlg::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu) 
{
	CDialog::OnMenuSelect(nItemID, nFlags, hSysMenu);
	
	// TODO: Add your message handler code here
	GetMenu()->EnableMenuItem(ID_EXPORTHTML,MF_BYCOMMAND | 
		((m_bThreadIsRunning || m_lstResults.GetItemCount()==0 || m_WhatIfList.GetSize()) ? MF_GRAYED : MF_ENABLED));
}

void CFindDuplicatesDlg::MoveOrDeleteTo(bool bMove)
{
	int n=m_lstResults.GetItemCount();
	if (n>0)
	{
		UpdateData(TRUE);
		CString temp;
		bool bExtinction=false;
		int PrevGrp=-1,nSelCount=0;
		bool bAllChecked;
		for (int i=0; i<n && !bExtinction; i++)
		{
			int Grp=m_lstResults.GetItemData(i);
			if (Grp!=PrevGrp)
			{
				if (PrevGrp!=-1 && bAllChecked) bExtinction=true;
				PrevGrp=Grp;
				bAllChecked=true;
			}
			if (m_lstResults.GetCheck(i)==FALSE) bAllChecked=false;
		}
		if (PrevGrp!=-1 && bAllChecked) bExtinction=true;
		if (bExtinction)
		{
			temp.Format("In one or more groups you have marked every file for %s. Proceed?",bMove?"moving":"deletion");
			if (MessageBox(temp,"Warning",MB_YESNO|MB_ICONWARNING)==IDNO)
				return;
		}
		CString ToDelete;
		CArray<CString,CString> Folders;
		for (i=0; i<n; i++)
			if (m_lstResults.GetCheck(i))
			{
				nSelCount++;
				CString File=m_lstResults.GetItemText(i,0);
				if (FileExists(File)) ToDelete+=File+"|";
				CString ThisFolder=File.Left(File.ReverseFind('\\'));
				bool bAlreadyThere=false;
				for (int i=0; i<Folders.GetSize(); ++i)
					if (Folders[i]==ThisFolder) {bAlreadyThere=true; break;}
				if (!bAlreadyThere) Folders.Add(ThisFolder);
			}
		if (nSelCount==0) 
		{
			temp.Format("There is no file selected to %s!",bMove?"move":"delete");
			m_stInfo.SetWindowText(temp);
			return;
		}
		SetCursor(LoadCursor(NULL,IDC_WAIT));
		n=ToDelete.GetLength();
		char *Buffer=ToDelete.GetBuffer(n);
		SHFILEOPSTRUCT shf; ZeroMemory(&shf,sizeof shf);
		if (bMove) {
			shf.pTo=BrowseFolder(false,"Choose a folder to move files to:");
			if (*(shf.pTo)==0) return;
			temp.Format("Do you want to move the selected files to \"%s\"?",shf.pTo);
			if(MessageBox(temp,"Confirmation",MB_YESNO|MB_ICONQUESTION)!=IDYES) return;
		}
		for (i=0; i<n; i++)
			if (Buffer[i]=='|') Buffer[i]=0;
		shf.hwnd=m_hWnd;
		shf.wFunc=bMove?FO_MOVE:FO_DELETE;
		shf.pFrom=Buffer;
		if (bMove || (~GetKeyState(VK_SHIFT) & 0x10000)) shf.fFlags=FOF_ALLOWUNDO;
		shf.lpszProgressTitle="Find Duplicates";
		if (SHFileOperation(&shf)==0 && !shf.fAnyOperationsAborted)
		{
			ToDelete.ReleaseBuffer(0);
			SetCursor(LoadCursor(NULL,IDC_WAIT));
			nSelCount-=ClearErased();
			if (m_chkRemoveFolders)
				for (int i=0; i<Folders.GetSize(); ++i)
					RemoveDirectory(Folders[i]);
			SetCursor(LoadCursor(NULL,IDC_ARROW));
			if (nSelCount==0) ToDelete="All selected files were deleted.";
			else ToDelete.Format("Could not delete %i file%s.",nSelCount,nSelCount<=1?"":"s");
			m_stInfo.SetWindowText(ToDelete);
		}
	}
}

void CFindDuplicatesDlg::OnMoveFile() 
{
	// TODO: Add your control notification handler code here
	MoveOrDeleteTo(true);
}

void CFindDuplicatesDlg::OnDropFiles(HDROP hDropInfo) 
{
	// TODO: Add your message handler code here and/or call default

	bool bAdded=false;
	int nFiles=DragQueryFile(hDropInfo,0xFFFFFFFF,NULL,0);
	for (int i=0; i<nFiles; i++) {
		char szPath[MAX_PATH];
		DragQueryFile(hDropInfo,i,szPath,MAX_PATH);
		DWORD dwAttribs=GetFileAttributes(szPath);
		if (dwAttribs!=0xFFFFFFFF && dwAttribs &
			(FILE_ATTRIBUTE_DIRECTORY & dwAttribs)) {
			int nLen=strlen(szPath);
			if (szPath[nLen-1]!='\\') {
				szPath[nLen+1]=0; szPath[nLen]='\\';
			}
			m_lstFolders.InsertItem(m_lstFolders.GetItemCount(),szPath);
		}
		bAdded=true;
	}
	DragFinish(hDropInfo);
	if (bAdded)
	{
		m_btnRemoveFolder.EnableWindow(true);
		GetDlgItem(IDC_START_SCAN)->EnableWindow(!m_bThreadIsRunning);
		GetDlgItem(IDC_START_SELECTIVE)->EnableWindow(!m_bThreadIsRunning);
	}

	//CDialog::OnDropFiles(hDropInfo);
}
