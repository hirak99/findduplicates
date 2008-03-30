; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CFindDuplicatesDlg
LastTemplate=CWinThread
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "Find Duplicates.h"

ClassCount=4
Class1=CFindDuplicatesApp
Class2=CFindDuplicatesDlg
Class3=CAboutDlg

ResourceCount=5
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Class4=CWorkerThread
Resource3=IDR_MENUPOPUP
Resource4=IDD_FINDDUPLICATES_DIALOG
Resource5=IDR_MENUMAIN

[CLS:CFindDuplicatesApp]
Type=0
HeaderFile=Find Duplicates.h
ImplementationFile=Find Duplicates.cpp
Filter=N

[CLS:CFindDuplicatesDlg]
Type=0
HeaderFile=Find DuplicatesDlg.h
ImplementationFile=Find DuplicatesDlg.cpp
Filter=C
BaseClass=CDialog
VirtualFilter=dWC
LastObject=IDC_CHK_REMOVEFOLDERS

[CLS:CAboutDlg]
Type=0
HeaderFile=Find DuplicatesDlg.h
ImplementationFile=Find DuplicatesDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=IDC_EDITINFO

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_EDITINFO,edit,1342248964

[DLG:IDD_FINDDUPLICATES_DIALOG]
Type=1
Class=CFindDuplicatesDlg
ControlCount=23
Control1=IDC_LIST_FOLDERS,SysListView32,1350795801
Control2=IDC_ADD_FOLDER,button,1342373888
Control3=IDC_REMOVE_FOLDER,button,1476460544
Control4=IDC_GRP_OPTIONS,button,1342308359
Control5=IDC_CHK_SUBDIRS,button,1342373891
Control6=IDC_CHK_ZEROFILES,button,1342242819
Control7=IDC_CHK_HIDDEN,button,1342242819
Control8=IDC_CHK_REMOVEFOLDERS,button,1342242819
Control9=IDC_START_SCAN,button,1476460545
Control10=IDC_FILL_SCREEN,button,1342410755
Control11=IDC_LBL_RESULTS,static,1342308364
Control12=IDLST_RESULTS,SysListView32,1350796293
Control13=IDST_INFO,static,1342308364
Control14=IDC_DESELECT_ALL,button,1342373888
Control15=IDC_AUTO_SELECT,button,1342242816
Control16=IDC_SELECT_IN_FOLDER,button,1342242816
Control17=IDC_WHAT_IF,button,1342373888
Control18=IDC_DELETE,button,1342242816
Control19=IDC_MOVE,button,1342242816
Control20=IDCANCEL,button,1342242816
Control21=IDC_ANIMATE_SCAN,SysAnimate32,1073807362
Control22=IDPRG_COMPARE,msctls_progress32,1082130432
Control23=IDC_START_SELECTIVE,button,1476460544

[CLS:CWorkerThread]
Type=0
HeaderFile=WorkerThread.h
ImplementationFile=WorkerThread.cpp
BaseClass=CWinThread
Filter=N

[MNU:IDR_MENUPOPUP]
Type=1
Class=CFindDuplicatesDlg
Command1=IDPOP_SAVERESULTS
Command2=IDPOP_LOADRESULTS
Command3=IDPOP_CLEARRESULTS
Command4=IDPOP_UPDATELIST
CommandCount=4

[MNU:IDR_MENUMAIN]
Type=1
Class=CFindDuplicatesDlg
Command1=ID_INTEGRATE
Command2=ID_EXPORTHTML
Command3=ID_HELP_ABOUT
CommandCount=3

