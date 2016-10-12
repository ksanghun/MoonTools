#pragma once
#include "resource.h"

// CDlgConf dialog

class CDlgConf : public CDialog
{
	DECLARE_DYNAMIC(CDlgConf)

public:
	CDlgConf(CWnd* pParent = NULL, CString strSrc = _T(""), CString strLog = _T(""));   // standard constructor
	virtual ~CDlgConf();

// Dialog Data
	enum { IDD = IDD_DIALOG_CONF };

	CString GetSrcPath() { return strSrcPath; }
	CString GetLogPath() { return strLogPath; }
	void SetSrcPath(CString str) { strSrcPath = str; }
	void SetLogPath(CString str) { strLogPath = str; }
	void	UpdateStrData() { UpdateData(FALSE); }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CString strSrcPath;
	CString strLogPath;
	afx_msg void OnBnClickedButton1();
};
