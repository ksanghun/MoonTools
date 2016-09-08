#include "stdafx.h"
#include "CNSDlgView.h"
#include "CutNSearchView.h"


CCNSDlgView::CCNSDlgView()
{
	m_fScreenScale = 1.0f;
	m_pSrcImage = NULL;

	memset(&m_LogFont, 0, sizeof(m_LogFont));

	lstrcpy(m_LogFont.lfFaceName, _T("Consolas"));
	m_LogFont.lfCharSet = ANSI_CHARSET;
	m_LogFont.lfHeight = -14;
	m_LogFont.lfWidth = 0;

	m_pBmpInfo = (BITMAPINFO *)malloc(sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD) * 256));

	m_strMousePos = "";
	mtSetPoint3D(&m_mousePos, 0.0f, 0.0f, 0.0f);

	m_fXScale = 1.0f;
	m_fYScale = 1.0f;

	m_bCNSReady = false;
	m_pCutImg = NULL;

}


CCNSDlgView::~CCNSDlgView()
{
	delete m_pBmpInfo;
}

BEGIN_MESSAGE_MAP(CCNSDlgView, COGLWnd)
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()



void CCNSDlgView::Render()
{
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Render2D();

	SwapBuffers(m_CDCPtr->GetSafeHdc());
}

void CCNSDlgView::Render2D()
{
	gl_PushOrtho(m_left, m_right, m_bottom, m_top);

	if (m_pSrcImage){
		m_pSrcImage->DrawImage(1.0f);


		glColor3f(0.2f, 0.2f, 0.2f);
		gl_DrawText(m_mousePos, m_strMousePos, m_LogFont, 1, m_pBmpInfo, m_CDCPtr);
		DrawGuideLine();
	}
	else{
		glColor3f(0.2f, 0.2f, 0.2f);
		gl_DrawText(m_vScrCenter, _T("Drag & Drop a source image here..."), m_LogFont, 1, m_pBmpInfo, m_CDCPtr);
	}



	gl_PopOrtho();
}

void CCNSDlgView::DrawGuideLine()
{

	float height = m_nHeight * m_fYScale;
	float width = m_nWidth * m_fXScale;
	glPushMatrix();
	glScalef(1.0f / m_fXScale, 1.0f / m_fYScale, 1.0f);


	glColor4f(0.1f, 0.5f, 0.99f, 0.5f);

	glBegin(GL_LINES);
	glVertex3f(m_vSelPos.x, 0.0f, 0.0f);
	glVertex3f(m_vSelPos.x, height, 0.0f);
	glVertex3f(0.0f, height - m_vSelPos.y, 0.0f);
	glVertex3f(width, height - m_vSelPos.y, 0.0f);
	glEnd();


	// Draw selection rect //
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glBegin(GL_LINE_STRIP);

	glVertex3f(m_selRect.x1, height - m_selRect.y1, 0.0f);
	glVertex3f(m_selRect.x2, height - m_selRect.y1, 0.0f);
	glVertex3f(m_selRect.x2, height - m_selRect.y2, 0.0f);
	glVertex3f(m_selRect.x1, height - m_selRect.y2, 0.0f);
	glVertex3f(m_selRect.x1, height - m_selRect.y1, 0.0f);
	glEnd();

	glPopMatrix();

}



void CCNSDlgView::InitGLview(int _nWidth, int _nHeight)
{
	m_nWidth = _nWidth;
	m_nHeight = _nHeight;


	//m_lookAt.x = 0;
	//m_lookAt.y = 0;
	//m_lookAt.z = 0;
	//m_cameraPri.InitializeCamera(30.0f, 0.0f, 0.0f, m_lookAt, _nWidth, _nHeight);
	//m_cameraPri.SetInitLevelHeight(1000);

	glDisable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);


	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);



	//	SetTimer(100, 10, NULL);
}

void CCNSDlgView::OnSize(UINT nType, int cx, int cy)
{
	COGLWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);
	m_nWidth = cx;
	m_nHeight = cy;

	m_right = m_left + m_nWidth;
	m_top = m_bottom + m_nHeight;

	mtSetPoint3D(&m_vScrCenter, m_nWidth*0.5f, m_nHeight*0.5f, 0.0f);

	if (m_pSrcImage){
		POINT3D pos;
		mtSetPoint3D(&pos, m_nWidth*0.5f, m_nHeight*0.5f, 0.0f);
		m_pSrcImage->SetPosition(pos);
		m_pSrcImage->SetSize(m_nWidth, m_nHeight, 0);

		m_fXScale = (float)m_pSrcImage->GetImgWidth() / (float)m_nWidth;
		m_fYScale = (float)m_pSrcImage->GetImgHeight() / (float)m_nHeight;
	}

	m_cameraPri.SetProjectionMatrix(30.0f, 0.0f, 0.0f, cx, cy);
	m_cameraPri.SetModelViewMatrix(m_cameraPri.GetLookAt(), 0.0f, 0.0f);
}


void CCNSDlgView::MouseWheel(short zDelta)
{

}

bool CCNSDlgView::LoadSNImage(CString strPath, unsigned short &_width, unsigned short &_height)
{
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);

	if (m_pSrcImage != NULL){
		delete m_pSrcImage;
		m_pSrcImage = NULL;
	}
	m_pSrcImage = new CSNImage;

	m_pSrcImage->SetName(strPath, _T(""), _T(""), 0, 0);

	USES_CONVERSION;
	char* sz = T2A(strPath);

	IplImage *pimg = cvLoadImage(sz);
	if (pimg){
		//	cvShowImage(sz, pimg);
		cvCvtColor(pimg, pimg, CV_BGR2RGB);

		m_pSrcImage->SetImgSize(pimg->width, pimg->height);
		m_pSrcImage->SetSize(pimg->width, pimg->height, 0);
		

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
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, pimg->width, pimg->height, GL_RGB, GL_UNSIGNED_BYTE, pimg->imageData);
		//======================================================================================//

		m_pSrcImage->SetTexId(tid);

		POINT3D pos;
		mtSetPoint3D(&pos, pimg->width*0.5f, pimg->height*0.5f, 0.0f);
		m_pSrcImage->SetPosition(pos);


		_width = pimg->width;
		_height = pimg->height;

		cvReleaseImage(&pimg);
		return true;
	}

	return false;
}

void CCNSDlgView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);

	m_mousePos.x = m_nWidth - 40;;
	m_mousePos.y = 20;


	m_vSelPos.x = (float)point.x * m_fXScale;
	m_vSelPos.y = (float)point.y * m_fYScale;
	m_strMousePos.Format(_T("[%d, %d]"), m_vSelPos.x, m_vSelPos.y);


	if (GetCapture()){
		m_selRect.x2 = m_vSelPos.x;
		m_selRect.y2 = m_vSelPos.y;
	}


	Render();

	COGLWnd::OnMouseMove(nFlags, point);
}



void CCNSDlgView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default


	m_selRect.set(0, 0, 0, 0);
	m_bCNSReady = false;

	SetCapture();

	m_vSelPos.x = (float)point.x * m_fXScale;
	m_vSelPos.y = (float)point.y * m_fYScale;

	m_selRect.x1 = m_vSelPos.x;
	m_selRect.y1 = m_vSelPos.y;
	m_selRect.x2 = m_vSelPos.x;
	m_selRect.y2 = m_vSelPos.y;


	COGLWnd::OnLButtonDown(nFlags, point);
}


void CCNSDlgView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if (GetCapture()){

		RECT2D tmp = m_selRect;
		if (m_selRect.x1 > m_selRect.x2){
			tmp.x1 = m_selRect.x2;
			tmp.x2 = m_selRect.x1;
		}
		if (m_selRect.y1 > m_selRect.y2){
			tmp.y1 = m_selRect.x2;
			tmp.y2 = m_selRect.x1;
		}

		if (((tmp.x2 - tmp.x1) < 8) || ((tmp.y2 - tmp.y1) < 8)){
			m_selRect.set(0, 0, 0, 0);
			m_bCNSReady = false;
		}
		else{
			m_selRect = tmp;
			m_selRect.width = tmp.x2 - tmp.x1;
			m_selRect.height = tmp.y2 - tmp.y1;
			m_bCNSReady = true;
		}
		ReleaseCapture();
	}


	COGLWnd::OnLButtonUp(nFlags, point);
}

void CCNSDlgView::DoCNSearch()
{
	if (m_bCNSReady){

		if (m_pCutImg != NULL){
			cvReleaseImage(&m_pCutImg);
			m_pCutImg = NULL;
		}

		USES_CONVERSION;
		char* sz = T2A(m_pSrcImage->GetPath());

		IplImage *pSrc = cvLoadImage(sz);
		if (pSrc != NULL){
			IplImage *gray = cvCreateImage(cvSize(pSrc->width, pSrc->height), 8, 1);
			if (pSrc->nChannels == 3){
				cvCvtColor(pSrc, gray, CV_RGB2GRAY);//Change from RGB to GrayScale
			}
			else{
				cvCopy(pSrc, gray);
			}

			m_pCutImg = cvCreateImage(cvSize(m_selRect.width, m_selRect.height), gray->depth, gray->nChannels);

			cvSetImageROI(gray, cvRect(m_selRect.x1, m_selRect.y1, m_selRect.width, m_selRect.height));		// posx, posy = left - top
			cvCopy(gray, m_pCutImg);
			//	cvResetImageROI(gray);


			//cvShowImage("crop", m_pCutImg);

			cvReleaseImage(&pSrc);
			cvReleaseImage(&gray);		


			// Search in destate image //
			pView->DoCNSearch(m_pCutImg);		// Search destinatio images using cut templete.		


		}
	}

}