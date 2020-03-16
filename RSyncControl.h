﻿#pragma once

// RSyncControl.h: RSyncControl.DLL 的主标头文件

#if !defined( __AFXCTL_H__ )
#error "在包括此文件之前包括“afxctl.h”"
#endif

#include "resource.h"       // 主符号


// CRSyncControlApp: 请参阅 RSyncControl.cpp 了解实现。

class CRSyncControlApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
};

extern const GUID CDECL _tlid;
extern const GUID CDECL _RSyncControlCtrl;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;
extern void RegisterControlsSafely();
extern void UnRegisterControlsSafely();

