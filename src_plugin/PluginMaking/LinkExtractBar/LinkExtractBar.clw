; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CBaseWindow
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "linkextractbar.h"
LastPage=0

ClassCount=5
Class1=CBaseWindow
Class2=CLinkExtractBarApp
Class3=CMyDialog
Class4=CMyToolBar
Class5=CMyWindow

ResourceCount=2
Resource1=IDD_DIALOG1
Resource2=IDR_POPMENU

[CLS:CBaseWindow]
Type=0
BaseClass=CWnd
HeaderFile=BaseWindow.h
ImplementationFile=BaseWindow.cpp
LastObject=CBaseWindow
Filter=W
VirtualFilter=WC

[CLS:CLinkExtractBarApp]
Type=0
BaseClass=CWinApp
HeaderFile=LinkExtractBar.h
ImplementationFile=LinkExtractBar.cpp
LastObject=CLinkExtractBarApp

[CLS:CMyDialog]
Type=0
BaseClass=CDialog
HeaderFile=MyDialog.h
ImplementationFile=MyDialog.cpp
LastObject=CMyDialog

[CLS:CMyToolBar]
Type=0
BaseClass=CToolBarCtrl
HeaderFile=MyToolBar.h
ImplementationFile=MyToolBar.cpp
LastObject=CMyToolBar
Filter=W
VirtualFilter=YWC

[CLS:CMyWindow]
Type=0
BaseClass=CWnd
HeaderFile=MyWindow.h
ImplementationFile=MyWindow.cpp
Filter=W
LastObject=CMyWindow

[DLG:IDD_DIALOG1]
Type=1
Class=CMyDialog
ControlCount=24
Control1=IDC_COMBO1,combobox,1344339971
Control2=IDC_COMBO2,combobox,1344339970
Control3=IDC_COMBO3,combobox,1344339970
Control4=IDC_LIST1,SysListView32,1350631433
Control5=IDC_STATIC,static,1342308864
Control6=IDC_STATIC,static,1342308864
Control7=IDC_STATIC,static,1342308864
Control8=IDC_STATIC,static,1342308864
Control9=IDC_EDIT1,edit,1350631552
Control10=IDC_BUTTON1,button,1342242816
Control11=IDC_OPEN,button,1342242816
Control12=IDC_COPY,button,1342242816
Control13=IDC_SAVE,button,1342242816
Control14=IDC_FILTER,button,1342242816
Control15=IDC_IRIA,button,1342242816
Control16=IDC_IRVINE,button,1342242816
Control17=IDC_DCSAKURA,button,1342242816
Control18=IDC_STATIC2,static,1342177803
Control19=IDC_FLASHGET,button,1342242816
Control20=IDC_NETTRANS,button,1342242816
Control21=IDC_REGET,button,1073807360
Control22=IDC_REGEX,button,1342242819
Control23=IDC_SELECTED,button,1342242819
Control24=IDC_IMGEXTRACT,button,1342242819

[MNU:IDR_POPMENU]
Type=1
Class=?
Command1=ID_ALLSELECT
Command2=ID_SELECTREVERSE
Command3=ID_SELECTRELEASE
Command4=ID_EXCLUDESELECT
Command5=ID_EXCLUDENOTSELECT
Command6=ID_SELECTEDITEM_OPEN
Command7=ID_ALLITEM_OPEN
Command8=ID_EXTRACT_LINK
CommandCount=8

