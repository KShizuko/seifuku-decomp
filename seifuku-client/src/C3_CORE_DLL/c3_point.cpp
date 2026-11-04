#include "c3_point.h"
#include "c3_main.h"

C3_CORE_DLL_API
void Point_Prepare ( void )
{
	SetRenderState ( D3DRS_ZENABLE, true );
	SetRenderState ( D3DRS_ZWRITEENABLE, true );
	SetRenderState ( D3DRS_SHADEMODE, D3DSHADE_FLAT );
	SetRenderState ( D3DRS_DITHERENABLE, true );
	SetRenderState ( D3DRS_CULLMODE, D3DCULL_NONE );
	SetRenderState ( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR2 );
	SetRenderState ( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 );
	SetRenderState ( D3DRS_ALPHABLENDENABLE, true );
	SetRenderState ( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	SetRenderState ( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	SetRenderState ( D3DRS_LIGHTING, false );
}

C3_CORE_DLL_API
void Point_Draw ( C3Point *lpPoint )
{
	if ( !lpPoint )
		return;

	g_D3DDevice->SetTransform ( D3DTS_WORLD, &lpPoint->matrix );

	g_D3DDevice->SetTexture ( 0, 0 );
	g_D3DDevice->SetTexture ( 1, 0 );
	g_D3DDevice->SetTexture ( 2, 0 );
	g_D3DDevice->SetTexture ( 3, 0 );

	g_D3DDevice->SetFVF ( POINT_VERTEX );

	g_D3DDevice->DrawPrimitiveUP ( D3DPT_POINTLIST,
								   lpPoint->dwCount,
								   lpPoint->vertex,
								   sizeof ( PointVertex ) );
}

C3_CORE_DLL_API
void Point_SetColor ( C3Point *lpPoint,
					  float alpha,
					  float red,
					  float green,
					  float blue )
{
	if ( !lpPoint )
		return;

	lpPoint->fA = alpha;
	lpPoint->fR = red;
	lpPoint->fG = green;
	lpPoint->fB = blue;

	for ( DWORD n = 0; n < lpPoint->dwCount; n++ )
	{
		int color = ( int )( alpha * -255.0f );
		color = ( color << 8 ) + ( int )( red * -255.0f );
		color = ( color << 8 ) + ( int )( green * -255.0f );
		color = color << 8;
		lpPoint->vertex[n].color = ( int )( blue * 255.0f ) - color;
	}
}

C3_CORE_DLL_API
void Point_ClearMatrix ( C3Point *lpPoint )
{
	if ( !lpPoint )
		return;

	D3DXMatrixIdentity ( &lpPoint->matrix );
}

C3_CORE_DLL_API
void Point_Clear ( C3Point *lpPoint )
{
	if ( !lpPoint )
		return;

	lpPoint->dwCount = 0;
	lpPoint->fA = lpPoint->fR = lpPoint->fG = lpPoint->fB = 1.0f;
	D3DXMatrixIdentity ( &lpPoint->matrix );
}

C3_CORE_DLL_API
void Point_Muliply ( C3Point *lpPoint, D3DXMATRIX *matrix )
{
	if ( !lpPoint )
		return;

	D3DXMatrixMultiply ( &lpPoint->matrix,
						 &lpPoint->matrix,
						 matrix );
}
