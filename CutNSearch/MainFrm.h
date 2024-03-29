
// MainFrm.h : interface of the CMainFrame class
//

#pragma once
//#include "FileView.h"
//#include "ClassView.h"
#include "OutputWnd.h"
#include "DlgConf.h"

class CMainFrame : public CFrameWndEx
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

	void AddOutputString(CString str, bool IsReplace = false);
	void ResetOutputWnd();

	void InitConfituration();
	void GetImgFilePath(CString strPath);

//	CFileView* GetViewFiewCtrl() { return &m_wndFileView; }

	std::vector<_IMGPATHInfo>& GetImageListInfo() { return m_imgFileList; }
// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CMFCMenuBar       m_wndMenuBar;
//	CMFCToolBar       m_wndToolBar;
	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;
//	CFileView         m_wndFileView; 
//	CClassView        m_wndClassView;
	COutputWnd        m_wndOutput;


	CString			m_strSrcPath;
	CString			m_strLogPath;

	std::vector<_IMGPATHInfo> m_imgFileList;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
public:
	afx_msg void OnFileOpen();

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnFileConfiguration();
};


