// DlgCutNSearch.cpp : implementation file
//

#include "stdafx.h"
#include "CutNSearch.h"
#include "DlgCutNSearch.h"
#include "afxdialogex.h"
#include "CutNSearchView.h"

// CDlgCutNSearch dialog
#define _WND_TOP_MARGIN 50

IMPLEMENT_DYNAMIC(CDlgCutNSearch, CDialog)

CDlgCutNSearch::CDlgCutNSearch(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCutNSearch::IDD, pParent)
	, m_editTh(0.0f)
	, m_editKeyword(_T(""))
	, m_checkKeyword(FALSE)
{
	m_pView = NULL;
}

CDlgCutNSearch::~CDlgCutNSearch()
{
	if (m_pView != NULL){
		delete m_pView;
		}
}

void CDlgCutNSearch::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, m_sliderCtrl);
	DDX_Text(pDX, IDC_EDIT1, m_editTh);
	DDV_MinMaxFloat(pDX, m_editTh, 0, 1.0);
	DDX_Text(pDX, IDC_EDIT_KEYWORD, m_editKeyword);
	DDX_Check(pDX, IDC_CHECK_KEYWORD, m_checkKeyword);
}


BEGIN_MESSAGE_MAP(CDlgCutNSearch, CDialog)
	ON_WM_SIZE()
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgCutNSearch::OnBnClickedButton1)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER1, &CDlgCutNSearch::OnNMReleasedcaptureSlider1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &CDlgCutNSearch::OnNMCustomdrawSlider1)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_RESULT, &CDlgCutNSearch::OnBnClickedButtonClearResult)
	ON_BN_CLICKED(IDC_BUTTON_KEYWORD_SEARCH, &CDlgCutNSearch::OnBnClickedButtonKeywordSearch)
	ON_BN_CLICKED(IDC_CHECK_KEYWORD, &CDlgCutNSearch::OnBnClickedCheckKeyword)
	ON_BN_CLICKED(IDC_BN_PIXELMAP, &CDlgCutNSearch::OnBnClickedBnPixelmap)
END_MESSAGE_MAP()


// CDlgCutNSearch message handlers


BOOL CDlgCutNSearch::Create(CWnd* pParentWnd)
{
	// TODO: Add your specialized code here and/or call the base class

	

	return CDialog::Create(IDD, pParentWnd);
}


void CDlgCutNSearch::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (m_pView){
		m_pView->MoveWindow(0, 40, cx, cy - 40);
	
	}
}

void CDlgCutNSearch::SetSliderPos(int _pos)
{
	m_editTh = (float)_pos*0.01f;
	m_sliderCtrl.SetPos(_pos);
	UpdateData(FALSE);
}

BOOL CDlgCutNSearch::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	CRect cRect;
	GetWindowRect(&cRect);
	int w = cRect.right - cRect.left;
	int h = cRect.bottom - cRect.top;

	if (m_pView == nullptr){
		m_pView = new CCNSDlgView;;
		m_pView->Create(NULL, NULL, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE, cRect, this, 0x02);
		m_pView->InitGLview(w,h);
		m_pView->MoveWindow(0, _WND_TOP_MARGIN, w, h - _WND_TOP_MARGIN);
	}

	m_sliderCtrl.SetRange(0, 100);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgCutNSearch::OnDropFiles(HDROP hDropInfo)
{
	// TODO: Add your message handler code here and/or call default

	UINT i = 0;
	UINT uCount = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
	TCHAR buffer[256];

	CString strPath;
	for (i = 0; i < uCount; i++){
		DragQueryFile(hDropInfo, i, buffer, 255);
		strPath = buffer;
		break;	
	}

	if (m_pView){
		SetSNImage(strPath);
	}

	CDialog::OnDropFiles(hDropInfo);
}


void CDlgCutNSearch::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	if (m_pView){
		m_pView->DoCNSearch();		// Cut template image from source code		
	}
}

void CDlgCutNSearch::SetSNImage(CString strPath)
{
	unsigned short w = 0, h = 0;
	if (m_pView->LoadSNImage(strPath, w, h) == true){

		if (w < 512)
			w = 512;
		if (h < 512)
			h = 512;

		MoveWindow(0, 0, w + 16, h + 80);
		m_pView->MoveWindow(0, 40, w, h);
		m_pView->FitImgToWnd(w, h);
	}
	m_pView->Render();
}





void CDlgCutNSearch::OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	int pos = m_sliderCtrl.GetPos();
	pView->SetThreshold(pos);


	*pResult = 0;
}




void CDlgCutNSearch::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);
	int pos = m_sliderCtrl.GetPos();
	m_editTh = (float)pos*0.01f;
	UpdateData(FALSE);

	*pResult = 0;
}


void CDlgCutNSearch::OnBnClickedButtonClearResult()
{
	// TODO: Add your control notification handler code here
	pView->ClearMatchingResult();
}


void CDlgCutNSearch::OnBnClickedButtonKeywordSearch()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_pView->KeywordSearch(m_editKeyword);	
}


void CDlgCutNSearch::OnBnClickedCheckKeyword()
{
	// TODO: Add your control notification handler code here

}


void CDlgCutNSearch::SetPixelMap(IplImage* pImg)
{
	if (m_pView){
		m_pView->SetPixelMap(pImg);
	}
}

void CDlgCutNSearch::OnBnClickedBnPixelmap()
{
	// TODO: Add your control notification handler code here

	if (m_pView){
		m_pView->DoPixelMap();		// Cut template image from source code		
	}
}
