
// CutNSearchView.h : interface of the CCutNSearchView class
//

#pragma once
#include "ImageView.h"
#include "DlgCutNSearch.h"




class CViewTree;
	class CCutNSearchView : public CView
	{
	protected: // create from serialization only
		CCutNSearchView();
		DECLARE_DYNCREATE(CCutNSearchView)

		// Attributes
	public:
		//	CCutNSearchDoc* GetDocument() const;

		// Operations
	public:
		

	private:
		CImageView* m_pImageView;
		CDlgCutNSearch* pDlg;

		

		// Overrides
	public:
		virtual void OnDraw(CDC* pDC);  // overridden to draw this view
		virtual BOOL PreCreateWindow(CREATESTRUCT& cs);


		void AddSNImage(CString str);
		void SetTreeDragItem(CImageList* pImage, HTREEITEM hItem, CViewTree* pCtrl);
		void RenderImageView();
		void DoCNSearch(IplImage *ptemp, bool IsKeyword=false);
		void SetThreshold(int _value);
		
		void ShowCNSDlg(int _th);
		void SetDlgImagePath(CString str);
		void ClearMatchingResult();
		IplImage* ExtractTempleteFromResult();
	protected:

		// Implementation
	public:
		virtual ~CCutNSearchView();
#ifdef _DEBUG
		virtual void AssertValid() const;
		virtual void Dump(CDumpContext& dc) const;
#endif

	protected:

		// Generated message map functions
	protected:
		afx_msg void OnFilePrintPreview();
		afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
		afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
		DECLARE_MESSAGE_MAP()
	public:
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnSize(UINT nType, int cx, int cy);

		afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
		afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
		afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
		afx_msg void OnRenderSortbygroup();
		afx_msg void OnRenderSortbyorder();
		afx_msg void OnImageClearimages();
	};

#ifndef _DEBUG  // debug version in CutNSearchView.cpp
//	inline CCutNSearchDoc* CCutNSearchView::GetDocument() const
//	{ return reinterpret_cast<CCutNSearchDoc*>(m_pDocument); }
#endif



