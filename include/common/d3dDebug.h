#ifndef D3DDEBUG_H
#define D3DDEBUG_H

#include <dxerr.h>

//---------------------------------------------------------------------------------------
// Simple d3d error checker for book demos.
//---------------------------------------------------------------------------------------

#ifndef DebugHR()
#define DebugHR(hr)                                                  \
	{                                                                \
    HRESULT result = hr;                                             \
	if(FAILED(result))                                               \
	{                                                                \
		DXTrace(__FILE__, (DWORD)__LINE__, result, L"Error", true);  \
	}                                                                \
	}
#endif

//---------------------------------------------------------------------------------------
// Convenience macro for releasing COM objects.
//---------------------------------------------------------------------------------------

#define ReleaseCOM(x) { if(x){ x->Release(); x = 0; } }

//---------------------------------------------------------------------------------------
// Convenience macro for deleting objects.
//---------------------------------------------------------------------------------------

#define SafeDelete(x) { delete x; x = 0; }


#endif

