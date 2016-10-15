#pragma once

#include "math_tool.h"


class CSNImage
{
public:
	CSNImage();
	~CSNImage();

	void SetName(CString _strpath, CString _strpname, CString _strname, unsigned long _pcode, unsigned long _nCode);
	void SetSize(unsigned short _w, unsigned short _h, float _size);
	void SetThTex(GLuint _thtex);
	void SetTexId(GLuint _texid);
	void SetTex(GLuint _tex);
	void SetPosition(POINT3D pos);
	void SetSelecttion(bool _isSel);
	void SetImgSize(unsigned short _w, unsigned long _h) { nImgWidth = _w; nImgHeight = _h; };
//	void SetBgColor(float r, float g, float b);

	void DrawThumbNail(float fAlpha);
	void DrawForPicking();
	void DrawImage(float fAlpha);
	void CSNImage::DrawBMPText();

	bool AddMatchedPoint(_MATCHInfo info, int search_size);

	GLuint GetTexId() { return texId; };
	GLuint GetTxTex() { return thTexId; };
	CString GetPath() { return strPath; };
	unsigned short GetWidth() { return nWidth; };
	unsigned short GetHeight() { return nHeight; };
	POINT3D GetPos() { return m_pos; };
	unsigned short GetImgWidth() { return nImgWidth; };
	unsigned short GetImgHeight() { return nImgHeight; };

	bool IsDuplicate(POINT3D pos, int search_size);


	void ClearMatchResult();
	std::vector<_MATCHInfo>* GetMatchResult() { return &m_matched_pos; };

private:
	CString strPath;
	CString strPName;
	CString strName;

	unsigned long parentCode;
	unsigned long nCode;

	unsigned short nWidth;
	unsigned short nHeight;
	float fARatio;

	GLuint thTexId, texId;

	float	m_fRectWidth;
	POINT3D m_pos;
	POINT3D m_vertex[4];
	POINT3D m_vertexBg[4];
	POINT2D m_texcoord[4];

	POINT3D m_vBgColor;
	bool	m_bIsSelected;


	unsigned short nImgWidth;
	unsigned short nImgHeight;


	std::vector<_MATCHInfo> m_matched_pos;

	float m_fXScale, m_fYScale;


};

