; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CFillView
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "Fill.h"
LastPage=0

ClassCount=6
Class1=CFillApp
Class2=CFillDoc
Class3=CFillView
Class4=CMainFrame

ResourceCount=3
Resource1=IDR_MAINFRAME
Resource2=IDD_ABOUTBOX
Class5=CAboutDlg
Class6=CTrigle
Resource3=IDD_DIALOG1

[CLS:CFillApp]
Type=0
HeaderFile=Fill.h
ImplementationFile=Fill.cpp
Filter=N

[CLS:CFillDoc]
Type=0
HeaderFile=FillDoc.h
ImplementationFile=FillDoc.cpp
Filter=N

[CLS:CFillView]
Type=0
HeaderFile=FillView.h
ImplementationFile=FillView.cpp
Filter=C
BaseClass=CView
VirtualFilter=VWC
LastObject=IDM_TRI


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
LastObject=IDM_TRI




[CLS:CAboutDlg]
Type=0
HeaderFile=Fill.cpp
ImplementationFile=Fill.cpp
Filter=D
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_SAVE_AS
Command5=ID_FILE_PRINT
Command6=ID_FILE_PRINT_PREVIEW
Command7=ID_FILE_PRINT_SETUP
Command8=ID_FILE_MRU_FILE1
Command9=ID_APP_EXIT
Command10=ID_EDIT_UNDO
Command11=ID_EDIT_CUT
Command12=ID_EDIT_COPY
Command13=ID_EDIT_PASTE
Command14=ID_VIEW_TOOLBAR
Command15=ID_VIEW_STATUS_BAR
Command16=IDM_TRI
Command17=ID_APP_ABOUT
CommandCount=17

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE
CommandCount=14

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
Command8=ID_APP_ABOUT
CommandCount=8

[DLG:IDD_DIALOG1]
Type=1
Class=CTrigle
ControlCount=8
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_X1,edit,1350631552
Control4=IDC_Y1,edit,1350631552
Control5=IDC_X2,edit,1350631552
Control6=IDC_Y2,edit,1350631552
Control7=IDC_X3,edit,1350631552
Control8=IDC_Y3,edit,1350631552

[CLS:CTrigle]
Type=0
HeaderFile=Trigle.h
ImplementationFile=Trigle.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC

