; CLW ファイルは MFC ClassWizard の情報を含んでいます。

[General Info]
Version=1
ClassCount=4
Class1=CTmpFavGroupApp
LastClass=CBaseWindow
NewFileInclude2=#include "TmpFavGroup.h"
ResourceCount=4
NewFileInclude1=#include "stdafx.h"
Class2=CTmpFavTreeView
LastTemplate=CDialog
Class3=CBaseWindow
Resource1=IDR_GROUP
Resource2=IDR_MENU
Class4=CExStyleDialog
Resource3=IDR_ITEM
Resource4=IDD_STYLE

[CLS:CTmpFavGroupApp]
Type=0
HeaderFile=TmpFavGroup.h
ImplementationFile=TmpFavGroup.cpp
Filter=N
BaseClass=CWinApp
VirtualFilter=AC
LastObject=CTmpFavGroupApp

[CLS:CTmpFavTreeView]
Type=0
HeaderFile=TmpFavTreeView.h
ImplementationFile=TmpFavTreeView.cpp
BaseClass=CTreeCtrl
Filter=W
VirtualFilter=GWC
LastObject=CTmpFavTreeView

[CLS:CBaseWindow]
Type=0
HeaderFile=BaseWindow.h
ImplementationFile=BaseWindow.cpp
BaseClass=CWnd
Filter=C
VirtualFilter=WC
LastObject=ID_RENAME_GROUP

[MNU:IDR_MENU]
Type=1
Class=?
Command1=ID_ADD_TMPGROUP
Command2=ID_ADDITEMTOGROUP
Command3=ID_OPENANDCLOSE
Command4=ID_OPEN_ACTIVE
Command5=ID_OPEN_DBLCLICK
Command6=ID_RELOAD_FOLDER
CommandCount=6

[MNU:IDR_GROUP]
Type=1
Class=?
Command1=ID_GROUP_OPEN
Command2=ID_ADDITEMTOGROUP
Command3=ID_GROUP_DELETE
Command4=ID_RENAME_GROUP
CommandCount=4

[DLG:IDD_STYLE]
Type=1
Class=CExStyleDialog
ControlCount=23
Control1=IDC_CHECK_VIDEO,button,1342242819
Control2=IDC_CHECK_SOUND,button,1342242819
Control3=IDC_CHECK_IMAGE,button,1342242819
Control4=IDC_CHECK_RUNACTIVEXCTLS,button,1342242819
Control5=IDC_CHECK_DLACTIVEXCTLS,button,1342242819
Control6=IDC_CHECK_SCRIPTS,button,1342242819
Control7=IDC_CHECK_JAVA,button,1342242819
Control8=IDC_CHECK_NAVILOCK,button,1342242819
Control9=IDC_CHK_MSG_FILTER,button,1342242819
Control10=IDC_CHK_MOUSE_GESTURE,button,1342242819
Control11=IDC_CHK_BLOCK_MAILTO,button,1342242819
Control12=IDC_RADIO_RELOAD_NONE,button,1342373897
Control13=IDC_RADIO_RELOAD_15SEC,button,1342177289
Control14=IDC_RADIO_RELOAD_30SEC,button,1342177289
Control15=IDC_RADIO_RELOAD_1MIN,button,1342177289
Control16=IDC_RADIO_RELOAD_2MIN,button,1342177289
Control17=IDC_RADIO_RELOAD_5MIN,button,1342177289
Control18=IDC_RADIO_RELOAD_USER,button,1342177289
Control19=IDOK,button,1342373889
Control20=IDCANCEL,button,1342242816
Control21=IDC_STATIC,static,1342181376
Control22=IDC_STATIC,static,1342181376
Control23=IDC_STATIC,static,1342181376

[CLS:CExStyleDialog]
Type=0
HeaderFile=ExStyleDialog.h
ImplementationFile=ExStyleDialog.cpp
BaseClass=CDialog
Filter=D
LastObject=CExStyleDialog
VirtualFilter=dWC

[MNU:IDR_ITEM]
Type=1
Class=?
Command1=ID_ITEM_PROPERTY
Command2=ID_ITEM_DELETE
CommandCount=2

