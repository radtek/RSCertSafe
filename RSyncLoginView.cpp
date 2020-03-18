// RSyncLoginView.cpp: 实现文件
//

#include "stdafx.h"
#include "RSyncControl.h"
#include "RSyncLoginView.h"
#include "afxdialogex.h"
#include <comutil.h>
#include <cassert>

// RSyncLoginView 对话框

IMPLEMENT_DYNAMIC(RSyncLoginView, CDialog)

RSyncLoginView::RSyncLoginView(CString& cid, CString& cpassword)
	: CDialog(IDD_DIALOG1, NULL)
	, m_nameStr(cid)
	, m_passwordStr(cpassword)
{
	
}

RSyncLoginView::~RSyncLoginView()
{
}

BOOL RSyncLoginView::OnInitDialog()
{
	CDialog::OnInitDialog();
	CRect r;
	GetWindowRect(r);

	MoveWindow(CRect(r.left, r.top, r.left + 350, r.top + 200));
	return 0;
}

void RSyncLoginView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_passwordStr);
}


BEGIN_MESSAGE_MAP(RSyncLoginView, CDialog)
	ON_BN_CLICKED(IDOK, &RSyncLoginView::OnBnClickedOk)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// RSyncLoginView 消息处理程序


void RSyncLoginView::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	
	CDialog::OnOK();
}

void RSyncLoginView::SetInputName(CString name)
{
	m_pName = name;
}

CString RSyncLoginView::GetInputName()
{
	return m_nameStr;
}

CString RSyncLoginView::GetInputPassword()
{
	return m_passwordStr;
}

std::string toString(CString& str)
{
	DWORD dBufSize = WideCharToMultiByte(CP_OEMCP, 0, str, -1, NULL, 0, NULL, FALSE);
	char *dBuf = new char[dBufSize];
	memset(dBuf, 0, dBufSize);
	int nRet = WideCharToMultiByte(CP_OEMCP, 0, str, -1, dBuf, dBufSize, NULL, FALSE);
	assert(nRet > 0);
	return std::string(dBuf);
}

std::string RSyncLoginView::name()
{
	return toString(m_nameStr);
}

std::string RSyncLoginView::password()
{
	return toString(m_passwordStr);
}


void RSyncLoginView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
	CBitmap bitmap;
	bitmap.LoadBitmap(IDB_BITMAP1);
	CStatic *p = (CStatic *)GetDlgItem(IDC_STATIC_PIC);
	p->ModifyStyle(0xf, SS_BITMAP | SS_CENTERIMAGE);
	p->SetBitmap(bitmap);


	BITMAP bmpInfo;
	bitmap.GetBitmap(&bmpInfo);
	CDC dcMemory;
	CDC* pDC = GetDlgItem(IDC_STATIC_PIC)->GetDC();
	dcMemory.CreateCompatibleDC(pDC);
	CBitmap* pOldBitmap = dcMemory.SelectObject(&bitmap);
	CRect rect;
	p->GetClientRect(&rect);
	int nX = rect.left + (rect.Width() - bmpInfo.bmWidth) / 2;
	int nY = rect.top + (rect.Height() - bmpInfo.bmHeight) / 2;
	//pDC->BitBlt(0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, &dcMemory, 0, 0, SRCCOPY);
	pDC->SetStretchBltMode(COLORONCOLOR);
	pDC->StretchBlt(0, 0, rect.Width(), rect.Height(), &dcMemory, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, SRCCOPY);
	dcMemory.SelectObject(pOldBitmap);
	ReleaseDC(pDC);
					   // 不为绘图消息调用 CDialog::OnPaint()
}
