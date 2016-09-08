
#pragma once

/////////////////////////////////////////////////////////////////////////////
// CViewTree window

class CViewTree : public CTreeCtrl
{
// Construction
public:
	CViewTree();
	CString GetItemFullPath();
	CString GetItemFullPath(HTREEITEM hItem);



private:
	CString m_strFullPath;

	CImageList  *m_pTreeDragImage;
	HTREEITEM  m_hDragItem;
	BOOL m_bMouseTracking;

public:
	
	     
	void MouseMoveEnvet();
	void ReleaseItemDrag();
	
// Overrides
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

// Implementation
public:
	virtual ~CViewTree();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	
	
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnMouseLeave();
};
