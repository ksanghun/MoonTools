// DlgConf.cpp : implementation file
//

#include "stdafx.h"
#include "CutNSearch.h"
#include "DlgConf.h"
#include "afxdialogex.h"


// CDlgConf dialog

IMPLEMENT_DYNAMIC(CDlgConf, CDialog)

CDlgConf::CDlgConf(CWnd* pParent /*=NULL*/, CString strSrc, CString strLog)
	: CDialog(CDlgConf::IDD, pParent)
//	, strSrcPath(_T(""))
//	, strLogPath(_T(""))
{
	strSrcPath = strSrc;
	strLogPath = strLog;
}

CDlgConf::~CDlgConf()
{
}

void CDlgConf::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, strSrcPath);
	DDX_Text(pDX, IDC_EDIT2, strLogPath);
}


BEGIN_MESSAGE_MAP(CDlgConf, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgConf::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgConf::OnBnClickedButton1)
END_MESSAGE_MAP()


// CDlgConf message handlers


void CDlgConf::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here	
	if (strSrcPath != L""){
		CDialog::OnOK();
	}	
	else{
		AfxMessageBox(_T("Select Source File Folder"));
	}
}


void CDlgConf::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	ITEMIDLIST *pidlBrowse;
	CString strPath;

	BROWSEINFO BrInfo;

	BrInfo.hwndOwner = GetSafeHwnd();
	BrInfo.pidlRoot = NULL;

	memset(&BrInfo, 0, sizeof(BrInfo));
	BrInfo.pszDisplayName = (LPWSTR)(LPCTSTR)strPath;
	BrInfo.lpszTitle = _T("Select Source Location");
	BrInfo.ulFlags = BIF_RETURNONLYFSDIRS;

	// 다이얼로그 띄우기
	pidlBrowse = SHBrowseForFolder(&BrInfo);

	if (pidlBrowse != NULL)
	{
		BOOL bSuccess = ::SHGetPathFromIDList(pidlBrowse, (LPWSTR)(LPCTSTR)strPath);
		if (!bSuccess)
		{
			MessageBox(_T("Wrong folder name."), _T(""), MB_OKCANCEL | MB_ICONASTERISK);
			return;
		}
	}

	strSrcPath = strPath;
	strLogPath = strPath;
	UpdateData(FALSE);
}
