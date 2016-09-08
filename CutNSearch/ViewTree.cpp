
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
		// Ʈ����Ʈ�� �������� ���콺 ��ǥ ���
		CPoint  p = point;
		ClientToScreen(&p);
		::ScreenToClient(m_hWnd, &p);

		// ���콺�� ��ġ�� �������� �˻��Ѵ�.�׸��� Ʈ�� �� �׸����� �ִ��� Ȯ���ϰ� �׷��ٸ� �׸��� ��� ǥ�õǵ����Ѵ�.
		HTREEITEM hItem = HitTest(p);

		//// ��� ǥ�õ� �κа� ���� ���õ� �������� Ʋ���ٸ�

		if (hItem != GetDropHilightItem())
		{
			// �巡�� �̹��� �׸��� ����
			m_pTreeDragImage->DragLeave(this);
			// ���ο� �׸��� ��� ǥ���Ѵ�.
			SelectDropTarget(hItem);			
			// �巡�� �̹����� �ٽ� �����ش�.
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
		// ���콺 �޽��� ĸ�� ����� �����Ѵ�.
		ReleaseCapture();
		// �巡�� ������ �ߴ��Ѵ�.
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
		// ���콺 �޽��� ĸ�� ����� �����Ѵ�.
		ReleaseCapture();
		// �巡�� ������ �ߴ��Ѵ�.
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
	GetItemRect(pNMTreeView->itemNew.hItem, &rc, TRUE); // �������� �����ϴ� ũ��

	// �巡�׸� ����
	m_pTreeDragImage->BeginDrag(0, CPoint(pNMTreeView->ptDrag.x - rc.left + 16, pNMTreeView->ptDrag.y - rc.top));
	// �巡�� �̹��� ǥ��


	m_pTreeDragImage->DragEnter(this, pNMTreeView->ptDrag);

	// ���콺 �޽����� ��Ƶΰ�
	SetCapture();

	// ���� ���õ� ������ �ڵ��� ���
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
