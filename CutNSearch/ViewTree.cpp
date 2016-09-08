
#include "stdafx.h"
#include "ViewTree.h"

#include "CutNSearchView.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewTree

CViewTree::CViewTree()
{
	m_pTreeDragImage = NULL;
	m_hDragItem = NULL;       
	m_bMouseTracking = FALSE;
}

CViewTree::~CViewTree()
{
}

BEGIN_MESSAGE_MAP(CViewTree, CTreeCtrl)
	ON_NOTIFY_REFLECT(NM_CLICK, &CViewTree::OnNMClick)	
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, &CViewTree::OnTvnBegindrag)
	ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewTree message handlers

BOOL CViewTree::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL bRes = CTreeCtrl::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT(pNMHDR != NULL);

	if (pNMHDR && pNMHDR->code == TTN_SHOW && GetToolTips() != NULL)
	{
		GetToolTips()->SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return bRes;
}


void CViewTree::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here	
	*pResult = 0;
}

CString CViewTree::GetItemFullPath()
{
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);

//	HTREEITEM hItem = HitTest(pt);

	HTREEITEM hItem = GetSelectedItem();
	std::vector<CString> strItem;

	if (hItem != NULL){
		strItem.push_back(GetItemText(hItem));

		HTREEITEM hPItem = GetParentItem(hItem);
		while (hPItem != NULL){
			strItem.push_back(GetItemText(hPItem));
			hPItem = GetParentItem(hPItem);
		}
	}


	if (strItem.size() > 0){
		m_strFullPath = "";
		for (int i = strItem.size() - 1; i >= 1; i--){			
			m_strFullPath += strItem[i];
			m_strFullPath += "\\";
		}
		m_strFullPath += strItem[0];
	}

	return m_strFullPath;
}


CString CViewTree::GetItemFullPath(HTREEITEM hItem)
{
	std::vector<CString> strItem;

	if (hItem != NULL){
		strItem.push_back(GetItemText(hItem));

		HTREEITEM hPItem = GetParentItem(hItem);
		while (hPItem != NULL){
			strItem.push_back(GetItemText(hPItem));
			hPItem = GetParentItem(hPItem);
		}
	}


	if (strItem.size() > 0){
		m_strFullPath = "";
		for (int i = strItem.size() - 1; i >= 1; i--){
			m_strFullPath += strItem[i];
			m_strFullPath += "\\";
		}
		m_strFullPath += strItem[0];
	}

	return m_strFullPath;
}



void CViewTree::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_pTreeDragImage)
	{
		// 트리컨트롤 기준으로 마우스 좌표 계산
		CPoint  p = point;
		ClientToScreen(&p);
		::ScreenToClient(m_hWnd, &p);

		// 마우스가 위치한 아이템을 검사한다.항목이 트리 뷰 항목위에 있는지 확인하고 그렇다면 항목이 밝게 표시되도록한다.
		HTREEITEM hItem = HitTest(p);

		//// 밝게 표시된 부분과 현재 선택된 아이템이 틀리다면

		if (hItem != GetDropHilightItem())
		{
			// 드래그 이미지 그리기 중지
			m_pTreeDragImage->DragLeave(this);
			// 새로운 항목을 밝게 표시한다.
			SelectDropTarget(hItem);			
			// 드래그 이미지를 다시 보여준다.
			m_pTreeDragImage->DragEnter(this, p);
		}
		else
		{
			m_pTreeDragImage->DragMove(p);
		}
	}


	CTreeCtrl::OnMouseMove(nFlags, point);
}


void CViewTree::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_pTreeDragImage)
	{
		// 마우스 메시지 캡쳐 기능을 제거한다.
		ReleaseCapture();
		// 드래그 과정을 중단한다.
		m_pTreeDragImage->DragLeave(this);
		m_pTreeDragImage->EndDrag();
		m_pTreeDragImage->DeleteImageList();
		
		m_pTreeDragImage = NULL;
		m_hDragItem = NULL;
		SelectItem(NULL);

	}

	CTreeCtrl::OnLButtonUp(nFlags, point);
}

void CViewTree::ReleaseItemDrag()
{
	if (m_pTreeDragImage)
	{
		// 마우스 메시지 캡쳐 기능을 제거한다.
		ReleaseCapture();
		// 드래그 과정을 중단한다.
		m_pTreeDragImage->DragLeave(this);
		m_pTreeDragImage->EndDrag();
		m_pTreeDragImage->DeleteImageList();
		m_pTreeDragImage = NULL;	
		m_hDragItem = NULL;
		
		SelectItem(NULL);
	}
}

void CViewTree::OnTvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	if (m_pTreeDragImage) m_pTreeDragImage->DeleteImageList();
	m_pTreeDragImage = CreateDragImage(pNMTreeView->itemNew.hItem);


	RECT  rc;
	GetItemRect(pNMTreeView->itemNew.hItem, &rc, TRUE); // 아이콘을 포함하는 크기

	// 드래그를 시작
	m_pTreeDragImage->BeginDrag(0, CPoint(pNMTreeView->ptDrag.x - rc.left + 16, pNMTreeView->ptDrag.y - rc.top));
	// 드래그 이미지 표시


	m_pTreeDragImage->DragEnter(this, pNMTreeView->ptDrag);

	// 마우스 메시지를 잡아두고
	SetCapture();

	// 현재 선택된 아이템 핸들을 기억
	m_hDragItem = pNMTreeView->itemNew.hItem;


	pView->SetTreeDragItem(m_pTreeDragImage, m_hDragItem, this);
	

	*pResult = 0;
}


void CViewTree::MouseMoveEnvet()
{
}


void CViewTree::OnMouseLeave()
{
	// TODO: Add your message handler code here and/or call default
	m_bMouseTracking = FALSE;

	CTreeCtrl::OnMouseLeave();
}
