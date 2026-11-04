#include	<windows.h>
#include	"3DAniObj.h"
#include	"c3_common.h"
#include	"BaseFunc.h"
#include	"MyHeap.h"
#include	<vector>

class C3DAniObjX : public I3DAniObj
{
public:
	C3DAniObjX();
	virtual ~C3DAniObjX();
	static void* operator new(size_t uSize) throw()	{ return s_heap.Alloc(uSize); }
	static void	 operator delete(void* p)	{ s_heap.Free(p); }
	static CMyHeap	s_heap;

	virtual DWORD	Release	( void );
	virtual BOOL	Create	( const char* pszFileName );
	virtual void	SetFrame	( DWORD dwFrame );
	virtual void	Draw	( void );
	virtual void	ChangeTexture ( I3DTexture* pTexture, char *objname );
	virtual void	ChangeMixTexture ( I3DTexture* pTexture, char *objname );
	virtual void	SetDrawType ( int nType )	{ if ( nType ) m_nDrawType = nType; }
	virtual void	SetBlend	( DWORD dwAsb, DWORD dwAdb )	{ m_dwAsb = dwAsb; m_dwAdb = dwAdb; }
	virtual void	SetColor	( DWORD dwColor )	{ m_dwColor = dwColor; }
	virtual void	SetMaterial ( D3DMATERIAL8 *lpMaterial )	{ CopyMemory ( &m_material, lpMaterial, sizeof ( D3DMATERIAL8 ) ); }
	virtual bool	HitTest	( int nScrX, int nScrY, float* pfX, float* pfY, float* pfZ );
public:
	std::vector<C3Phy*>		m_setPhy;
	std::vector<C3Motion*>	m_setMotion;
	DWORD					m_dwColor;
	DWORD					m_dwAsb;
	DWORD					m_dwAdb;
	int						m_nDrawType;
	D3DMATERIAL8			m_material;
};

CMyHeap C3DAniObjX::s_heap("C3DAniObjX");

C3DAniObjX::C3DAniObjX ()
{
	m_nDrawType	= 4;
	m_dwAsb		= 5;
	m_dwAdb		= 6;
	m_dwColor	= 0xffffffff;
	ZeroMemory ( &m_material, sizeof ( m_material ) );
	m_material.Diffuse.a = m_material.Diffuse.r = m_material.Diffuse.g = m_material.Diffuse.b = 1.0f;
	m_material.Ambient.a = m_material.Ambient.r = m_material.Ambient.g = m_material.Ambient.b = 0.5f;
}

C3DAniObjX::~C3DAniObjX ()
{
	for ( DWORD i = 0; i < m_setPhy.size (); i++ )
		Phy_Unload ( &m_setPhy[i] );
	m_setPhy.clear ();
}
DWORD C3DAniObjX::Release ( void )
{
	delete this;
	return 0;
}
BOOL C3DAniObjX::Create ( const char* pszFileName )
{
	if ( !pszFileName )
		return false;
	DWORD dwSize = 0;
	void *f = Common_OpenRes ( pszFileName, dwSize, 1 );
	if ( f )
	{
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
				C3Phy* phy = NULL;
				BOOL bLoad = false;
				if ( chunk.byChunkID[3] == ' ' )
					bLoad = Phy_Load ( &phy, f, false, true, false, true );
				else if ( chunk.byChunkID[3] == '2' )
					bLoad = Phy_Load ( &phy, f, true, false, false, true );
				else if ( chunk.byChunkID[3] == '3' )
					bLoad = Phy_Load ( &phy, f, true, false, false, false );
				else
					bLoad = Phy_Load ( &phy, f, false, true, false, false );
				if ( !bLoad )
					break;
				m_setPhy.push_back ( phy );
			}
			else if ( chunk.byChunkID[0] == 'M' &&
					  chunk.byChunkID[1] == 'O' &&
					  chunk.byChunkID[2] == 'T' &&
					  chunk.byChunkID[3] == 'I' )
			{
				C3Motion* motion = NULL;
				if ( !Motion_Load ( &motion, f ) )
					break;
				m_setMotion.push_back ( motion );
			}
			else
			{
				Common_SeekRes ( f, chunk.dwChunkSize, SEEK_CUR );
			}
		}
		Common_CloseRes ( f );
	}

	if ( m_setPhy.size () == 0 || m_setPhy.size () != m_setMotion.size () )
		return false;
	for ( DWORD i = 0; i < m_setPhy.size (); i++ )
		m_setPhy[i]->lpMotion = m_setMotion[i];
	return true;
}
void C3DAniObjX::SetFrame ( DWORD dwFrame )
{
	for ( DWORD i = 0; i < m_setPhy.size (); i++ )
		Phy_SetFrame ( m_setPhy[i], dwFrame );
}
void C3DAniObjX::Draw ( void )
{
	CMyBitmap::PrepareModle ();

	D3DMATERIAL8 material;
	CopyMemory ( &material, &m_material, sizeof ( D3DMATERIAL8 ) );
	m_material.Diffuse.a = ( float )( m_dwColor >> 24 ) * m_material.Diffuse.a * ( 1.0f / 255 );
	m_material.Diffuse.r = ( float )( ( m_dwColor >> 16 ) & 0xff ) * m_material.Diffuse.r * ( 1.0f / 255 );
	m_material.Diffuse.g = ( float )( ( m_dwColor >> 8 ) & 0xff ) * m_material.Diffuse.g * ( 1.0f / 255 );
	m_material.Diffuse.b = ( float )( m_dwColor & 0xff ) * m_material.Diffuse.b * ( 1.0f / 255 );

	for ( DWORD i = 0; i < m_setPhy.size (); i++ )
	{
		D3DXVECTOR3 vLight;
		vLight.x = vLight.y = vLight.z = 0;
		Phy_Calculate ( m_setPhy[i], true, 1, vLight );
		Phy_DrawNormal ( m_setPhy[i], material, NULL, true, m_dwAsb, m_dwAdb, 4, 0.35f );
	}
}
void C3DAniObjX::ChangeTexture ( I3DTexture* pTexture, char *objname )
{
	if ( !pTexture )
		return;
	for ( DWORD i = 0; i < m_setPhy.size (); i++ )
	{
		if ( objname && strcmp ( objname, m_setPhy[i]->lpName ) != 0 )
			continue;
		Phy_ChangeTexture ( m_setPhy[i], pTexture->GetTextureID () );
	}
}
void C3DAniObjX::ChangeMixTexture ( I3DTexture* pTexture, char *objname )
{
	if ( !pTexture )
		return;
	for ( DWORD i = 0; i < m_setPhy.size (); i++ )
	{
		if ( objname && strcmp ( objname, m_setPhy[i]->lpName ) != 0 )
			continue;
		Phy_ChangeMixTexture ( m_setPhy[i], pTexture->GetTextureID () );
	}
}
bool C3DAniObjX::HitTest ( int nScrX, int nScrY, float* pfX, float* pfY, float* pfZ )
{
	D3DXVECTOR3 vFrom, vDir;
	CMyBitmap::ScrRayBuild ( nScrX, nScrY, vFrom, vDir );

	bool bFirst = true;
	D3DXVECTOR3 vBest;
	vBest.x = vBest.y = vBest.z = 0;
	for ( DWORD i = 0; i < m_setPhy.size (); i++ )
	{
		C3Phy* phy = m_setPhy[i];
		if ( !phy || !m_setMotion[i] )
			continue;

		D3DXMATRIXA16 matrix, matInverse;
		Motion_GetMatrix ( m_setMotion[i], 0, &matrix );
		D3DXMatrixInverse ( &matInverse, NULL, &matrix );
		D3DXVECTOR3 vLocalFrom, vLocalDir;
		D3DXVec3TransformCoord ( &vLocalFrom, &vFrom, &matInverse );
		D3DXVec3TransformNormal ( &vLocalDir, &vDir, &matInverse );
		if ( !D3DXBoxBoundProbe ( &phy->bboxMin, &phy->bboxMax, &vLocalFrom, &vLocalDir ) )
			continue;

		DWORD dwTriCount = phy->dwATriCount + phy->dwNTriCount;
		for ( DWORD t = 0; t < dwTriCount; t++ )
		{
			D3DXVECTOR3 v[3];
			for ( int k = 0; k < 3; k++ )
			{
				v[k] = phy->lpVB[phy->lpIB[t * 3 + k]].pos;
				D3DXVec3TransformCoord ( &v[k], &v[k], &matrix );
			}
			D3DXVECTOR3 vResult;
			if ( IntersectTriangle ( vFrom, vDir, v[0], v[1], v[2], vResult ) )
			{
				if ( bFirst )
				{
					bFirst = false;
					vBest = vResult;
				}
				else if ( vResult.z < vBest.z )
					vBest = vResult;
			}
		}
	}
	if ( bFirst )
		return false;
	*pfX = vBest.x;
	*pfY = vBest.y;
	*pfZ = vBest.z;
	return true;
}
I3DAniObj* Scene3DCreate ( const char* pszFileName )
{
	if (!pszFileName)
		return NULL;
	C3DAniObjX* pObj = new C3DAniObjX;
	if (!pObj)
		return NULL;
	if (!pObj->Create(pszFileName))
	{
		SAFE_DELETE(pObj);
		return NULL;
	}
	return pObj;
}
