
// CutNSearchView.cpp : implementation of the CCutNSearchView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "CutNSearch.h"
#endif

#include "CutNSearchDoc.h"
#include "CutNSearchView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCutNSearchView


IMPLEMENT_DYNCREATE(CCutNSearchView, CView)

BEGIN_MESSAGE_MAP(CCutNSearchView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_COMMAND(ID_RENDER_SORTBYGROUP, &CCutNSearchView::OnRenderSortbygroup)
	ON_COMMAND(ID_RENDER_SORTBYORDER, &CCutNSearchView::OnRenderSortbyorder)
	ON_COMMAND(ID_IMAGE_CLEARIMAGES, &CCutNSearchView::OnImageClearimages)
END_MESSAGE_MAP()

// CCutNSearchView construction/destruction
CCutNSearchView* pView = nullptr;

CCutNSearchView::CCutNSearchView()
{
	// TODO: add construction code here
	pView = this;

	m_pImageView = NULL;
	pDlg = NULL;
	
}

CCutNSearchView::~CCutNSearchView()
{
	if (m_pImageView != nullptr){
		delete m_pImageView;
	}

	if (pDlg != nullptr){
		delete pDlg;
	}


}

BOOL CCutNSearchView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CCutNSearchView drawing

void CCutNSearchView::OnDraw(CDC* /*pDC*/)
{
	//CCutNSearchDoc* pDoc = GetDocument();
	//ASSERT_VALID(pDoc);
	//if (!pDoc)
	//	return;

	// TODO: add draw code for native data here
}

void CCutNSearchView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CCutNSearchView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CCutNSearchView diagnostics

#ifdef _DEBUG
void CCutNSearchView::AssertValid() const
{
	CView::AssertValid();
}

void CCutNSearchView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

//CCutNSearchDoc* CCutNSearchView::GetDocument() const // non-debug version is inline
//{
//	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCutNSearchDoc)));
//	return (CCutNSearchDoc*)m_pDocument;
//}
#endif //_DEBUG


// CCutNSearchView message handlers


int CCutNSearchView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	CRect cRect;
	GetWindowRect(&cRect);

	if (m_pImageView == nullptr){
		m_pImageView = new CImageView;
		m_pImageView->Create(NULL, NULL, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE, cRect, this, 0x01);
		m_pImageView->InitGLview(0, 0);
	}


	if (pDlg == NULL){
		pDlg = new CDlgCutNSearch;
		pDlg->Create(this);
		pDlg->ShowWindow(SW_HIDE);
	}
	return 0;
}


void CCutNSearchView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (m_pImageView){
		m_pImageView->MoveWindow(0, 0, cx, cy);
	}
}

void CCutNSearchView::AddSNImage(CString str)
{
	//if (m_pImageView){
	//	m_pImageView->LoadSNImage(str);
	//}

}

void CCutNSearchView::ShowCNSDlg(int _th)
{	
	pDlg->ShowWindow(SW_SHOW);
	pDlg->SetSliderPos(_th);
}

void CCutNSearchView::SetDlgImagePath(CString str)
{
	pDlg->SetSNImage(str);
}

void CCutNSearchView::SetTreeDragItem(CImageList* pImage, HTREEITEM hItem, CViewTree* pCtrl)
{
	if (m_pImageView){
		m_pImageView->SetTreeDragItem(pImage, hItem, pCtrl);
	}
}

void CCutNSearchView::RenderImageView()
{
	if (m_pImageView){
		m_pImageView->Render();
	}
}


void CCutNSearchView::DoCNSearch(IplImage *ptemp)
{
	if (m_pImageView){
		m_pImageView->StartCNSearch(ptemp);
	}

}

void CCutNSearchView::SetThreshold(int _value)
{
	if (m_pImageView){
		m_pImageView->SetThreshold(_value);
	}
}




BOOL CCutNSearchView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	if (m_pImageView){
		m_pImageView->MouseWheel(zDelta);
	}

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}


void CCutNSearchView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	if (nChar == 17){  // ctrl key dowon
		if (m_pImageView){
			m_pImageView->SetIconMode(true);
		}
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CCutNSearchView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if (nChar == 17){  // ctrl key dowon
		if (m_pImageView){
			m_pImageView->SetIconMode(false);
		}
	}

	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}


void CCutNSearchView::OnRenderSortbygroup()
{
	// TODO: Add your command handler code here
	if (m_pImageView){
		m_pImageView->SetRenderMode(_BYGROUP);
	}
}


void CCutNSearchView::OnRenderSortbyorder()
{
	// TODO: Add your command handler code here
	if (m_pImageView){
		m_pImageView->SetRenderMode(_BYORDER);
	}
}

void CCutNSearchView::ClearMatchingResult()
{
	if (m_pImageView){
		m_pImageView->ClearMatchingResult();
	}
}

void CCutNSearchView::OnImageClearimages()
{
	// TODO: Add your command handler code here
	if (m_pImageView){
		m_pImageView->ReleaseImageData();
	}
}
