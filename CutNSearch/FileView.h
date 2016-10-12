
#pragma once

#include "ViewTree.h"


typedef	std::vector<CString> _strlist;



class CFileViewToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CFileView : public CDockablePane
{
// Construction
public:
	CFileView();

	void AdjustLayout();
	void OnChangeVisualStyle();
	void FillFileView(CString strFolder);
	CViewTree* GetTreeViewCtrl() { return &m_wndFileView; }
	CImageList* GetImageList() { return &m_FileViewImages; }
	HTREEITEM GetRootItem() { return m_rootItem; }

	void MouseMoveEnvet();

private:
	HTREEITEM ExtractFolder(CString strFolder, CString strName, _strlist& filelist, HTREEITEM& parentItem);

// Attributes
protected:

	CViewTree m_wndFileView;
	CImageList m_FileViewImages;
	CFileViewToolBar m_wndToolBar;
	HTREEITEM m_rootItem;

    
	
protected:


// Implementation
public:
	virtual ~CFileView();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnProperties();
	afx_msg void OnFileOpen();
	afx_msg void OnFileOpenWith();
	afx_msg void OnDummyCompile();
	afx_msg void OnEditCut();
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()

	
public:

	afx_msg void OnClose();
};

