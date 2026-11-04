#include "c3_line.h"
#include "c3_main.h"

C3_CORE_DLL_API
void Line_SetColor ( C3Line *lpLine,
					 float alpha,
					 float red,
					 float green,
					 float blue )
{
	if ( !lpLine )
		return;

	lpLine->fA = alpha;
	lpLine->fR = red;
	lpLine->fG = green;
	lpLine->fB = blue;

	for ( DWORD n = 0; n < lpLine->dwCount; n++ )
	{
		int color = ( int )( alpha * -255.0f );
		color = ( color << 8 ) + ( int )( red * -255.0f );
		color = ( color << 8 ) + ( int )( green * -255.0f );
		color = color << 8;
		lpLine->vertex[n].color = ( int )( blue * 255.0f ) - color;
	}
}

C3_CORE_DLL_API
void Line_ClearMatrix ( C3Line *lpLine )
{
	if ( !lpLine )
		return;

	D3DXMatrixIdentity ( &lpLine->matrix );
}

C3_CORE_DLL_API
void Line_Clear ( C3Line *lpLine )
{
	if ( !lpLine )
		return;

	lpLine->dwCount = 0;
	lpLine->fA = lpLine->fR = lpLine->fG = lpLine->fB = 1.0f;
	D3DXMatrixIdentity ( &lpLine->matrix );
}

C3_CORE_DLL_API
void Line_Prepare ( void )
{
	SetRenderState ( D3DRS_ZENABLE, true );
	SetRenderState ( D3DRS_ZWRITEENABLE, true );
	SetRenderState ( D3DRS_SHADEMODE, D3DSHADE_FLAT );
	SetRenderState ( D3DRS_FILLMODE, D3DFILL_SOLID );
	SetRenderState ( D3DRS_CULLMODE, D3DCULL_NONE );
	SetRenderState ( D3DRS_DITHERENABLE, false );
	SetRenderState ( D3DRS_LIGHTING, false );
	SetRenderState ( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR2 );
	SetRenderState ( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 );
	SetRenderState ( D3DRS_ALPHABLENDENABLE, true );
	SetRenderState ( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	SetRenderState ( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
}

C3_CORE_DLL_API
void Line_Draw ( C3Line *lpLine )
{
	if ( !lpLine )
		return;

	g_D3DDevice->SetTransform ( D3DTS_WORLD, &lpLine->matrix );

	g_D3DDevice->SetTexture ( 0, 0 );
	g_D3DDevice->SetTexture ( 1, 0 );
	g_D3DDevice->SetTexture ( 2, 0 );
	g_D3DDevice->SetTexture ( 3, 0 );

	g_D3DDevice->SetFVF ( LINE_VERTEX );

	g_D3DDevice->DrawPrimitiveUP ( D3DPT_LINELIST,
								   lpLine->dwCount / 2,
								   lpLine->vertex,
								   sizeof ( LineVertex ) );
}

C3_CORE_DLL_API
void Line_Muliply ( C3Line *lpLine, D3DXMATRIX *matrix )
{
	if ( !lpLine )
		return;

	D3DXMatrixMultiply ( &lpLine->matrix,
						 &lpLine->matrix,
						 matrix );
}
