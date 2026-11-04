#include "seifuku_jpegenc.h"
#include <wincodec.h>
#include <objbase.h>

namespace
{
	struct JpegComScope
	{
		HRESULT hr;
		JpegComScope () { hr = CoInitializeEx ( NULL, COINIT_APARTMENTTHREADED ); }
		~JpegComScope () { if ( SUCCEEDED ( hr ) ) CoUninitialize (); }
	};

	template <class T> struct JpegComPtr
	{
		T *p;
		JpegComPtr () : p ( 0 ) {}
		~JpegComPtr () { if ( p ) p->Release (); }
		T** operator& () { return &p; }
		T* operator-> () { return p; }
	};
}

BOOL Seifuku_JpegEncode24 ( const char *lpJpegName, DWORD dwWidth, DWORD dwHeight, const unsigned char *lpRows, BOOL bBgr, DWORD dwQuality )
{
	if ( !lpJpegName || !lpRows || dwWidth == 0 || dwHeight == 0 )
		return FALSE;

	JpegComScope com;
	JpegComPtr<IWICImagingFactory> factory;
	if ( FAILED ( CoCreateInstance ( CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, ( void** )&factory ) ) )
		return FALSE;

	JpegComPtr<IStream> stream;
	if ( FAILED ( CreateStreamOnHGlobal ( NULL, TRUE, &stream ) ) )
		return FALSE;

	JpegComPtr<IWICBitmapEncoder> encoder;
	if ( FAILED ( factory->CreateEncoder ( GUID_ContainerFormatJpeg, NULL, &encoder ) ) ||
		 FAILED ( encoder->Initialize ( stream.p, WICBitmapEncoderNoCache ) ) )
		return FALSE;

	JpegComPtr<IWICBitmapFrameEncode> frame;
	JpegComPtr<IPropertyBag2> props;
	if ( FAILED ( encoder->CreateNewFrame ( &frame, &props ) ) )
		return FALSE;

	PROPBAG2 names[2];
	VARIANT values[2];
	ZeroMemory ( names, sizeof ( names ) );
	ZeroMemory ( values, sizeof ( values ) );
	names[0].pstrName = L"ImageQuality";
	values[0].vt = VT_R4;
	values[0].fltVal = ( float )( dwQuality > 100 ? 100 : dwQuality ) / 100.0f;
	names[1].pstrName = L"JpegYCrCbSubsampling";
	values[1].vt = VT_UI1;
	values[1].bVal = WICJpegYCrCbSubsampling420;
	if ( FAILED ( props->Write ( 2, names, values ) ) || FAILED ( frame->Initialize ( props.p ) ) )
		return FALSE;

	const UINT stride = dwWidth * 3;
	const UINT bytes = stride * dwHeight;
	const unsigned char *pixels = lpRows;
	unsigned char *swapped = 0;
	if ( !bBgr )
	{
		swapped = new unsigned char[bytes];
		for ( UINT i = 0; i < bytes; i += 3 )
		{
			swapped[i] = lpRows[i + 2];
			swapped[i + 1] = lpRows[i + 1];
			swapped[i + 2] = lpRows[i];
		}
		pixels = swapped;
	}

	WICPixelFormatGUID format = GUID_WICPixelFormat24bppBGR;
	BOOL bOk = SUCCEEDED ( frame->SetSize ( dwWidth, dwHeight ) ) &&
			   SUCCEEDED ( frame->SetPixelFormat ( &format ) ) &&
			   IsEqualGUID ( format, GUID_WICPixelFormat24bppBGR ) &&
			   SUCCEEDED ( frame->WritePixels ( dwHeight, stride, bytes, ( BYTE* )pixels ) ) &&
			   SUCCEEDED ( frame->Commit () ) &&
			   SUCCEEDED ( encoder->Commit () );
	delete [] swapped;
	if ( !bOk )
		return FALSE;

	STATSTG st;
	HGLOBAL mem = NULL;
	if ( FAILED ( stream->Stat ( &st, STATFLAG_NONAME ) ) || FAILED ( GetHGlobalFromStream ( stream.p, &mem ) ) )
		return FALSE;
	const DWORD size = ( DWORD )st.cbSize.QuadPart;
	void *data = GlobalLock ( mem );
	if ( !data )
		return FALSE;
	HANDLE file = CreateFileA ( lpJpegName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( file == INVALID_HANDLE_VALUE )
	{
		GlobalUnlock ( mem );
		return FALSE;
	}
	DWORD written = 0;
	bOk = WriteFile ( file, data, size, &written, NULL ) && written == size;
	CloseHandle ( file );
	GlobalUnlock ( mem );
	return bOk;
}
