; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CDrawLineView
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "DrawLine.h"
LastPage=0

ClassCount=7
Class1=CDrawLineApp
Class2=CDrawLineDoc
Class3=CDrawLineView
Class4=CMainFrame

ResourceCount=5
Resource1=IDD_ABOUTBOX
Class5=CAboutDlg
Resource2=IDD_DLGCIRCLE
Class6=CDlgPoints
Resource3=IDR_MAINFRAME
Class7=CCircleOrds
Resource4=IDD_DLGCORDS
Resource5=IDD_DIALOG1

[CLS:CDrawLineApp]
Type=0
HeaderFile=DrawLine.h
ImplementationFile=DrawLine.cpp
Filter=N

[CLS:CDrawLineDoc]
Type=0
HeaderFile=DrawLineDoc.h
ImplementationFile=DrawLineDoc.cpp
Filter=N

[CLS:CDrawLineView]
Type=0
HeaderFile=DrawLineView.h
ImplementationFile=DrawLineView.cpp
Filter=C
BaseClass=CView
VirtualFilter=VWC
LastObject=IDM_CIRCLE


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
LastObject=IDM_DLGPOINTS




[CLS:CAboutDlg]
Type=0
HeaderFile=DrawLine.cpp
ImplementationFile=DrawLine.cpp
Filter=D

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
Command16=IDM_DLGPOINTS
Command17=IDM_CIRCLE
Command18=ID_APP_ABOUT
CommandCount=18

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
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
Command8=ID_APP_ABOUT
CommandCount=8

[DLG:IDD_DLGCORDS]
Type=1
Class=CDlgPoints
ControlCount=8
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_EDTSX,edit,1350631552
Control5=IDC_STATIC,static,1342308352
Control6=IDC_EDTSY,edit,1350631552
Control7=IDC_EDTEX,edit,1350631552
Control8=IDC_EDTEY,edit,1350631552

[CLS:CDlgPoints]
Type=0
HeaderFile=DlgPoints.h
ImplementationFile=DlgPoints.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDM_DLGPOINTS

[DLG:IDD_DLGCIRCLE]
Type=1
Class=CCircleOrds
ControlCount=7
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_CX,edit,1350631552
Control5=IDC_CY,edit,1350631552
Control6=IDC_STATIC,static,1342308352
Control7=IDC_R,edit,1350631552

[CLS:CCircleOrds]
Type=0
HeaderFile=CircleOrds.h
ImplementationFile=CircleOrds.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDC_CX

[DLG:IDD_DIALOG1]
Type=1
Class=?
ControlCount=8
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_EDIT1,edit,1350631552
Control5=IDC_EDIT2,edit,1350631552
Control6=IDC_EDIT3,edit,1350631552
Control7=IDC_EDIT4,edit,1350631552
Control8=IDC_STATIC,static,1342308352

