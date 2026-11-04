#include "3DEffect.h"
#include "3DObjX.h"
#include "MyHeap.h"

class C3DEffectX : public I3DEffect
{
public:
	C3DEffectX();
	virtual ~C3DEffectX();
	static void* operator new(size_t uSize) throw()	{ return s_heap.Alloc(uSize); }
	static void	 operator delete(void* p)	{ s_heap.Free(p); }
	static CMyHeap	s_heap;

private:
	DWORD		m_dwShapes;
	C3Shape		*m_Shape[_EFFECT_PART_MAX_];
	C3SMotion	*m_SMotion[_EFFECT_PART_MAX_];

	DWORD		m_dwPtcls;
	C3Ptcl		*m_Ptcl[_EFFECT_PART_MAX_];

	DWORD		m_dwPtclExs;
	C3PtclEx	*m_PtclEx[_EFFECT_PART_MAX_];

	DWORD		m_dwPtcl3s;
	C3Ptcl3		*m_Ptcl3[_EFFECT_PART_MAX_];

	DWORD		m_dwPhys;
	C3Phy		*m_Phy[_EFFECT_PART_MAX_];
	C3Motion	*m_Motion[_EFFECT_PART_MAX_];

	DWORD		m_dwStepCount;
	D3DXMATRIXA16	matrix;
	D3DMATERIAL8	m_material;

public:
	virtual DWORD	Release ( void );
	virtual BOOL	Load ( char *lpFilename );
	virtual void	Unload ( void );
	virtual void	Bind ( I3DObj *lpObj );
	virtual void	Draw ( BOOL bNextFrame, int nAsb, int nAdb );
	virtual void	SetTexture ( I3DTexture* pTexture );
	virtual void	ResetShape ( void );
	virtual void	Move ( float x, float y, float z );
	virtual void	Rotate ( float x, float y, float z );
	virtual void	Scale ( float x, float y, float z );
	virtual void	BindMatrix ( D3DXMATRIXA16 *lpMatrix );
	virtual void	SetColor ( DWORD dwColor );
	virtual DWORD	GetPtclAmount ()	{return m_dwPtcls;}
	virtual void	SetPtclCurFrame ( DWORD dwIndex, DWORD dwFrame );
	virtual DWORD	GetPtclFrames ( DWORD dwIndex );
	virtual DWORD	GetPhyAmount ()		{return m_dwPhys;}
	virtual void	SetPhyCurFrame ( DWORD dwIndex, DWORD dwFrame );
	virtual DWORD	GetPhyFrames ( DWORD dwIndex );
	virtual DWORD	GetFrames ();
	virtual void	SetFrames ( DWORD dwFrame );
	virtual DWORD	GetShapeAmount ()	{return m_dwShapes;}
	virtual ShapeBackupInfo*	CreateShapeInfo	(void);
	virtual void	BackupShapeInfo		(ShapeBackupInfo* pBuf);
	virtual void	RestoreShapeInfo	(ShapeBackupInfo* pBuf);
	virtual PTCL_INFO*	CreatePtclInfo	(void);
	virtual void	SetPtclInfo			(PTCL_INFO* pInfo);
	virtual PTCL3_INFO*	CreatePtcl3Info	(void);
	virtual void	SetPtcl3Info		(PTCL3_INFO* pInfo);
	virtual void	SetPtcl3Gravity		(float x, float y, float z);
	virtual void	SetPtcl3ExternalSpeed (float x, float y, float z);
	virtual void	SetStepCount		(DWORD dwStep);
};

CMyHeap C3DEffectX::s_heap("C3DEffectX");

C3DEffectX::C3DEffectX()
{
	m_dwShapes = 0;
	m_dwPtcls = 0;
	m_dwPtclExs = 0;
	m_dwPtcl3s = 0;
	m_dwStepCount = 0;

	for ( int n = 0; n < _EFFECT_PART_MAX_; n++ )
	{
		m_Shape[n] = 0;
		m_SMotion[n] = 0;
		m_Ptcl[n] = 0;
		m_PtclEx[n] = 0;
		m_Ptcl3[n] = 0;
		m_Phy[n] = 0;
		m_Motion[n] = 0;
	}

	D3DXMatrixIdentity ( &matrix );

	ZeroMemory ( &m_material, sizeof ( m_material ) );
	m_material.Diffuse.r = m_material.Diffuse.g = m_material.Diffuse.b = m_material.Diffuse.a = 1.0f;
	m_material.Ambient.r = m_material.Ambient.g = m_material.Ambient.b = m_material.Ambient.a = 1.0f;
	m_material.Specular.r = m_material.Specular.g = m_material.Specular.b = m_material.Specular.a = 1.0f;
}

C3DEffectX::~C3DEffectX()
{
	this->Unload();
}

DWORD C3DEffectX::Release ( void )
{
	delete this;
	return 0;
}
BOOL C3DEffectX::Load ( char *lpFilename )
{
	if ( !lpFilename )
		return false;

	m_dwShapes = 0;
	m_dwPtcls = 0;
	m_dwPtclExs = 0;
	m_dwPtcl3s = 0;
	m_dwPhys = 0;

	DWORD dwSize = 0;
	void *f = Common_OpenRes ( lpFilename, dwSize, 1 );
	if ( !f )
		return false;

	int ms = 0, sms = 0;
	ChunkHeader chunk;
	long nStart = Common_GetSeek ( f );
	Common_SeekRes ( f, 16, SEEK_CUR );
	while ( ( DWORD )Common_GetSeek ( f ) < nStart + dwSize )
	{
		Common_GetChunk ( f, &chunk );
		if ( chunk.byChunkID[0] == 'P' &&
			 chunk.byChunkID[1] == 'H' &&
			 chunk.byChunkID[2] == 'Y' &&
			 ( chunk.byChunkID[3] == ' ' || chunk.byChunkID[3] == '2' || chunk.byChunkID[3] == '3' || chunk.byChunkID[3] == '4' ) )
		{
			if ( m_Phy[m_dwPhys] )
				Phy_Unload ( &m_Phy[m_dwPhys] );
			BOOL bLoad = false;
			if ( chunk.byChunkID[3] == ' ' )
				bLoad = Phy_Load ( &m_Phy[m_dwPhys], f, false, false, false, true );
			else if ( chunk.byChunkID[3] == '2' )
				bLoad = Phy_Load ( &m_Phy[m_dwPhys], f, true, false, false, true );
			else if ( chunk.byChunkID[3] == '3' )
				bLoad = Phy_Load ( &m_Phy[m_dwPhys], f, true, false, false, false );
			else
				bLoad = Phy_Load ( &m_Phy[m_dwPhys], f, false, false, false, false );
			if ( !bLoad )
				break;
			m_dwPhys++;
			if ( m_dwPhys >= _EFFECT_PART_MAX_ )
				break;
		}
		else
		if ( chunk.byChunkID[0] == 'M' &&
			 chunk.byChunkID[1] == 'O' &&
			 chunk.byChunkID[2] == 'T' &&
			 chunk.byChunkID[3] == 'I' )
		{
			if ( m_Motion[ms] )
				Motion_Unload ( &m_Motion[ms] );
			if ( !Motion_Load ( &m_Motion[ms], f ) )
				break;
			ms++;
		}
		else
		if ( chunk.byChunkID[0] == 'S' &&
			 chunk.byChunkID[1] == 'H' &&
			 chunk.byChunkID[2] == 'A' &&
			 chunk.byChunkID[3] == 'P' )
		{
			if ( m_Shape[m_dwShapes] )
				Shape_Unload ( &m_Shape[m_dwShapes] );
			if ( !Shape_Load ( &m_Shape[m_dwShapes], f, 0 ) )
				break;
			m_dwShapes++;
			if ( m_dwShapes >= _EFFECT_PART_MAX_ )
				break;
		}
		else
		if ( chunk.byChunkID[0] == 'P' &&
			 chunk.byChunkID[1] == 'T' &&
			 chunk.byChunkID[2] == 'C' &&
			 chunk.byChunkID[3] == 'L' )
		{
			if ( m_Ptcl[m_dwPtcls] )
				Ptcl_Unload ( &m_Ptcl[m_dwPtcls] );
			if ( !Ptcl_Load ( &m_Ptcl[m_dwPtcls], f, 0 ) )
				break;
			m_dwPtcls++;
			if ( m_dwPtcls >= _EFFECT_PART_MAX_ )
				break;
		}
		else
		if ( chunk.byChunkID[0] == 'P' &&
			 chunk.byChunkID[1] == 'T' &&
			 chunk.byChunkID[2] == 'C' &&
			 chunk.byChunkID[3] == 'X' )
		{
			if ( m_PtclEx[m_dwPtclExs] )
				Ptcl_Unload ( &m_PtclEx[m_dwPtclExs] );
			if ( !Ptcl_Load ( &m_PtclEx[m_dwPtclExs], f, 0 ) )
				break;
			m_dwPtclExs++;
			if ( m_dwPtclExs >= _EFFECT_PART_MAX_ )
				break;
		}
		else
		if ( chunk.byChunkID[0] == 'P' &&
			 chunk.byChunkID[1] == 'T' &&
			 chunk.byChunkID[2] == 'C' &&
			 chunk.byChunkID[3] == '3' )
		{
			if ( m_Ptcl3[m_dwPtcl3s] )
				Ptcl_Unload ( &m_Ptcl3[m_dwPtcl3s] );
			if ( !Ptcl_Load ( &m_Ptcl3[m_dwPtcl3s], f, 0 ) )
				break;
			m_dwPtcl3s++;
			if ( m_dwPtcl3s >= _EFFECT_PART_MAX_ )
				break;
		}
		else
		if ( chunk.byChunkID[0] == 'S' &&
			 chunk.byChunkID[1] == 'M' &&
			 chunk.byChunkID[2] == 'O' &&
			 chunk.byChunkID[3] == 'T' )
		{
			if ( m_SMotion[sms] )
				SMotion_Unload ( &m_SMotion[sms] );
			if ( !SMotion_Load ( &m_SMotion[sms], f ) )
				break;
			sms++;
		}
		else
		{
			Common_SeekRes ( f, chunk.dwChunkSize, SEEK_CUR );
		}
	}
	Common_CloseRes ( f );

	DWORD n;
	for ( n = 0; n < m_dwPhys; n++ )
	{
		m_Phy[n]->lpMotion = m_Motion[n];
	}
	for ( n = 0; n < m_dwShapes; n++ )
	{
		m_Shape[n]->lpMotion = m_SMotion[n];
	}
	return true;
}
void C3DEffectX::Unload ( void )
{
	m_dwShapes = 0;
	m_dwPtcls = 0;
	m_dwPtclExs = 0;
	m_dwPtcl3s = 0;
	m_dwPhys = 0;

	for ( int n = 0; n < _EFFECT_PART_MAX_; n++ )
	{
		if ( m_Shape[n] )
			Shape_Unload ( &m_Shape[n] );

		if ( m_SMotion[n] )
			SMotion_Unload ( &m_SMotion[n] );

		if ( m_Ptcl[n] )
			Ptcl_Unload ( &m_Ptcl[n] );

		if ( m_PtclEx[n] )
			Ptcl_Unload ( &m_PtclEx[n] );

		if ( m_Ptcl3[n] )
			Ptcl_Unload ( &m_Ptcl3[n] );

		if ( m_Phy[n] )
			Phy_Unload ( &m_Phy[n] );

		if ( m_Motion[n] )
			Motion_Unload ( &m_Motion[n] );
	}
}
void C3DEffectX::Bind ( I3DObj *lpObj )
{
	C3DObjX* pObj = ( C3DObjX* )lpObj;
	this->BindMatrix ( &pObj->m_phy[0]->lpMotion->matrix[0] );
}
void C3DEffectX::BindMatrix ( D3DXMATRIXA16 *lpMatrix )
{
	CopyMemory ( &matrix, lpMatrix, sizeof ( D3DXMATRIXA16 ) );
}
void C3DEffectX::Draw ( BOOL bNextFrame,  int nAsb, int nAdb )
{
	DWORD n;
	if(m_dwShapes > 0)
	{
		if(s_nShowMode != modeShape)
		{
			Shape_Prepare (nAsb, nAdb);
			s_nShowMode = modeShape;
		}
		for ( n = 0; n < m_dwShapes; n++ )
		{
			if ( m_Shape[n] )
			{
				m_Shape[n]->lpMotion = m_SMotion[n];
				Shape_Muliply(m_Shape[n], &matrix);
				Shape_Draw ( m_Shape[n], false );
				Shape_ClearMatrix ( m_Shape[n] );
			}
		}
	}

	if(m_dwPtcls > 0)
	{
		if(s_nShowMode != modePtcl)
		{
			Ptcl_Prepare ();
			s_nShowMode = modePtcl;
		}
		for ( n = 0; n < m_dwPtcls; n++ )
		{
			if ( m_Ptcl[n] )
			{
				Ptcl_Muliply(m_Ptcl[n], &matrix);
				Ptcl_Draw ( m_Ptcl[n], nAsb, nAdb );
				Ptcl_ClearMatrix ( m_Ptcl[n] );
				if(bNextFrame)
					Ptcl_NextFrame ( m_Ptcl[n], 1 );
			}
		}
	}

	if(m_dwPtclExs > 0)
	{
		if(s_nShowMode != modePtcl)
		{
			Ptcl_Prepare ();
			s_nShowMode = modePtcl;
		}
		for ( n = 0; n < m_dwPtclExs; n++ )
		{
			if ( m_PtclEx[n] )
			{
				Ptcl_Muliply(m_PtclEx[n], &matrix);
				Ptcl_Draw ( m_PtclEx[n], nAsb, nAdb );
				Ptcl_ClearMatrix ( m_PtclEx[n] );
			}
		}
	}

	if(m_dwPtcl3s > 0)
	{
		if(s_nShowMode != modePtcl)
		{
			Ptcl_Prepare ();
			s_nShowMode = modePtcl;
		}
		for ( n = 0; n < m_dwPtcl3s; n++ )
		{
			if ( m_Ptcl3[n] )
			{
				Ptcl_Muliply( ( C3PtclEx* )m_Ptcl3[n], &matrix);
				Ptcl_Draw ( m_Ptcl3[n], nAsb, nAdb );
				Ptcl_ClearMatrix ( ( C3PtclEx* )m_Ptcl3[n] );
			}
		}
	}

	if(m_dwPhys > 0)
	{
		if(s_nShowMode != modeMesh)
		{
			Phy_Prepare ();
			s_nShowMode = modeMesh;
		}
		for ( n = 0; n < m_dwPhys; n++ )
		{
			if (m_Phy[n] && m_Motion[n])
			{
				m_Phy[n]->bMatrix = true;
				m_Phy[n]->lpMotion = m_Motion[n];
				D3DXVECTOR3 vLight ( 0.0f, 0.0f, 0.0f );
				if ( m_Phy[n]->nBillboard != 0 )
				{
					D3DXVECTOR3 vUp ( 0.0f, 0.0f, -1.0f );
					D3DXVECTOR3 vCamera ( g_InvViewMatrix._41, g_InvViewMatrix._42, g_InvViewMatrix._43 );
					D3DXVECTOR3 vPos ( matrix._41, matrix._42, matrix._43 );
					if ( m_Phy[n]->nBillboard == 2 )
						vUp = D3DXVECTOR3 ( matrix._21, matrix._22, matrix._23 );
					vLight = vCamera - vPos;
					D3DXVECTOR3 vDir, vRight, vUp2;
					D3DXVec3Normalize ( &vDir, &vLight );
					D3DXVec3Cross ( &vRight, &vDir, &vUp );
					D3DXVec3Cross ( &vUp2, &vUp, &vRight );
					D3DXMATRIXA16 mat;
					mat._11 = vRight.x;	mat._12 = vRight.y;	mat._13 = vRight.z;	mat._14 = 0.0f;
					mat._21 = vUp.x;	mat._22 = vUp.y;	mat._23 = vUp.z;	mat._24 = 0.0f;
					mat._31 = vUp2.x;	mat._32 = vUp2.y;	mat._33 = vUp2.z;	mat._34 = 0.0f;
					mat._41 = vPos.x;	mat._42 = vPos.y;	mat._43 = vPos.z;	mat._44 = 1.0f;
					CopyMemory ( &m_Phy[n]->matrix, &mat, sizeof ( D3DXMATRIXA16 ) );
				}
				else
					CopyMemory ( &m_Phy[n]->matrix, &matrix, sizeof ( D3DXMATRIXA16 ) );
				Phy_SetStepCount(m_Phy[n], m_dwStepCount);
				Phy_Calculate(m_Phy[n], false, 1, vLight);
				Phy_DrawNormal(m_Phy[n], m_material, NULL, false, nAsb, nAdb, 4, 0.35f);
				Phy_ClearMatrix(m_Phy[n]);
			}
		}
	}
	D3DXMatrixIdentity ( &matrix );
}
void C3DEffectX::SetTexture(I3DTexture* pTexture)
{
	if(!pTexture)
		return;

	DWORD n;
	C3Texture* pMyTexture = pTexture->GetTexture();
	int nTextureID = pMyTexture->nID;

	MYASSERT(pMyTexture);

	for(n = 0; n < m_dwShapes; n++)
	{
		if(m_Shape[n])
		{
			m_Shape[n]->nTex = nTextureID;
		}
	}

	for(n = 0; n < m_dwPtcls; n++)
	{
		if(m_Ptcl[n])
		{
			m_Ptcl[n]->nTex = nTextureID;
		}
	}

	for(n = 0; n < m_dwPtclExs; n++)
	{
		if(m_PtclEx[n])
		{
			m_PtclEx[n]->nTex = nTextureID;
		}
	}

	for(n = 0; n < m_dwPtcl3s; n++)
	{
		if(m_Ptcl3[n])
		{
			m_Ptcl3[n]->nTex = nTextureID;
		}
	}

	for(n = 0; n < m_dwPhys; n++)
	{
		if(m_Phy[n])
		{
			m_Phy[n]->nTex = nTextureID;
		}
	}
}
void C3DEffectX::Move(float x, float y, float z)
{
	Common_Translate(&matrix, x, y, z);
}
void C3DEffectX::Rotate(float x, float y, float z)
{
	Common_Rotate(&matrix, x, y, z);
}
void C3DEffectX::Scale(float x, float y, float z)
{
	Common_Scale(&matrix, x, y, z);
}
void C3DEffectX::SetColor(DWORD dwColor)
{
	if(m_dwPhys == 0)
		return;
	for(DWORD i = 0; i < m_dwPhys; i++)
	{
		if(m_Phy[i])
			Phy_SetColor(m_Phy[i], (dwColor >> 24) / 255.0f, ((dwColor >> 16) & 0xff) / 255.0f, ((dwColor >> 8) & 0xff) / 255.0f, (dwColor & 0xff) / 255.0f);
	}
}

void C3DEffectX::ResetShape ( void )
{
	for ( DWORD n = 0; n < m_dwShapes; n++ )
	{
		if ( m_Shape[n] )
		{
			m_Shape[n]->bFrist = true;
			m_Shape[n]->dwSegmentCur = 0;
		}
	}
}

ShapeBackupInfo* C3DEffectX::CreateShapeInfo(void)
{
	if (m_dwShapes <= 0)
		return NULL;

	ShapeBackupInfo* pBuf = new ShapeBackupInfo[m_dwShapes];
	if (!pBuf)
		return NULL;

	memset (pBuf, 0L, sizeof(ShapeBackupInfo)*m_dwShapes);

	for (int i=0; i<(int)m_dwShapes; i++)
	{
		MYASSERT (m_Shape[i]);

		ShapeBackupInfo* pInfo = pBuf+i;
		pInfo->vb = new ShapeOutVertex[m_Shape[i]->dwSegment*2+2];
	}

	return pBuf;
}

void DestroyShapeInfo (ShapeBackupInfo*& pBuf, int nShapes)
{
	if (!pBuf || nShapes<= 0)
		return;

	for (int i=0; i<nShapes; i++)
	{
		ShapeBackupInfo* pInfo = pBuf+i;
		delete[] pInfo->vb;
		pInfo->vb = NULL;
	}

	delete[] pBuf;
	pBuf = NULL;
}

void C3DEffectX::BackupShapeInfo	(ShapeBackupInfo* pBuf)
{
	if (!pBuf)
		return;

	for (int i=0; i<(int)m_dwShapes; i++)
	{
		MYASSERT (m_Shape[i]);
		void* p = 0;
		if ( SUCCEEDED ( m_Shape[i]->vb->Lock ( 0, 0, &p, D3DLOCK_READONLY ) ) )
		{
			memcpy(pBuf->vb, p, sizeof(ShapeOutVertex)*2*m_Shape[i]->dwSegmentMax);
			m_Shape[i]->vb->Unlock ();
		}

		pBuf->dwSegmentCur	= m_Shape[i]->dwSegmentCur;
		pBuf->last[0]		= m_Shape[i]->last[0];
		pBuf->last[1]		= m_Shape[i]->last[1];

		pBuf->bFrist		= m_Shape[i]->bFrist;

		pBuf ++;
	}
}

void C3DEffectX::RestoreShapeInfo (ShapeBackupInfo* pBuf)
{
	if (!pBuf)
		return;

	for (int i=0; i<(int)m_dwShapes; i++)
	{
		MYASSERT (m_Shape[i]);
		void* p = 0;
		if ( SUCCEEDED ( m_Shape[i]->vb->Lock ( 0, 0, &p, D3DLOCK_NOSYSLOCK ) ) )
		{
			memcpy(p, pBuf->vb, sizeof(ShapeOutVertex)*2*m_Shape[i]->dwSegmentMax);
			m_Shape[i]->vb->Unlock ();
		}

		m_Shape[i]->dwSegmentCur	= pBuf->dwSegmentCur;
		m_Shape[i]->last[0]			= pBuf->last[0];
		m_Shape[i]->last[1]			= pBuf->last[1];

		m_Shape[i]->bFrist			= pBuf->bFrist;

		pBuf ++;
	}
}

void C3DEffectX::SetPtclCurFrame ( DWORD dwIndex, DWORD dwFrame )
{
	Ptcl_SetFrame ( m_Ptcl[dwIndex], dwFrame );
}
DWORD C3DEffectX::GetPtclFrames ( DWORD dwIndex )
{
	return m_Ptcl[dwIndex]->dwFrames;
}
void C3DEffectX::SetPhyCurFrame ( DWORD dwIndex, DWORD dwFrame )
{
	Phy_SetFrame ( m_Phy[dwIndex], dwFrame );
}
DWORD C3DEffectX::GetPhyFrames ( DWORD dwIndex )
{
	return m_Phy[dwIndex]->lpMotion->dwFrames;
}
DWORD C3DEffectX::GetFrames()
{
	DWORD dwFrames = 0;
	DWORD n;
	for ( n = 0; n < m_dwPtcls; n++ )
		dwFrames = __max ( dwFrames, m_Ptcl[n]->dwFrames );
	for ( n = 0; n < m_dwPtclExs; n++ )
		dwFrames = __max ( dwFrames, m_PtclEx[n]->dwFrames );
	for ( n = 0; n < m_dwPtcl3s; n++ )
		dwFrames = __max ( dwFrames, m_Ptcl3[n]->dwFrames );
	for ( n = 0; n < m_dwPhys; n++ )
		dwFrames = __max ( dwFrames, m_Phy[n]->lpMotion ? m_Phy[n]->lpMotion->dwFrames : 0 );
	for ( n = 0; n < m_dwShapes; n++ )
		dwFrames = __max ( dwFrames, m_Shape[n]->lpMotion ? m_Shape[n]->lpMotion->dwFrames : 0 );
	return dwFrames;
}
void C3DEffectX::SetFrames(DWORD dwFrame)
{
	if(m_dwPtcls > 0)
	{
		for(DWORD i = 0; i < m_dwPtcls; i++)
			Ptcl_SetFrame ( m_Ptcl[i], dwFrame );
	}
	if(m_dwPtclExs > 0)
	{
		for(DWORD i = 0; i < m_dwPtclExs; i++)
			Ptcl_SetFrame ( m_PtclEx[i], dwFrame );
	}
	if(m_dwPtcl3s > 0)
	{
		for(DWORD i = 0; i < m_dwPtcl3s; i++)
			Ptcl_SetFrame ( ( C3PtclEx* )m_Ptcl3[i], dwFrame );
	}
	if(m_dwPhys > 0)
	{
		for(DWORD i = 0; i < m_dwPhys; i++)
			Phy_SetFrame ( m_Phy[i], dwFrame );
	}
	if(m_dwShapes > 0)
	{
		for(DWORD i = 0; i < m_dwShapes; i++)
			Shape_SetFrame (m_Shape[i], dwFrame);
	}
}
PTCL_INFO* C3DEffectX::CreatePtclInfo (void)
{
	if (m_dwPtclExs <= 0)
		return NULL;
	return Ptcl_CreateInfo ( m_dwPtclExs );
}
void C3DEffectX::SetPtclInfo (PTCL_INFO* pInfo)
{
	if (!pInfo)
		return;
	for ( DWORD n = 0; n < m_dwPtclExs; n++ )
		m_PtclEx[n]->lpInfo = pInfo + n;
}
void DestroyPtclInfo ( PTCL_INFO*& pInfo )
{
	if (pInfo)
	{
		Ptcl_DestroyInfo ( pInfo );
		pInfo = NULL;
	}
}
PTCL3_INFO* C3DEffectX::CreatePtcl3Info (void)
{
	if (m_dwPtcl3s <= 0)
		return NULL;
	return Ptcl3_CreateInfo ( m_dwPtcl3s );
}
void C3DEffectX::SetPtcl3Info (PTCL3_INFO* pInfo)
{
	if (!pInfo)
		return;
	for ( DWORD n = 0; n < m_dwPtcl3s; n++ )
		m_Ptcl3[n]->lpInfo = pInfo + n;
}
void C3DEffectX::SetPtcl3Gravity (float x, float y, float z)
{
	for ( DWORD n = 0; n < m_dwPtcl3s; n++ )
		Ptcl3_SetGravity ( m_Ptcl3[n], D3DXVECTOR3 ( x, y, z ) );
}
void C3DEffectX::SetPtcl3ExternalSpeed (float x, float y, float z)
{
	for ( DWORD n = 0; n < m_dwPtcl3s; n++ )
		Ptrl3_SetExternalSpeed ( m_Ptcl3[n], D3DXVECTOR3 ( x, y, z ) );
}
void C3DEffectX::SetStepCount (DWORD dwStep)
{
	m_dwStepCount = dwStep;
}
void DestroyPtcl3Info ( PTCL3_INFO*& pInfo )
{
	if (pInfo)
	{
		Ptcl3_DestroyInfo ( pInfo );
		pInfo = NULL;
	}
}
I3DEffect* EffectCreate ( char* pszFileName )
{
	C3DEffectX* pEffect = new C3DEffectX;
	if (!pEffect)
		return NULL;
	if (!pEffect->Load(pszFileName))
	{
		SAFE_DELETE(pEffect);
		return NULL;
	}
	return pEffect;
}
