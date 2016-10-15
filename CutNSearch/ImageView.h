#pragma once
#include "OGLWnd.h"

#include "opencv/cv.h"
#include "opencv2\core\core.hpp"
#include "opencv2\highgui\highgui.hpp"


#include "SNImage.h"


enum _RENDERMODE { _BYGROUP = 0, _BYORDER };
enum _PICKMODE { _PICK_SELECT=0 };
#define SEL_BUFF_SIZE 1024

class CViewTree;


typedef std::vector<CSNImage*> _vecSNImage;



class CImageView :
	public COGLWnd
{
public:
	CImageView();
	~CImageView();

	void Render();
	void Render2D();
	void InitGLview(int _nWidth, int _nHeight);
	void MouseWheel(short zDelta);

	bool LoadSNImage(CString strPath, CSNImage* pInfo, unsigned short resolution);
	GLuint LoadSNImage(CString strPath);
//	void SetTreeDragItem(CImageList* pImage, HTREEITEM hItem, CViewTree* pCtrl);
//	void AddImageData(HTREEITEM _item);
	void SetImageData();
	void PushImageDataSet(unsigned long _code, unsigned long _pcode, CSNImage* pimg);
	void PrepareRender();
	void SetIconMode(bool _mode) { m_bIconMode = _mode; };
	void SetRenderMode(_RENDERMODE mode);
	void StartCNSearch(IplImage *ptemp, bool bIsKeyword=false);
	void StartCNSearchAll(IplImage *ptemp);
	void SetThreshold(int value);
	float GetThreshold() { return m_Threshold; };
	void ClearMatchingResult();

	void ReleaseImageData();
	IplImage* ExtractAverTempleteFromResult();
private:

	CPoint m_mousedown;
	CPoint m_preMmousedown;
	short m_mouseMode;
	float m_fMoveSpeed;

	IplImage *m_cutImg;
	IplImage *m_pTemplete;
//	bool m_bIsTemplateCreated;
	bool m_bKeyWordSearch;



	POINT3D m_lookAt;
	VA_DRAG_OPER m_dragOper;
	LOGFONT		m_LogFont;
	BITMAPINFO* m_pBmpInfo;

	int			m_nWidth;
	int			m_nHeight;


	CImageList  *m_pTreeDragImage;
	HTREEITEM  m_hDragItem;
	CViewTree* m_pTreeCtrl;

	float m_left, m_right, m_bottom, m_top;
	float m_fDataLastPos;
	CSNImage* m_pCurrSelImg;

	unsigned int m_frmCnt;
	unsigned int m_addCnt;
	unsigned int m_searchCnt;

	float m_Threshold;

	// Image Data Set //
	std::map<unsigned long, CSNImage*> m_mapImageData;
	std::map<unsigned long, _vecSNImage> m_mapGrupImg;
	_vecSNImage m_vecImageData;

	bool	m_bNeedPrepare;
	float	m_iconSize;
	float	m_fScreenScale;
	bool	m_bIconMode;
	bool	m_bActualSizeMode;

	_RENDERMODE	m_renderMode;
	GLuint m_sellBuffer[SEL_BUFF_SIZE];


	void DrawImageByGroup();
	void DrawImageByGroupForPicking();
	void DrawImageByOrderForPicking();
	void DrawImageByOrder();
	void ReSizeIcon();

	int select_object_2d(int x, int y, int rect_width, int rect_height, _PICKMODE _mode);
	bool process_select(GLuint* index, int hit_num, int selmode);
	void RealSizeIcon();
	void ResetIconTextureId();
	CSNImage* GetSNImageByIndex(unsigned int idx);
	void GenerateThumbnail();
	void ProcCutNSearch(IplImage *pcutImg);
	void ProcCutNSearchBinary();
	POINT3D GetColor(float fvalue);

	POINT3D m_result_color[10];
	



public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};

