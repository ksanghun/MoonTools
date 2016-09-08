#include "stdafx.h"
#include "SNImage.h"




CSNImage::CSNImage()
{
	parentCode = 0;
	nWidth=0;
	nHeight=0;
	fARatio=0.0f;

	thTexId = texId = 0;
	m_fRectWidth = 0.0f;

	mtSetPoint3D(&m_vBgColor, 1.0f, 1.0f, 1.0f);
	mtSetPoint3D(&m_pos, -10000, 10000, 0.0f);

	m_fXScale = 1.0f;
	m_fYScale = 1.0f;
}


CSNImage::~CSNImage()
{
	if (texId > 0){
		glDeleteTextures(1, &texId);
	}
	if (thTexId > 0){
		glDeleteTextures(1, &thTexId);
	}
}

//void CSNImage::SetBgColor(float r, float g, float b)
//{
//	//mtSetPoint3D(&m_vBgColor, r,g,b);
//}

void CSNImage::AddMatchedPoint(POINT3D pos, int search_size)
{
	if (IsDuplicate(pos, search_size) == false){
		m_matched_pos.push_back(pos);
	}
}

void CSNImage::SetName(CString _strpath, CString _strpname, CString _strname, unsigned long _pcode, unsigned long _nCode)
{
	strPath = _strpath;
	strPName = _strpname;
	strName = _strname;
	parentCode = _pcode;
	nCode = _nCode;

	mtSetPoint3D(&m_pos, 0.0f, 0.0f, 0.0f);

}
void CSNImage::SetSize(unsigned short _w, unsigned short _h, float _size)
{
	nWidth = _w;
	nHeight = _h;
	fARatio = (float)_w / (float)_h;
	float w, h;

	if (_size > 0){
		if (fARatio <= 1.0f){
			w = _size*fARatio*0.4f;
			h = _size*0.4f;
		}
		else{
			w = _size*0.4f;
			h = (_size / fARatio)*0.4f;
		}
	}
	else{
		w = _w*0.5f;
		h = _h*0.5f;
	}
	
	m_fXScale = w*2.0f / (float)nImgWidth;
	m_fYScale = h*2.0f / (float)nImgHeight;

	mtSetPoint3D(&m_vertex[0], -w, -h, 0.0f);		mtSetPoint2D(&m_texcoord[0], 0.0f, 1.0f);
	mtSetPoint3D(&m_vertex[1], w, -h, 0.0f);		mtSetPoint2D(&m_texcoord[1], 1.0f, 1.0f);
	mtSetPoint3D(&m_vertex[2], w, h, 0.0f);		mtSetPoint2D(&m_texcoord[2], 1.0f, 0.0f);
	mtSetPoint3D(&m_vertex[3], -w, h, 0.0f);		mtSetPoint2D(&m_texcoord[3], 0.0f, 0.0f);


	mtSetPoint3D(&m_vertexBg[0], -_size*0.5f, -_size*0.5f, 0.0f);
	mtSetPoint3D(&m_vertexBg[1], _size*0.5f, -_size*0.5f, 0.0f);
	mtSetPoint3D(&m_vertexBg[2], _size*0.5f, _size*0.5f, 0.0f);
	mtSetPoint3D(&m_vertexBg[3], -_size*0.5f, _size*0.5f, 0.0f);

	m_fRectWidth = _size*fARatio;
}


void CSNImage::DrawForPicking()
{
	glPushMatrix();
	glTranslatef(m_pos.x, m_pos.y, m_pos.z);

	// Background//	
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	glVertex3f(m_vertexBg[0].x, m_vertexBg[0].y, m_vertexBg[0].z);
	glVertex3f(m_vertexBg[1].x, m_vertexBg[1].y, m_vertexBg[1].z);
	glVertex3f(m_vertexBg[2].x, m_vertexBg[2].y, m_vertexBg[2].z);
	glVertex3f(m_vertexBg[3].x, m_vertexBg[3].y, m_vertexBg[3].z);
	glEnd();
	//==================//
	glPopMatrix();

}

void CSNImage::SetSelecttion(bool _isSel) 
{ 
	if (_isSel == true){
		mtSetPoint3D(&m_vBgColor, 0.1f, 0.99f, 0.1f);
	}
	else{
		mtSetPoint3D(&m_vBgColor, 1.0f, 1.0f, 1.0f);
	}
	m_bIsSelected = _isSel; 
};

void CSNImage::SetTexId(GLuint _texid)
{
	if (_texid != texId){
		if (texId > 0){
			glDeleteTextures(1, &texId);
		}
		texId = _texid;}	

}

void CSNImage::DrawImage(float fAlpha)
{
	glPushMatrix();
	glTranslatef(m_pos.x, m_pos.y, m_pos.z);

	glBindTexture(GL_TEXTURE_2D, texId);
	glColor4f(1.0f, 1.0f, 1.0f, fAlpha);

	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glTexCoord2f(m_texcoord[0].x, m_texcoord[0].y);
	glVertex3f(m_vertex[0].x, m_vertex[0].y, m_vertex[0].z);

	glTexCoord2f(m_texcoord[1].x, m_texcoord[1].y);
	glVertex3f(m_vertex[1].x, m_vertex[1].y, m_vertex[1].z);

	glTexCoord2f(m_texcoord[2].x, m_texcoord[2].y);
	glVertex3f(m_vertex[2].x, m_vertex[2].y, m_vertex[2].z);

	glTexCoord2f(m_texcoord[3].x, m_texcoord[3].y);
	glVertex3f(m_vertex[3].x, m_vertex[3].y, m_vertex[3].z);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}


void CSNImage::DrawThumbNail(float fAlpha)
{
	if (thTexId == 0){
		return;
	}
	glPushMatrix();
	glTranslatef(m_pos.x, m_pos.y, m_pos.z);

	// Background//
//	glDisable(GL_TEXTURE_2D);
	glColor4f(m_vBgColor.x, m_vBgColor.y, m_vBgColor.z, 1.0f);
	glBegin(GL_QUADS);	
	glVertex3f(m_vertexBg[0].x, m_vertexBg[0].y, m_vertexBg[0].z);
	glVertex3f(m_vertexBg[1].x, m_vertexBg[1].y, m_vertexBg[1].z);
	glVertex3f(m_vertexBg[2].x, m_vertexBg[2].y, m_vertexBg[2].z);
	glVertex3f(m_vertexBg[3].x, m_vertexBg[3].y, m_vertexBg[3].z);
	glEnd();
	//==================//


	glEnable(GL_TEXTURE_2D);
	if (texId == 0){
		glBindTexture(GL_TEXTURE_2D, thTexId);
	}
	else{
		glBindTexture(GL_TEXTURE_2D, texId);
	}
	glColor4f(1.0f, 1.0f, 1.0f, fAlpha);	

	glBegin(GL_QUADS);

	glTexCoord2f(m_texcoord[0].x, m_texcoord[0].y);
	glVertex3f(m_vertex[0].x, m_vertex[0].y, m_vertex[0].z);

	glTexCoord2f(m_texcoord[1].x, m_texcoord[1].y);
	glVertex3f(m_vertex[1].x, m_vertex[1].y, m_vertex[1].z);

	glTexCoord2f(m_texcoord[2].x, m_texcoord[2].y);
	glVertex3f(m_vertex[2].x, m_vertex[2].y, m_vertex[2].z);

	glTexCoord2f(m_texcoord[3].x, m_texcoord[3].y);
	glVertex3f(m_vertex[3].x, m_vertex[3].y, m_vertex[3].z);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	
	
	// Detected //
	if (m_matched_pos.size() > 0 ){
		glColor4f(1.0f, 0.2f, 0.1f, fAlpha);
		glBegin(GL_LINE_STRIP);		
		glVertex3f(m_vertex[0].x, m_vertex[0].y, m_vertex[0].z);	
		glVertex3f(m_vertex[1].x, m_vertex[1].y, m_vertex[1].z);	
		glVertex3f(m_vertex[2].x, m_vertex[2].y, m_vertex[2].z);	
		glVertex3f(m_vertex[3].x, m_vertex[3].y, m_vertex[3].z);
		glVertex3f(m_vertex[0].x, m_vertex[0].y, m_vertex[0].z);
		glEnd();

		// Draw detected position //
		glColor4f(1.0f, 0.2f, 0.1f, 0.7f);
		glPushMatrix();		
			glScalef(m_fXScale, m_fYScale, 1.0f);
			glTranslatef(-nImgWidth*0.5f, -nImgHeight*0.5f, 0.0f);
			glBegin(GL_POINTS);
			for (int i = 0; i < m_matched_pos.size(); i++){
				glVertex3f(m_matched_pos[i].x, nImgHeight - m_matched_pos[i].y, 0.0f);
			}
			glEnd();
		glPopMatrix();

	}


	glPopMatrix();

}


void CSNImage::SetThTex(GLuint _thtex)
{
	thTexId = _thtex;
}
void CSNImage::SetTex(GLuint _tex)
{
	texId = _tex;
}

void CSNImage::SetPosition(POINT3D pos)
{
	m_pos = pos;
}

void CSNImage::ClearMatchResult()
{
	m_matched_pos.clear();
}

bool CSNImage::IsDuplicate(POINT3D pos, int search_size)
{
	bool IsDup = false;
	for (int i = 0; i < m_matched_pos.size(); i++){
		float fDist = mtDistance(pos, m_matched_pos[i]);
		if (fDist < search_size){
			IsDup = true;
			break;
		}
	}
	return IsDup;
}