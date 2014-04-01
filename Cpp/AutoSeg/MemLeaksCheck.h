#pragma once

#ifdef _DEBUG
#define DEBUG_CLIENTBLOCK    new( _CLIENT_BLOCK, __FILE__, __LINE__)
#else
#define DEBUG_CLIENTBLOCK
#endif

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <cassert>

#if _MSC_VER > 1000
#include <crtdbg.h>
#endif

#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif

#if _MSC_VER > 1000
inline void Exit()
{
    int i = _CrtDumpMemoryLeaks();
    assert( i == 0);
}

inline void CheckMemLeaks(){
    atexit(Exit);
    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
}
#else
inline void Exit()
{
}

inline void CheckMemLeaks(){
}
#endif

