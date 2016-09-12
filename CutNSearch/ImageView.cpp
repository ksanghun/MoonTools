#include "stdafx.h"
#include "ImageView.h"
#include "resource.h"
#include "ViewTree.h"
#include "MainFrm.h"
#include "CutNSearchView.h"

enum TIMEREVNT { _RENDER=100, _ADDIMG , _SEARCHIMG};
#define _MIN_ICON_SIZE 16

CImageView::CImageView()
{
	memset(&m_LogFont, 0, sizeof(m_LogFont));
//	strcpy((char*)m_LogFont.lfFaceName, ("Arial"));
	m_LogFont.lfCharSet = ANSI_CHARSET;
	m_LogFont.lfHeight = -10;
	m_LogFont.lfWidth = 0;
	//	m_LogFont.lfWeight = FW_BOLD;

	m_pBmpInfo = (BITMAPINFO *)malloc(sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD) * 256));

	m_nWidth = 0;
	m_nHeight = 0;

	m_dragOper.init();
	m_mouseMode = 0;


	m_pTreeDragImage = NULL;
	m_hDragItem = NULL;
	m_pTreeCtrl = NULL;

	m_bNeedPrepare = false;

	m_left = m_right = m_bottom = m_top =0.0f;

	m_iconSize = 100.0f;
	m_fScreenScale = 2.5f;

	m_fMoveSpeed = 0.0f;
	m_bIconMode = false;

	m_renderMode = _BYGROUP;
	m_pCurrSelImg = NULL;

	m_frmCnt = 0;
	m_addCnt = 0;
	m_searchCnt = 0;
	m_cutImg = NULL;
	m_pTemplete = NULL;
	m_Threshold = 0.75f;
	m_bIsTemplateCreated = false;
	m_bKeyWordSearch = false;
}


CImageView::~CImageView()
{
	delete m_pBmpInfo;
	//std::map<unsigned long, CSNImage*>::iterator iter = m_mapImageData.begin();
	//for (; iter != m_mapImageData.end(); iter++){

	//	GLuint tex = iter->second->GetTxTex();
	//	glDeleteTextures(1, &tex);
	//	delete iter->second;
	//}
	ReleaseImageData();

}

BEGIN_MESSAGE_MAP(CImageView, COGLWnd)
	ON_WM_SIZE()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()



void CImageView::ReleaseImageData()
{
	//std::map<unsigned long, CSNImage*> m_mapImageData;
	//std::map<unsigned long, _vecSNImage> m_mapGrupImg;
	//_vecSNImage m_vecImageData;


	for (int i = 0; i < m_vecImageData.size(); i++){
		GLuint texid = m_vecImageData[i]->GetTexId();
		if (texid != 0)	glDeleteTextures(1, &texid);
		GLuint thtex = m_vecImageData[i]->GetTxTex();
		if (thtex != 0) glDeleteTextures(1, &thtex);

		delete m_vecImageData[i];
		m_vecImageData[i] = NULL;
	}
	
	std::map<unsigned long, _vecSNImage>::iterator iter_gr = m_mapGrupImg.begin();
	for (; iter_gr != m_mapGrupImg.end(); iter_gr++){
		iter_gr->second.clear();
	}

	m_vecImageData.clear();
	m_mapImageData.clear();
	m_mapGrupImg.clear();
	m_addCnt = 0;

	//CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
	//pM->AddOutputString(_T("Clear All Images.."));
	
}

void CImageView::RealSizeIcon()
{
	if (m_iconSize > m_nWidth*0.5f){		
		int hits = select_object_2d(m_nWidth*0.5f, m_nHeight*0.5f, m_nWidth, m_nHeight, _PICK_SELECT);
		for (int i = 0; i < hits; i++){
			CSNImage* pimg = GetSNImageByIndex((int)m_sellBuffer[i * 4 + 3]);
			if (pimg->GetTexId() == 0){
				GLuint texid = LoadSNImage(pimg->GetPath());
				pimg->SetTexId(texid);
			}
		}
	}
}

void CImageView::ResetIconTextureId()
{
	if (m_iconSize <= m_nWidth*0.5f){
		_vecSNImage::iterator iter = m_vecImageData.begin();
		int id = 0;
		for (; iter != m_vecImageData.end(); iter++){
			(*iter)->SetTexId(0);
		}
	}
}


CSNImage* CImageView::GetSNImageByIndex(unsigned int idx)
{
	CSNImage* pimg=NULL;
	switch (m_renderMode){
	case _BYGROUP:
		{
			int groupid = idx / 1000;
			int vid = idx % 1000;

			std::map<unsigned long, _vecSNImage>::iterator iter_gr = m_mapGrupImg.begin();
			int gid = 0;
			for (; iter_gr != m_mapGrupImg.end(); iter_gr++){
				if (gid == groupid){
					pimg = iter_gr->second[vid];
					break;
				}
				gid++;
			}
		}
		break;
	case _BYORDER:		
		pimg = m_vecImageData[idx];		
		break;
	}
	return pimg;
}


void CImageView::Render()
{
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	RealSizeIcon();
	Render2D();

	SwapBuffers(m_CDCPtr->GetSafeHdc());
}

void CImageView::Render2D()
{
	if (m_bNeedPrepare){
		PrepareRender();
		m_bNeedPrepare = false;
	}


	int pointSize = m_iconSize / 100;
	if (pointSize < 4)
		pointSize = 4;

	glLineWidth(2.0f);
	glPointSize(pointSize);

	gl_PushOrtho(m_left, m_right, m_bottom, m_top);

	if (m_renderMode == 0){
		DrawImageByGroup();
	}
	else{
		DrawImageByOrder();
	}
	gl_PopOrtho();

	glLineWidth(1.0f);
	glPointSize(1.0f);
}

void CImageView::SetRenderMode(_RENDERMODE mode)
{
	m_renderMode = mode;
	m_bNeedPrepare = true;
}

void CImageView::PrepareRender()
{
//	std::map<unsigned long, CSNImage*>::iterator iter = m_mapImageData.begin();

	POINT3D sPnt;
	int linecnt = 1;
	float fMargin = 5.0f;
	mtSetPoint3D(&sPnt, m_left, m_top - m_iconSize - fMargin, 0);

	//for (; iter != m_mapImageData.end(); iter++){

	//	iter->second->SetPosition(sPnt);
	//	sPnt.x += _IMGICON_SIZE + fMargin;

	//	if (sPnt.x > m_nWidth - _IMGICON_SIZE){ // Change Line
	//		mtSetPoint3D(&sPnt, m_left, m_top - (_IMGICON_SIZE + fMargin)* ++linecnt , 0);
	//	}		
	//}

	if (m_renderMode == _BYGROUP){
		std::map<unsigned long, _vecSNImage>::iterator iter_gr = m_mapGrupImg.begin();
		//	mtSetPoint3D(&sPnt, 0.0f + m_iconSize, m_top - m_iconSize - fMargin, 0);
		sPnt.x = m_iconSize*0.5f;
		sPnt.y = m_nHeight - m_iconSize - fMargin;
		sPnt.z = 0.0f;

		for (; iter_gr != m_mapGrupImg.end(); iter_gr++){
			// Group By Group //


			for (int i = 0; i < iter_gr->second.size(); i++){
				if (iter_gr->second[i]->GetTxTex() > 0){
					iter_gr->second[i]->SetPosition(sPnt);
					sPnt.x += m_iconSize + fMargin;

					if (sPnt.x > m_nWidth - m_iconSize*0.5f){ // Change Line
						//	mtSetPoint3D(&sPnt, m_left + m_iconSize, m_top - (m_iconSize + fMargin)* ++linecnt, 0);
						sPnt.x = m_iconSize*0.5f;
						sPnt.y -= (m_iconSize + fMargin);
					}
				}
			}


			sPnt.x = m_iconSize*0.5f;
			sPnt.y -= m_iconSize*1.5f;
		}

		m_fDataLastPos = sPnt.y;
		if (m_fDataLastPos > 0.0f)
			m_fDataLastPos = -m_iconSize;
	}


	else{
		_vecSNImage::iterator iter = m_vecImageData.begin();
		sPnt.x = m_iconSize*0.5f;
		sPnt.y = m_nHeight - m_iconSize - fMargin;
		sPnt.z = 0.0f;

		for (; iter != m_vecImageData.end(); iter++){
			if ((*iter)->GetTxTex() > 0){
				(*iter)->SetPosition(sPnt);
				sPnt.x += m_iconSize + fMargin;

				if (sPnt.x > m_nWidth - m_iconSize*0.5f){ // Change Line
					sPnt.x = m_iconSize*0.5f;
					sPnt.y -= (m_iconSize + fMargin);
				}
			}
		}

		m_fDataLastPos = sPnt.y;
		if (m_fDataLastPos > 0.0f)
			m_fDataLastPos = -m_iconSize;
	}




	if (m_pCurrSelImg != NULL){		// track selected Icon //
		m_bottom = m_pCurrSelImg->GetPos().y - m_nHeight*0.5f;
		m_top = m_bottom + m_nHeight;
	}


}


void CImageView::DrawImageByGroup()
{
	std::map<unsigned long, _vecSNImage>::iterator iter_gr = m_mapGrupImg.begin();
//	glEnable(GL_TEXTURE_2D);
	for (; iter_gr != m_mapGrupImg.end(); iter_gr++){
		for (int i = 0; i < iter_gr->second.size(); i++){
			iter_gr->second[i]->DrawThumbNail(1.0f);
		}				
	}
//	glDisable(GL_TEXTURE_2D);
}

void CImageView::DrawImageByGroupForPicking()
{
	std::map<unsigned long, _vecSNImage>::iterator iter_gr = m_mapGrupImg.begin();

	int gid = 0;
	for (; iter_gr != m_mapGrupImg.end(); iter_gr++){
		// Group By Group //
		for (int i = 0; i < iter_gr->second.size(); i++){
			glPushName(gid * 1000 + i);
			iter_gr->second[i]->DrawForPicking();
			glPopName();
		}
		gid++;
	}
	//	glDisable(GL_TEXTURE_2D);
}

void CImageView::DrawImageByOrderForPicking()
{
	_vecSNImage::iterator iter = m_vecImageData.begin();
	int id = 0;
	for (; iter != m_vecImageData.end(); iter++){
		glPushName(id);
		(*iter)->DrawForPicking();
		glPopName();
		id++;
	}

}

void CImageView::DrawImageByOrder()
{
	_vecSNImage::iterator iter = m_vecImageData.begin();
	glEnable(GL_TEXTURE_2D);
	for (; iter != m_vecImageData.end(); iter++){
		(*iter)->DrawThumbNail(1.0f);
	}
	glDisable(GL_TEXTURE_2D);
}

void CImageView::InitGLview(int _nWidth, int _nHeight)
{
	m_nWidth = _nWidth;
	m_nHeight = _nHeight;

	
	m_lookAt.x = 0;
	m_lookAt.y = 0;
	m_lookAt.z = 0;
	m_cameraPri.InitializeCamera(30.0f, 0.0f, 0.0f, m_lookAt, _nWidth, _nHeight);
	m_cameraPri.SetInitLevelHeight(1000);

	glDisable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);


	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);


	//SetClassLong(m_hWnd,
	//	GCL_HCURSOR,
	//	(LONG)LoadCursor(AfxGetInstanceHandle(),
	//	MAKEINTRESOURCE(IDC_CURSOR1)));
	SetTimer(_RENDER, 10, NULL);
}
void CImageView::MouseWheel(short zDelta)
{
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);	
	//float zoomValue = m_cameraPri.GetLevelHeight()*0.1f;
	//if (zDelta>0)
	//	zoomValue = -zoomValue;
	//m_cameraPri.SetLevelHeight(zoomValue);
	//m_cameraPri.SetModelViewMatrix(m_lookAt, 0, 0);
	//Render();
	//if (m_bIconMode == true){
		m_iconSize += zDelta*0.1f;
		if (m_iconSize < _MIN_ICON_SIZE)
			m_iconSize = _MIN_ICON_SIZE;
		if (m_iconSize > m_nWidth)
			m_iconSize = m_nWidth;
		
		ReSizeIcon();		
	//}
	//else{
	//	m_bottom += zDelta*0.5f;
	//	if (m_bottom > 0.0f)
	//		m_bottom = 0.0f;
	//	if (m_bottom < m_fDataLastPos)
	//		m_bottom = m_fDataLastPos;

	//	// Set Wnd rect =======//
	//	m_right = m_left + m_nWidth;		
	//	m_top = m_bottom + m_nHeight;
	//	
	//}
}


void CImageView::ReSizeIcon()
{
	_vecSNImage::iterator iter = m_vecImageData.begin();
	for (; iter != m_vecImageData.end(); iter++){
		(*iter)->SetSize((*iter)->GetWidth(), (*iter)->GetHeight(), m_iconSize);
	}
	m_bNeedPrepare = true;
}

void CImageView::OnSize(UINT nType, int cx, int cy)
{
	COGLWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);
	m_nWidth = cx*m_fScreenScale;
	m_nHeight = cy*m_fScreenScale;

	m_right = m_left + m_nWidth;
	m_top = m_bottom + m_nHeight;

	m_cameraPri.SetProjectionMatrix(30.0f, 0.0f, 0.0f, cx, cy);
	m_cameraPri.SetModelViewMatrix(m_cameraPri.GetLookAt(), 0.0f, 0.0f);

	m_bNeedPrepare = true;
}

GLuint CImageView::LoadSNImage(CString strPath)
{
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);
	GLuint nTexId = 0;
	USES_CONVERSION;
	char* sz = T2A(strPath);

	IplImage *pimg = cvLoadImage(sz);
	if (pimg){
		//	cvShowImage(sz, pimg);
		cvCvtColor(pimg, pimg, CV_BGR2RGB);

		// glupload Image - Thumnail image=======================================================//
		glGenTextures(1, &nTexId);
		glBindTexture(GL_TEXTURE_2D, nTexId);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F);
		//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		//glTexImage2D(GL_TEXTURE_2D, 0, 3, m_texture->sizeX,m_texture->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE,m_texture->data);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, pimg->width, pimg->height, GL_RGB, GL_UNSIGNED_BYTE, pimg->imageData);
		//======================================================================================//
		cvReleaseImage(&pimg);
	}
	return nTexId;
}

bool CImageView::LoadSNImage(CString strPath, CSNImage* pInfo, unsigned short resolution)
{
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);
	USES_CONVERSION;
	char* sz = T2A(strPath);

	IplImage *pimg = cvLoadImage(sz);	
	if (pimg){
	//	cvShowImage(sz, pimg);
		IplImage* timg = 0;
		
		timg = cvCreateImage(cvSize(resolution, resolution), pimg->depth, pimg->nChannels);
		cvResize(pimg, timg);
	//	cvShowImage("Resize", timg);		


		cvCvtColor(timg, timg, CV_BGR2RGB);

		pInfo->SetImgSize(pimg->width, pimg->height);
		pInfo->SetSize(pimg->width, pimg->height, m_iconSize);		


		// glupload Image - Thumnail image=======================================================//
		GLuint tid = 0;
		glGenTextures(1, &tid);
		glBindTexture(GL_TEXTURE_2D, tid);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F);
		//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		//glTexImage2D(GL_TEXTURE_2D, 0, 3, m_texture->sizeX,m_texture->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE,m_texture->data);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, resolution, resolution, GL_RGB, GL_UNSIGNED_BYTE, timg->imageData);
		//======================================================================================//

		pInfo->SetThTex(tid);

		cvReleaseImage(&timg);
		cvReleaseImage(&pimg);

		
		return true;
	}

	else{
		return false;
	}
	
}


void CImageView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (GetCapture()){
		ReleaseCapture();
		m_fMoveSpeed = point.y - m_preMmousedown.y;
	}

	COGLWnd::OnLButtonUp(nFlags, point);
}


void CImageView::SetTreeDragItem(CImageList* pImage, HTREEITEM hItem, CViewTree* pCtrl)
{
	m_pTreeDragImage = pImage;	
	m_hDragItem = hItem;
	m_pTreeCtrl = pCtrl;
}

void CImageView::PushImageDataSet(unsigned long _code, unsigned long _pcode, CSNImage* pimg)
{
	CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();

	std::map<unsigned long, _vecSNImage>::iterator iter_gr;
	std::map<unsigned long, CSNImage*>::iterator iter;

	iter = m_mapImageData.find(_code);
	if (iter == m_mapImageData.end()){

		// Store Data Information ================================//!!!!
		m_mapImageData[_code] = pimg;		// for duplication checking

		// push image data sequecily ..
		m_vecImageData.push_back(pimg);

		// Make Group =========================//
		iter_gr = m_mapGrupImg.find(_pcode);
		if (iter_gr == m_mapGrupImg.end()){		// New Group
			_vecSNImage vecImg;
			vecImg.push_back(pimg);
			m_mapGrupImg[_pcode] = vecImg;
		}
		else{
			m_mapGrupImg[_pcode].push_back(pimg);
		}
		//======================================//

	//	pM->ResetOutputWnd();
		pM->AddOutputString(pimg->GetPath(), true);
	}
}


void CImageView::AddImageData(HTREEITEM _item)
{
	USES_CONVERSION;
	char* sz = 0;

	CString strPath, strPName, strName;
	unsigned long pCode=0, cCode = 0;
	HTREEITEM hChildItem = m_pTreeCtrl->GetChildItem(_item);

	if (hChildItem == NULL){  // No Child!! File
		CSNImage* pimg = new CSNImage;

		HTREEITEM pItem = m_pTreeCtrl->GetParentItem(_item);

		strName = m_pTreeCtrl->GetItemText(_item);
		strPName = m_pTreeCtrl->GetItemText(pItem);
		strPath = m_pTreeCtrl->GetItemFullPath(_item);


		sz = T2A(strPName);
		pCode = getHashCode(sz);
		//=================================================//	
		
	//	if (LoadSNImage(strPath, pimg, 128) == true){
			//==================================//		
			sz = T2A(strPath);
			cCode = getHashCode(sz);

			// Add Image Data //
			pimg->SetName(strPath, strPName, strName, pCode, cCode);

			PushImageDataSet(cCode, pCode, pimg);			
			//iter = m_mapImageData.find(cCode);
			//if (iter == m_mapImageData.end()){

			//	// Store Data Information ================================//!!!!
			//	m_mapImageData[cCode] = pimg;		// for duplication checking

			//	// push image data sequecily ..
			//	m_vecImageData.push_back(pimg);

			//	// Make Group =========================//
			//	iter_gr = m_mapGrupImg.find(pCode);
			//	if (iter_gr == m_mapGrupImg.end()){		// New Group
			//		_vecSNImage vecImg;
			//		vecImg.push_back(pimg);
			//		m_mapGrupImg[pCode] = vecImg;
			//	}
			//	else{
			//		m_mapGrupImg[pCode].push_back(pimg);
			//	}
			//	//======================================//

			//	pM->ResetOutputWnd();
			//	pM->AddOutputString(strPath);
			//}
		//}
		//else{
		//	//pM->AddOutputString(strPath);
		//	//pM->AddOutputString(_T("!!!!!!!!!!!!!Not image file"));
		//	delete pimg;
		//}

	}
	else{		// Has Child : Folder //
		strPName = m_pTreeCtrl->GetItemText(_item);
		sz = T2A(strPName);
		pCode = getHashCode(sz);

		while (hChildItem){
			HTREEITEM cItem = m_pTreeCtrl->GetChildItem(hChildItem);

			if (cItem == NULL){ // File //
				CSNImage* pimg = new CSNImage;

				strName = m_pTreeCtrl->GetItemText(hChildItem);
				strPath = m_pTreeCtrl->GetItemFullPath(hChildItem);

				//===================================//
				
			//	if (LoadSNImage(strPath, pimg, 128) == true){
					//==================================//
					char* sz = T2A(strPath);
					cCode = getHashCode(sz);

					// Add Image Data //
					pimg->SetName(strPath, strPName, strName, pCode, cCode);
					std::map<unsigned long, CSNImage*>::iterator iter;

					PushImageDataSet(cCode, pCode, pimg);

					//iter = m_mapImageData.find(cCode);
					//if (iter == m_mapImageData.end()){

					//	// Store Data Information ================================//!!!!
					//	m_mapImageData[cCode] = pimg;
					//	m_vecImageData.push_back(pimg);						
					//	// Store Data Information ================================//!!!!

					//	pM->AddOutputString(strPath);
					//}
				//}
				//else{
				//	delete pimg;
				//	//pM->AddOutputString(strPath);
				//	//pM->AddOutputString(_T("!!!!!!!!!Not image file"));
				//}
			}

			else{
				AddImageData(hChildItem);
			}	

			hChildItem = m_pTreeCtrl->GetNextItem(hChildItem, TVGN_NEXT);
		}
	}
}



void CImageView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if (m_pTreeDragImage)
	{				
	//	m_pTreeCtrl->SendMessage(WM_LBUTTONUP);
		AddImageData(m_hDragItem);		

		CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
		pM->AddOutputString(_T("Generate thumbnails"));
		SetTimer(_ADDIMG, 10, NULL);

		m_pTreeCtrl->ReleaseItemDrag();
		m_pTreeDragImage = NULL;
	}
	
	if (GetCapture()){		
		m_bottom += (point.y - m_mousedown.y)*m_fScreenScale;
		if (m_bottom > 0.0f)
			m_bottom = 0.0f;
		if (m_bottom < m_fDataLastPos)
			m_bottom = m_fDataLastPos;


		// Set Wnd rect =======//
		m_right = m_left + m_nWidth;
		m_top = m_bottom + m_nHeight;

		m_preMmousedown = m_mousedown;
		m_mousedown = point;

	//	Render();
	}	

	COGLWnd::OnMouseMove(nFlags, point);
}


BOOL CImageView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return COGLWnd::OnEraseBkgnd(pDC);
}


void CImageView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == _RENDER){

		if ((m_fMoveSpeed > 0.5f) || (m_fMoveSpeed < -0.5f)){
			m_bottom += m_fMoveSpeed*m_fScreenScale;
			if (m_bottom > 0.0f)
				m_bottom = 0.0f;
			if (m_bottom < m_fDataLastPos)
				m_bottom = m_fDataLastPos;

			m_right = m_left + m_nWidth;
			m_top = m_bottom + m_nHeight;
			m_fMoveSpeed*=0.85f;
		}


		if (m_frmCnt > 300){
			ResetIconTextureId();
			m_frmCnt = 0;
		}
		Render();
		m_frmCnt++;
	}


	if (nIDEvent == _ADDIMG){
		GenerateThumbnail();		
	}

	if (nIDEvent == _SEARCHIMG){
		ProcCutNSearch();		
	//	ProcCutNSearchBinary();

	}
	COGLWnd::OnTimer(nIDEvent);
}

void CImageView::ProcCutNSearch()
{
	int cnt = 0;

	int search_size = m_cutImg->width;
	if (search_size > m_cutImg->height)
		search_size = m_cutImg->height;


	for (; m_searchCnt < m_vecImageData.size(); m_searchCnt++){
		CSNImage* pImg = m_vecImageData[m_searchCnt];
		if (pImg->GetTxTex() == 0)
			continue;
		//	LoadSNImage(pImg->GetPath(), pImg, 128);

		USES_CONVERSION;
		char* sz = T2A(pImg->GetPath());
		//=================================================================================================================
		IplImage *gray = cvLoadImage(sz, CV_LOAD_IMAGE_GRAYSCALE);
	//	cvShowImage("gray", gray);

		//IplImage* im_bw = cvCreateImage(cvGetSize(gray), IPL_DEPTH_8U, 1);
		//cvThreshold(gray, im_bw, 128, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
		//cvShowImage("im_bw", im_bw);

	//	IplImage *gray = cvCreateImage(cvSize(dstImg->width, dstImg->height), 8, 1);
		//if (dstImg->nChannels == 3)
		//	cvCvtColor(dstImg, gray, CV_RGB2GRAY);//Change from RGB to GrayScale
		//else
		//	cvCopy(dstImg, gray);

		IplImage *result_img = cvCreateImage(cvSize(gray->width - m_cutImg->width + 1, gray->height - m_cutImg->height + 1),
			IPL_DEPTH_32F, 1);

		cvMatchTemplate(gray, m_cutImg, result_img, CV_TM_CCOEFF_NORMED);
		cvShowImage("percentage map", result_img);

		float* d = (float*)result_img->imageData;
	//	float fTh = 0.75f;
	//	std::vector<POINT3D> tmp_result;
		for (int y = 0; y<result_img->height; y++){
			for (int x = 0; x<result_img->width; x++){
				float fD = *(d + y*result_img->width + x);
				if (fD>m_Threshold)	{
					POINT3D left_top;
					left_top.x = x + m_cutImg->width*0.5f;
					left_top.y = y + m_cutImg->height*0.5f;
					left_top.z = fD;

					pImg->AddMatchedPoint(left_top, search_size);

				}
			}
		}


		//for (int i = 0; i<tmp_result.size(); i++){
		//	POINT3D pos1 = tmp_result[i];
		//	bool bAvailable = true;
		//	for (int j = 0; j<tmp_result.size(); j++){
		//		if (j == i)
		//			continue;
		//		POINT3D pos2 = tmp_result[j];
		//		float fDist = mtDistance(pos1, pos2);
		//		if (fDist < search_size){
		//			//?					if (pos1.z < pos2.z){
		//			bAvailable = false;
		//			break;
		//			//?					}
		//		}				
		//	}

		//	if (bAvailable == true)
		//	{
		//		//	match_result.push_back(pos1);
		//		pImg->AddMatchedPoint(pos1);
		//	}
		//}


//		cvReleaseImage(&dstImg);
		cvReleaseImage(&result_img);
		cvReleaseImage(&gray);
		//================================================================//	
		//if (tmp_result.size() > 0){
		//	pImg->SetBgColor(0.9f, 0.7f, 0.2f);
		//}

		cnt++;
		if (cnt>10){
			break;
		}
	}

	if (m_searchCnt >= m_vecImageData.size()){
		KillTimer(_SEARCHIMG);


		if (m_bKeyWordSearch == true){
			ExtractAverTempleteFromResult();
			m_cutImg = m_pTemplete;

			ClearMatchingResult();

			m_searchCnt = 0;
			SetTimer(_SEARCHIMG, 10, NULL);

			CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
			pM->AddOutputString(_T("Start Keyword Search Process...."));

			m_bKeyWordSearch = false;			
		}


	}


	float complete = (float)m_searchCnt / (float)m_vecImageData.size();

	CString str;
	str.Format(_T("Searching images.....%d"), int(complete * 100));
	str += _T("%");
	str += _T(" completed.");

	CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
	pM->AddOutputString(str, true);
}

void CImageView::GenerateThumbnail()
{
	int cnt = 0;
	for (; m_addCnt < m_vecImageData.size(); m_addCnt++){
		CSNImage* pImg = m_vecImageData[m_addCnt];
		LoadSNImage(pImg->GetPath(), pImg, 128);
		cnt++;
		if (cnt>10){
			break;
		}
	}

	if (m_addCnt >= m_vecImageData.size()){
		KillTimer(_ADDIMG);
	}

	m_bNeedPrepare = true;
	float complete = (float)m_addCnt / (float)m_vecImageData.size();

	CString str;
	str.Format(_T("Generating thumbnails.....%d"), int(complete * 100));
	str += _T("%");
	str += _T(" completed.");

	CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
	pM->AddOutputString(str, true);
}



void CImageView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);

	m_mousedown = point;
	m_preMmousedown = m_mousedown;
	m_fMoveSpeed = 0.0f;
	SetCapture();

	if (select_object_2d(point.x, point.y, 2, 2, _PICK_SELECT) > 0){
		if (m_pCurrSelImg != NULL){
			m_pCurrSelImg->SetSelecttion(false);
		}
		m_pCurrSelImg = GetSNImageByIndex((int)m_sellBuffer[3]);
		m_pCurrSelImg->SetSelecttion(true);
	}


	COGLWnd::OnLButtonDown(nFlags, point);
}




int CImageView::select_object_2d(int x, int y, int rect_width, int rect_height, _PICKMODE _mode)
{
	//GLuint selectBuff[1024];
	memset(&m_sellBuffer, 0, sizeof(GLuint) * SEL_BUFF_SIZE);

	GLint hits, viewport[4];
	hits = 0;

	glSelectBuffer(SEL_BUFF_SIZE, m_sellBuffer);
	glGetIntegerv(GL_VIEWPORT, viewport);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glRenderMode(GL_SELECT);
	glLoadIdentity();
	gluPickMatrix(x, viewport[3] - y, rect_width, rect_height, viewport);
	gluOrtho2D(m_left, m_right, m_bottom, m_top);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//======Render For Picking========//
	switch (m_renderMode){
	case _BYGROUP:
		DrawImageByGroupForPicking();
		break;
	case _BYORDER:
		DrawImageByOrderForPicking();
		break;
	}

	//glPushName(1000);
	//if (m_b_show_book_case == false)
	//	m_button[0]->DrawButton(0);		// open book case;
	//else
	//	m_button[0]->DrawButton(1);
	//glPopName();

	//for (int i = 1; i<m_button.size(); i++)
	//{
	//	glPushName(i + 1000);
	//	m_button[i]->DrawButton(0);
	//	glPopName();
	//}

	hits = glRenderMode(GL_RENDER);
	//if (hits>0)
	//{
	//	process_select(selectBuff, hits, selmode);
	//	int a = 0;
	//}
	//else
	//{
	//	if (selmode == 1)
	//	{

	//	}
	//}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	return hits;
}


bool CImageView::process_select(GLuint* index, int hit_num, int selmode)
{
	int selid = 0;
	for (int i = 0; i < hit_num; i++){
		selid = (int)index[i*4+3];
	}
	return true;
}

void CImageView::SetThreshold(int _value)
{
	m_Threshold = (float)_value*0.01f;

	CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
	CString str;
	str.Format(_T("Threshold value is changed (%3.2f)"), m_Threshold);
	pM->AddOutputString(str);
}

void CImageView::StartCNSearch(IplImage *ptemp, bool bIsKeyword)
{
	m_cutImg = ptemp;
	m_bKeyWordSearch = bIsKeyword;
	if (m_pTemplete != NULL){
		cvReleaseImage(&m_pTemplete);
	}
	m_pTemplete = cvCreateImage(cvGetSize(m_cutImg), IPL_DEPTH_8U, 1);
	m_bIsTemplateCreated = false;



	m_searchCnt = 0;
	SetTimer(_SEARCHIMG, 10, NULL);

	CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
	pM->AddOutputString(_T("Start Search Process...."));
}

void CImageView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (select_object_2d(point.x, point.y, 2, 2, _PICK_SELECT) > 0){
		if (m_pCurrSelImg != NULL){
			m_pCurrSelImg->SetSelecttion(false);
		}
		m_pCurrSelImg = GetSNImageByIndex((int)m_sellBuffer[3]);
		m_pCurrSelImg->SetSelecttion(true);	

		pView->ShowCNSDlg(m_Threshold*100);
		pView->SetDlgImagePath(m_pCurrSelImg->GetPath());
	}

	COGLWnd::OnLButtonDblClk(nFlags, point);
}


void CImageView::ClearMatchingResult()
{
	for (int i = 0; i < m_vecImageData.size(); i++){
		m_vecImageData[i]->ClearMatchResult();
	}

}


void CImageView::ProcCutNSearchBinary()
{
	CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
	int cnt = 0;

	int search_size = m_cutImg->width;
	if (search_size > m_cutImg->height)
		search_size = m_cutImg->height;

	IplImage* cut_bw = cvCreateImage(cvGetSize(m_cutImg), IPL_DEPTH_8U, 1);

	cvThreshold(m_cutImg, cut_bw, 128, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	cvShowImage("cut_bw", cut_bw);

	//cvCopy(cut_bw, pTemplete);
	//cvShowImage("Modified Templete", pTemplete);


	for (; m_searchCnt < m_vecImageData.size(); m_searchCnt++){
		CSNImage* pImg = m_vecImageData[m_searchCnt];
		if (pImg->GetTxTex() == 0)
			continue;
		//	LoadSNImage(pImg->GetPath(), pImg, 128);

		USES_CONVERSION;
		char* sz = T2A(pImg->GetPath());
		//=================================================================================================================
		IplImage *gray = cvLoadImage(sz, CV_LOAD_IMAGE_GRAYSCALE);
		if (gray){
			//	cvShowImage("gray", gray);

			IplImage* im_bw = cvCreateImage(cvGetSize(gray), IPL_DEPTH_8U, 1);
			cvThreshold(gray, im_bw, 128, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
			cvShowImage("im_bw", im_bw);

			IplImage *result_img = cvCreateImage(cvSize(gray->width - m_cutImg->width + 1, gray->height - cut_bw->height + 1),
				IPL_DEPTH_32F, 1);

			cvMatchTemplate(im_bw, cut_bw, result_img, CV_TM_CCOEFF_NORMED);
			cvShowImage("percentage map", result_img);

			float* d = (float*)result_img->imageData;
			//	float fTh = 0.75f;
			//	std::vector<POINT3D> tmp_result;
			for (int y = 0; y < result_img->height; y++){
				for (int x = 0; x < result_img->width; x++){
					float fD = *(d + y*result_img->width + x);
					if (fD > m_Threshold)	{
						POINT3D left_top;
						left_top.x = x + m_cutImg->width*0.5f;
						left_top.y = y + m_cutImg->height*0.5f;
						left_top.z = fD;

						pImg->AddMatchedPoint(left_top, search_size);

					}
				}
			}
			//		cvReleaseImage(&dstImg);
			cvReleaseImage(&result_img);
			cvReleaseImage(&gray);
			cvReleaseImage(&im_bw);
			//================================================================//	
			//if (tmp_result.size() > 0){
			//	pImg->SetBgColor(0.9f, 0.7f, 0.2f);
			//}
		}

		cnt++;

		if (cnt > 10){
			break;
		}
	}

	if (m_searchCnt >= m_vecImageData.size()){
		KillTimer(_SEARCHIMG);
		//==========================//
		//cvZero(m_pTemplete);		
		float complete = (float)m_searchCnt / (float)m_vecImageData.size();

		CString str;
		str.Format(_T("Searching images.....%d"), int(complete * 100));
		str += _T("%");
		str += _T(" completed.");

		pM->AddOutputString(str, true);
	}

}


IplImage* CImageView::ExtractAverTempleteFromResult()
{
	double* dBuffer = new double[m_pTemplete->width * m_pTemplete->height];
	memset(dBuffer, 0x00, sizeof(double) * m_pTemplete->width * m_pTemplete->height);

	int totalCnt = 0;
	for (int k = 0; k < m_vecImageData.size(); k++){

		CSNImage* pImg = m_vecImageData[k];
		int matchCnt = pImg->GetMatchResult()->size();

		if (matchCnt > 0){
			USES_CONVERSION;
			char* sz = T2A(pImg->GetPath());
			//=================================================================================================================
			IplImage *gray = cvLoadImage(sz, CV_LOAD_IMAGE_GRAYSCALE);
			//	cvShowImage("gray", gray);
			if (gray){

				//IplImage* im_bw = cvCreateImage(cvGetSize(gray), IPL_DEPTH_8U, 1);
				//cvThreshold(gray, im_bw, 128, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
				//	cvShowImage("im_bw", im_bw);

				IplImage* pTmp = cvCreateImage(cvSize(m_cutImg->width, m_cutImg->height), gray->depth, gray->nChannels);

				for (int i = 0; i < pImg->GetMatchResult()->size(); i++){
					POINT3D pos = pImg->GetMatchResult()->at(i);
					pos.x -= m_cutImg->width*0.5f;
					pos.y -= m_cutImg->height*0.5f;

					cvZero(pTmp);
					cvSetImageROI(gray, cvRect(pos.x, pos.y, m_cutImg->width, m_cutImg->height));		// posx, posy = left - top
					cvCopy(gray, pTmp);

					cvShowImage("pTmp", pTmp);

					//float* d = (float*)m_pTemplete->imageData;
					double d = 0;
					for (int y = 0; y < pTmp->height; y++){
						for (int x = 0; x < pTmp->width; x++){
							dBuffer[y*pTmp->width + x] += (unsigned char)pTmp->imageData[y*pTmp->width + x];
						}
					}
					totalCnt++;
				}
				cvReleaseImage(&pTmp);
				cvReleaseImage(&gray);

			}
		}
	}

	for (int y = 0; y < m_pTemplete->height; y++){
		for (int x = 0; x < m_pTemplete->width; x++){
			m_pTemplete->imageData[y*m_pTemplete->width + x] = (char)(dBuffer[y*m_pTemplete->width + x] / (double)totalCnt);
		}
	}

	cvShowImage("Template", m_pTemplete);
	delete[] dBuffer;
		//m_bIsTemplateCreated = true;

		//ClearMatchingResult();
		//m_Threshold = 0.65f;
		////=============================//
		//m_cutImg = m_pTemplete;
		//m_searchCnt = 0;
		//SetTimer(_SEARCHIMG, 10, NULL);

		////	CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
		//pM->AddOutputString(_T("Start Deep Search Process...."));

	return m_pTemplete;

}