
// SCB.h : SCB ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.


// CSCBApp:
// �� Ŭ������ ������ ���ؼ��� SCB.cpp�� �����Ͻʽÿ�.
//

class CSCBApp : public CWinApp
{
public:
	CSCBApp();


// �������Դϴ�.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// �����Դϴ�.
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CSCBApp theApp;
