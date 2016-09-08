#pragma once
#include "OGLWnd.h"

#include "opencv/cv.h"
#include "opencv2\core\core.hpp"
#include "opencv2\highgui\highgui.hpp"

#include "SNImage.h"

class CCNSDlgView :
	public COGLWnd
{
public:
	CCNSDlgView();
	~CCNSDlgView();

	void Render();
	void Render2D();
	void InitGLview(int _nWidth, int _nHeight);
	void MouseWheel(short zDelta);
	bool LoadSNImage(CString strPath, unsigned short &_width, unsigned short &_height);
	void DoCNSearch();

private:
	float m_left, m_right, m_bottom, m_top;
	int		m_nWidth;
	int		m_nHeight;
	float	m_fXScale, m_fYScale;
	bool	m_bCNSReady;

	LOGFONT		m_LogFont;
	BITMAPINFO* m_pBmpInfo;

	POINT3D m_vScrCenter;
	float m_fScreenScale;
	CSNImage* m_pSrcImage;
	IplImage *m_pCutImg;

	CString m_strMousePos;
	POINT3D m_mousePos;

	POINT2Di m_vSelPos;
	RECT2D m_selRect;

	void DrawGuideLine();

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

