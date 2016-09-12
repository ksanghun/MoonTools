#pragma once
#include "resource.h"

#include "CNSDlgView.h"
#include "afxcmn.h"

// CDlgCutNSearch dialog

class CDlgCutNSearch : public CDialog
{
	DECLARE_DYNAMIC(CDlgCutNSearch)

public:
	CDlgCutNSearch(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgCutNSearch();

	void SetSNImage(CString strPath);
	void SetSliderPos(int _pos);

// Dialog Data
	enum { IDD = IDD_DIALOG_CNS };

private:
	CCNSDlgView* m_pView;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedButton1();
	CSliderCtrl m_sliderCtrl;

	afx_msg void OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	float m_editTh;
	afx_msg void OnBnClickedButtonClearResult();
	CString m_editKeyword;
	BOOL m_checkKeyword;
	afx_msg void OnBnClickedButtonKeywordSearch();
	afx_msg void OnBnClickedCheckKeyword();
};
