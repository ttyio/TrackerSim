
// TrackerApp.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTrackerAppApp: 
// �йش����ʵ�֣������ TrackerApp.cpp
//

class CTrackerAppApp : public CWinApp
{
public:
	CTrackerAppApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTrackerAppApp theApp;