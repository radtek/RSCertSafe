﻿// RSyncControlCtrl.cpp : CRSyncControlCtrl ActiveX 控件类的实现。

#include "stdafx.h"
#include "afxdialogex.h"
#include "RSyncControl.h"
#include "RSyncControlCtrl.h"
#include "RSyncControlPropPage.h"
#include "RSyncLoginView.h"
#include "RSyncMsgTip.h"
#include "RSyncChangPassWd.h"
#include "Utility.h"
#include "Poco/String.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Dynamic/Var.h"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/Object.h"
#include "Poco/DynamicStruct.h"
#include "Poco/Observer.h"
#include "Poco/String.h"
#include "Poco/Debugger.h"
#include <cassert>
#include <sstream>
#include <atlconv.h>
#include <comutil.h>

using namespace Poco;
using Poco::Dynamic::Var;
using Poco::Net::HTMLForm;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPClientSession;
using Poco::JSON::Parser;
using Poco::JSON::Object;
using Poco::DynamicStruct;
using Poco::Debugger;
using Poco::JSONOptions;

using Reach::ActiveX::Utility;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define UM_EVENT (WM_USER + 666)

IMPLEMENT_DYNCREATE(CRSyncControlCtrl, COleControl)

// 消息映射

BEGIN_MESSAGE_MAP(CRSyncControlCtrl, COleControl)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()

// 调度映射

BEGIN_DISPATCH_MAP(CRSyncControlCtrl, COleControl)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_KeyGetKeySn",						dispid_20,			RS_KeyGetKeySn,				VT_BSTR, VTS_NONE)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_KeyGetKeySnExt",					dispid_21,			RS_KeyGetKeySnExt,			VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_GetUserList",						dispid_22,			RS_GetUserList,				VT_BSTR, VTS_NONE)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_GetCertBase64String",				dispid_23,			RS_GetCertBase64String,		VT_BSTR, VTS_BSTR VTS_I2)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_CertLogin",							dispid_24,			RS_CertLogin,				VT_BSTR, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_GetPinRetryCount",					dispid_25,			RS_GetPinRetryCount,		VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_ChangePassWd",						dispid_26,			RS_ChangePassWd,			VT_BSTR, VTS_BSTR VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_KeySignByP1",						dispid_27,			RS_KeySignByP1,				VT_BSTR, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_VerifySignByP1",					dispid_28,			RS_VerifySignByP1,			VT_BSTR, VTS_BSTR VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_KeyEncryptData",					dispid_29,			RS_KeyEncryptData,			VT_BSTR, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_KeyDecryptData",					dispid_30,			RS_KeyDecryptData,			VT_BSTR, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_GetCertInfo",						dispid_31,			RS_GetCertInfo,				VT_BSTR, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_KeySignByP7",						dispid_32,			RS_KeySignByP7,				VT_BSTR, VTS_BSTR VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_VerifySignByP7",					dispid_33,			RS_VerifySignByP7,			VT_BSTR, VTS_BSTR VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_CloudLoginAuth",					dispid_34,			RS_CloudLoginAuth,			VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_CloudSealAuth",						dispid_35,			RS_CloudSealAuth,					VT_BSTR,VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_CloudGetAuth",						dispid_36,			RS_CloudGetAuth,					VT_BSTR,VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_CloudGetSealList",					dispid_37,			RS_CloudGetSealList,				VT_BSTR,VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_CloudSignByP7",						dispid_38,			RS_CloudSignByP7,					VT_BSTR,VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_CloudGetSignResult",				dispid_39,			RS_CloudGetSignResult,				VT_BSTR,VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_CloudLogoutAuth",					dispid_40,			RS_CloudLogoutAuth,					VT_BSTR,VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_CloudEncryptAuth",					dispid_41,			RS_CloudEncryptAuth,				VT_BSTR,VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_CloudEncryptData",					dispid_42,			RS_CloudEncryptData,				VT_BSTR,VTS_BSTR VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_CloudEncryptFile",					dispid_43,			RS_CloudEncryptFile,				VT_BSTR,VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_CloudDevryptAuth",					dispid_44,			RS_CloudDevryptAuth,				VT_BSTR,VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_CloudDevryptData",					dispid_45,			RS_CloudDevryptData,				VT_BSTR,VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_CloudDevryptFile",					dispid_46,			RS_CloudDevryptFile,				VT_BSTR,VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_CloudReceiveDevryptResult",			dispid_47,			RS_CloudReceiveDevryptResult,		VT_BSTR, VTS_BSTR VTS_BSTR VTS_BSTR )
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_CloudGetCompanyCert",				dispid_48,			RS_CloudGetCompanyCert,				VT_BSTR,VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_CloudGetCertAuth",					dispid_49,			RS_CloudGetCertAuth,				VT_BSTR,VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_CloudGetCertBase64",				dispid_50,			RS_CloudGetCertBase64,				VT_BSTR,VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_CloudLogout",						dispid_51,			RS_CloudLogout,						VT_BSTR,VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_KeyStatus",							dispid_52,			RS_KeyStatus,						VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_ConfigParameters",					dispid_53,			RS_ConfigParameters,				VTS_NONE, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_GetParameters",						dispid_54,			RS_GetParameters,					VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_GreateQRCode",						dispid_55,			RS_CreateQRCode,					VT_BSTR, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_GetTransid",						dispid_56,			RS_GetTransid,						VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_EncryptFile",						dispid_57,			RS_EncryptFile,						VT_BSTR, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_DevryptFile",						dispid_58,			RS_DevryptFile,						VT_BSTR, VTS_BSTR VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_VerifyIdentity",					dispid_59,			RS_VerifyIdentity,					VT_BSTR, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_KeyEncryptFile",					dispid_60,			RS_KeyEncryptFile,					VT_BSTR, VTS_BSTR VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_KeyDecryptFile",					dispid_61,			RS_KeyDecryptFile,					VT_BSTR, VTS_BSTR VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_EncryptDataBase64",					dispid_62,			RS_EncryptDataBase64,				VT_BSTR, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_EncryptFileBase64",					dispid_63,			RS_EncryptFileBase64,				VT_BSTR, VTS_BSTR VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_ConfigParamsByBussSys",				dispid_64,			RS_ConfigParamsByBussSys,			VT_BSTR, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_EncryptData",						dispid_65,			RS_EncryptData,						VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_DevryptData",						dispid_66,			RS_DevryptData,						VT_BSTR, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CRSyncControlCtrl, "RS_KeyGetDeviceInfo",					dispid_67,			RS_KeyGetDeviceInfo,				VT_BSTR, VTS_BSTR VTS_BSTR)
END_DISPATCH_MAP()

// 事件映射

BEGIN_EVENT_MAP(CRSyncControlCtrl, COleControl)
END_EVENT_MAP()

// 属性页

// TODO: 根据需要添加更多属性页。请记住增加计数!
BEGIN_PROPPAGEIDS(CRSyncControlCtrl, 1)
	PROPPAGEID(CRSyncControlPropPage::guid)
END_PROPPAGEIDS(CRSyncControlCtrl)

// 初始化类工厂和 guid

//IMPLEMENT_OLECREATE_EX(CRSyncControlCtrl, "RSyncControl.RSyncControlCtrl.1",
//	0xa0b23721,0x9350,0x4b4d,0xb5,0x81,0x65,0xad,0xd1,0xa7,0x7a,0x5e)
IMPLEMENT_OLECREATE_EX(CRSyncControlCtrl, "RS_CERTSAFE.RS_CertSafeCtrl.1",
	0xF84C8F57,0x5A05,0x4D8C,0x82,0x6D,0x29,0x58,0x9C,0x7A,0x12,0x1C)

// 键入库 ID 和版本

IMPLEMENT_OLETYPELIB(CRSyncControlCtrl, _tlid, _wVerMajor, _wVerMinor)

// 接口 ID

//onst IID IID_DRSyncControl = { 0xdb403885,0xf81d,0x41d2,{0x80,0xd7,0x40,0x36,0x50,0x97,0xd6,0x0c} };
const IID IID_DRSyncControl = { 0x7A58AA86,0xD40D,0x4611,{0x9D,0xA1,0xDF,0xCE,0xAE,0x95,0x31,0x5A} };
//const IID IID_DRSyncControlEvents = { 0xb1c93fb3,0x0485,0x4837,{0x9a,0xaf,0xaf,0x8f,0x96,0xa2,0xc0,0xd2} };
const IID IID_DRSyncControlEvents = { 0xEECD3FF1,0xDED8,0x44B3,{0x87,0x7C,0x99,0xD0,0x56,0xD5,0x44,0x9D} };

// 控件类型信息

static const DWORD _dwRSyncControlOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CRSyncControlCtrl, IDS_RSYNCCONTROL, _dwRSyncControlOleMisc)

// CRSyncControlCtrl::CRSyncControlCtrlFactory::UpdateRegistry -
// 添加或移除 CRSyncControlCtrl 的系统注册表项

BOOL CRSyncControlCtrl::CRSyncControlCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO:  验证您的控件是否符合单元模型线程处理规则。
	// 有关更多信息，请参考 MFC 技术说明 64。
	// 如果您的控件不符合单元模型规则，则
	// 必须修改如下代码，将第六个参数从
	// afxRegApartmentThreading 改为 0。

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_RSYNCCONTROL,
			IDB_RSYNCCONTROL,
			afxRegApartmentThreading,
			_dwRSyncControlOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}

CRSyncControlCtrl::CRSyncControlCtrl()
{
	OutputDebugStringA("Enter CRSyncControlCtrl::CRSyncControlCtrl\n");
	InitializeIIDs(&IID_DRSyncControl, &IID_DRSyncControlEvents);
	// TODO:  在此初始化控件的实例数据。
	m_bLoginState = FALSE;
	OutputDebugStringA("Exit CRSyncControlCtrl::CRSyncControlCtrl\n");
}

// CRSyncControlCtrl::~CRSyncControlCtrl - 析构函数

CRSyncControlCtrl::~CRSyncControlCtrl()
{
	// TODO:  在此清理控件的实例数据。
	OutputDebugStringA("CRSyncControlCtrl::~CRSyncControlCtrl\n");
}

bool IsLogined(const std::string& id)
{
	HTMLForm params;
	params.set("containerId", id);
	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequest("/RS_KeyStatus", body.str());

	Parser ps;
	Var res = ps.parse(result);
	Object::Ptr object = res.extract<Object::Ptr>();
	assert(object);
	DynamicStruct ds = *object;

	return ds["code"] == "0000";
}

enum action
{
	LoginAuth = 1,
	EncryptAuth,
	DecryptAuth,
	SignedSealAuth,
	UnSignedSealAuth,
	GetCertAuth,
	GetSignResult = 44
};

void CRSyncControlCtrl::OnSetClientSite()
{
	OutputDebugStringA("Enter CRSyncControlCtrl::OnSetClientSite()\n");
	HWND hwnd = GetSafeHwnd();
	if (NULL == hwnd)
	{
		OutputDebugStringA("case CRSyncControlCtrl::OnSetClientSite() hwnd\n");
		VERIFY(CreateControlWindow(::GetDesktopWindow(), CRect(0, 0, 0, 0), CRect(0, 0, 0, 0)));
	}

	COleControl::OnSetClientSite();
	OutputDebugStringA("Exit CRSyncControlCtrl::OnSetClientSite()\n");
}

LRESULT CRSyncControlCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM Lparam)
{
	OutputDebugStringA("CRSyncControlCtrl::WindowProc()\n");
	return COleControl::WindowProc(message, wParam, Lparam);
}

// CRSyncControlCtrl::OnDraw - 绘图函数

void CRSyncControlCtrl::OnDraw(
	CDC* pdc, const CRect& rcBounds, const CRect& /* rcInvalid */)
{
	OutputDebugStringA("Enter CRSyncControlCtrl::OnDraw()\n");

	if (!pdc)
		return;

	// TODO:  用您自己的绘图代码替换下面的代码。
	pdc->FillRect(rcBounds, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
	pdc->Ellipse(rcBounds);

	OutputDebugStringA("Exit CRSyncControlCtrl::OnDraw()\n");
}

// CRSyncControlCtrl::DoPropExchange - 持久性支持

void CRSyncControlCtrl::DoPropExchange(CPropExchange* pPX)
{
	OutputDebugStringA("Enter CRSyncControlCtrl::DoPropExchange()\n");

	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	OutputDebugStringA("Exit CRSyncControlCtrl::DoPropExchange()\n");
	// TODO: 为每个持久的自定义属性调用 PX_ 函数。
}


// CRSyncControlCtrl::OnResetState - 将控件重置为默认状态

void CRSyncControlCtrl::OnResetState()
{
	OutputDebugStringA("Enter CRSyncControlCtrl::OnResetState()\n");

	COleControl::OnResetState();  // 重置 DoPropExchange 中找到的默认值

	OutputDebugStringA("Exit CRSyncControlCtrl::OnResetState()\n");
	// TODO:  在此重置任意其他控件状态。
}


// CRSyncControlCtrl::AboutBox - 向用户显示“关于”框

void CRSyncControlCtrl::AboutBox()
{
	CDialogEx dlgAbout(IDD_ABOUTBOX_RSYNCCONTROL);
	dlgAbout.DoModal();
}

void CRSyncControlCtrl::RS_ConfigParameters(BSTR cmd, BSTR val)
{
	std::string cmdid = utility_bstr_check(_com_util::ConvertBSTRToString(cmd));
	std::string valadress = utility_bstr_check(_com_util::ConvertBSTRToString(val));

	HTMLForm params;
	params.set("cmd", Utility::GBKEncodingUTF8(cmdid));
	params.set("val", Utility::GBKEncodingUTF8(valadress));

	std::ostringstream body;
	params.write(body);
	Utility::SuperRequestGBK("/RS_ConfigParameters", body.str());
}

BSTR CRSyncControlCtrl::RS_GetParameters(BSTR cmd)
{
	std::string cmdid = utility_bstr_check(_com_util::ConvertBSTRToString(cmd));

	HTMLForm params;
	params.set("cmd", cmdid);
	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_GetParameters", body.str());

	return _bstr_t(result.data());
}

BSTR CRSyncControlCtrl::RS_ConfigParamsByBussSys(BSTR cmd, BSTR val)
{
	std::string CMD = utility_bstr_check(_com_util::ConvertBSTRToString(cmd));
	std::string VALUE = utility_bstr_check(_com_util::ConvertBSTRToString(val));
	HTMLForm params;
	params.set("cmd", CMD);
	params.set("val", VALUE);
	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_ConfigParamsByBussSys", body.str());
	return _bstr_t(result.data());
}

BSTR CRSyncControlCtrl::RS_CreateQRCode(BSTR qrcode, BSTR path)
{
	std::string qrcodestr = utility_bstr_check(_com_util::ConvertBSTRToString(qrcode));
	std::string pathstr = utility_bstr_check(_com_util::ConvertBSTRToString(path));

	HTMLForm params;
	params.set("qrcode", qrcodestr);
	params.set("path", Utility::GBKEncodingUTF8(pathstr));

	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_GreateQRCode", body.str());

	return _bstr_t(result.data());
}

BSTR CRSyncControlCtrl::RS_GetTransid(BSTR joinCode)
{
	std::string joinCodestr = utility_bstr_check(_com_util::ConvertBSTRToString(joinCode));
	utility_message_f2("Begin %s joinCode:%s", std::string(__FUNCTION__), joinCodestr);
	HTMLForm params;
	params.set("joinCode", joinCodestr);

	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_GetTransid", body.str());
	utility_message_f2("Begin %s transid:%s", std::string(__FUNCTION__), result);
	return _bstr_t(result.data());
}

BSTR CRSyncControlCtrl::RS_EncryptFile(BSTR souceFilePath, BSTR encFilePath)
{
	std::string soucestr = utility_bstr_check(_com_util::ConvertBSTRToString(souceFilePath));
	std::string encstr = utility_bstr_check(_com_util::ConvertBSTRToString(encFilePath));

	HTMLForm params;
	params.set("souceFilePath", Utility::GBKEncodingUTF8(soucestr));
	params.set("encFilePath", Utility::GBKEncodingUTF8(encstr));

	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_EncryptFile", body.str());

	return _bstr_t(result.data());
}

BSTR CRSyncControlCtrl::RS_DevryptFile(BSTR symKey, BSTR encFilePath, BSTR dncDirectoryPath)
{
	std::string symkeyid = utility_bstr_check(_com_util::ConvertBSTRToString(symKey));
	std::string encstr = utility_bstr_check(_com_util::ConvertBSTRToString(encFilePath));
	std::string dncpathstr = utility_bstr_check(_com_util::ConvertBSTRToString(dncDirectoryPath));

	HTMLForm params;
	params.set("symKey", symkeyid);
	params.set("encFilePath", Utility::GBKEncodingUTF8(encstr));
	params.set("dncDirectoryPath", Utility::GBKEncodingUTF8(dncpathstr));

	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_DecryptFile", body.str());

	return _bstr_t(result.data());
}

BSTR CRSyncControlCtrl::RS_VerifyIdentity(BSTR certBase64, BSTR authNo)
{
	std::string CERTBASE64 = utility_bstr_check(_com_util::ConvertBSTRToString(certBase64));
	std::string AUTHNO = utility_bstr_check(_com_util::ConvertBSTRToString(authNo));

	HTMLForm params;
	params.set("certBase64", CERTBASE64);
	params.set("authNo", AUTHNO);

	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_VerifyIdentity", body.str());

	return _bstr_t(result.data());
}

BSTR CRSyncControlCtrl::RS_KeyEncryptFile(BSTR souceFilePath, BSTR encFilePath, BSTR certBase64)
{
	std::string souceFile = utility_bstr_check(_com_util::ConvertBSTRToString(souceFilePath));
	std::string encFile = utility_bstr_check(_com_util::ConvertBSTRToString(encFilePath));
	std::string cert = utility_bstr_check(_com_util::ConvertBSTRToString(certBase64));

	HTMLForm params;
	params.set("souceFilePath", Utility::GBKEncodingUTF8(souceFile));
	params.set("encFilePath", Utility::GBKEncodingUTF8(encFile));
	params.set("certBase64", cert);

	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_KeyEncryptFile", body.str());

	return _bstr_t(result.data());
}
//需要登录状态下才能调用接口
BSTR CRSyncControlCtrl::RS_KeyDecryptFile(BSTR encFilePath, BSTR dncFilePath, BSTR containerId)
{
	std::string encFile = utility_bstr_check(_com_util::ConvertBSTRToString(encFilePath));
	std::string dncFile = utility_bstr_check(_com_util::ConvertBSTRToString(dncFilePath));
	std::string id = utility_bstr_check(_com_util::ConvertBSTRToString(containerId));

	if (!IsLogined(id))
	{
		RSyncLoginView loginView(CString(id.c_str()), CString(""));
		if (loginView.DoModal() != IDOK) return _bstr_t();

		std::string result = Login(loginView.name(), loginView.password());

		if (!OK(result)) return _bstr_t(result.data());
	}

	HTMLForm params;
	params.set("encFilePath", Utility::GBKEncodingUTF8(encFile));
	params.set("dncFilePath", Utility::GBKEncodingUTF8(dncFile));
	params.set("containerId", id);

	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_KeyDecryptFile", body.str());
  
	return _bstr_t(result.data());
}

BSTR CRSyncControlCtrl::RS_EncryptDataBase64(BSTR symKey, BSTR certBase64)
{
	std::string SYMKEY = utility_bstr_check(_com_util::ConvertBSTRToString(symKey));
	std::string CERTBASE64 = utility_bstr_check(_com_util::ConvertBSTRToString(certBase64));

	HTMLForm params;
	params.set("symKey", Utility::GBKEncodingUTF8(SYMKEY));
	params.set("certBase64", Utility::GBKEncodingUTF8(CERTBASE64));

	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_EncryptDataBase64", body.str());

	return _bstr_t(result.data());
}

BSTR CRSyncControlCtrl::RS_EncryptFileBase64(BSTR souceFilePath, BSTR encFilePath, BSTR certBase64)
{
	std::string SOUCEFILEPATH = utility_bstr_check(_com_util::ConvertBSTRToString(souceFilePath));
	std::string ENCFILEPATH = utility_bstr_check(_com_util::ConvertBSTRToString(encFilePath));
	std::string CERTBASE64 = utility_bstr_check(_com_util::ConvertBSTRToString(certBase64));

	HTMLForm params;
	params.set("sourceFilePath", Utility::GBKEncodingUTF8(SOUCEFILEPATH));
	params.set("encFilePath", Utility::GBKEncodingUTF8(ENCFILEPATH));
	params.set("certBase64", Utility::GBKEncodingUTF8(CERTBASE64));

	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_EncryptFileBase64", body.str());

	return _bstr_t(result.data());
}

BSTR CRSyncControlCtrl::RS_KeyGetDeviceInfo(BSTR containerId, BSTR type)
{
	std::string ID = utility_bstr_check(_com_util::ConvertBSTRToString(containerId));
	std::string TYPE = utility_bstr_check(_com_util::ConvertBSTRToString(type));

	HTMLForm params;
	params.set("containerId", Utility::GBKEncodingUTF8(ID));
	params.set("type", Utility::GBKEncodingUTF8(TYPE));

	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_KeyGetDeviceInfo", body.str());
	return _bstr_t(result.data());
}


BSTR CRSyncControlCtrl::RS_GetUserList()
{

	OutputDebugStringA("Enter RS_GetUserList\n");

	std::string result = Utility::SuperRequestGBK("/RS_GetUserList", "");

	OutputDebugStringA(format("result: %s\n Exit RS_GetUserList\n",result).c_str());
	return _bstr_t(result.data());
}

BSTR CRSyncControlCtrl::RS_GetCertBase64String(BSTR containerId, SHORT certType)
{
	std::string id = utility_bstr_check(_com_util::ConvertBSTRToString(containerId));

	HTMLForm params;
	params.set("containerId", id);
	params.set("certType", Var(certType));

	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_GetCertBase64String", body.str());

	return _bstr_t(result.data());
}

BSTR CRSyncControlCtrl::RS_CertLogin(BSTR containerId, BSTR password)
{
	/// containerId=91B0A65F-FBA9-4B7E-9627-F74475C53B9C&password=222222

	std::string id = utility_bstr_check(_com_util::ConvertBSTRToString(containerId));
	std::string word = utility_bstr_check(_com_util::ConvertBSTRToString(password));

	if (word.empty())
	{
		RSyncLoginView loginView(CString(id.data()), CString(word.data()));
		if (loginView.DoModal() == IDOK)
		{
			id = loginView.name();
			word = loginView.password();
		}
		else
			return onCancelRespone();
	}

	std::string result = Login(id, word);
	return _bstr_t(result.data());
}

std::string Login(const std::string& id, const std::string& word)
{
	HTMLForm params;
	params.set("containerId", id);
	params.set("password", word);

	std::ostringstream body;
	params.write(body);
	return Utility::SuperRequest("/RS_CertLogin", body.str());
}

BSTR onCancelRespone() 
{
	Poco::JSON::Object result;
	result.set("code", "9999");
	result.set("msg", "user cancel");
	std::ostringstream out;
	result.stringify(out);
	return _bstr_t(out.str().data());
}


bool OK(const std::string& result)
{
	Parser T;
	Var R = T.parse(result);
	assert(R.type() == typeid(Object::Ptr));
	DynamicStruct ds = *R.extract<Object::Ptr>();
	return ds["code"] == "0000";
}

BSTR CRSyncControlCtrl::RS_GetPinRetryCount(BSTR containerId)
{
	std::string id = utility_bstr_check(_com_util::ConvertBSTRToString(containerId));

	HTMLForm params;
	params.set("containerId", id);

	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_GetPinRetryCount", body.str());


	return _bstr_t(result.data());
}

BSTR CRSyncControlCtrl::RS_ChangePassWd(BSTR containerId, BSTR oldCode, BSTR newCode)
{
	std::string id = utility_bstr_check(_com_util::ConvertBSTRToString(containerId));
	std::string theold = utility_bstr_check(_com_util::ConvertBSTRToString(oldCode));
	std::string thenew = utility_bstr_check(_com_util::ConvertBSTRToString(newCode));

	if (theold.empty() || thenew.empty())
	{
		RSyncChangPassWd passwd(CString(id.data()), CString(theold.data()), CString(thenew.data()));
		INT_PTR modal = passwd.DoModal();
		if (modal != IDOK) return onCancelRespone();
		{
			id = passwd.name();
			theold = passwd.theOld();
			thenew = passwd.theNew();
		}
	}
	
	std::string result = changpasswd(id, theold, thenew);
	return _bstr_t(result.data());
}

std::string changpasswd(const std::string& id, const std::string& theold, const std::string& thenew) {
	HTMLForm params;
	params.set("containerId", id);
	params.set("oldCode", theold);
	params.set("newCode", thenew);

	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequest("/RS_ChangePassWd", body.str());

	return result;
}

BSTR CRSyncControlCtrl::RS_KeyGetKeySnExt(BSTR containerId)
{
	std::string id = utility_bstr_check(_com_util::ConvertBSTRToString(containerId));
	
	HTMLForm params;
	params.set("containerId", id);

	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_KeyGetKeySn", body.str());


	return _bstr_t(result.data());
}

BSTR CRSyncControlCtrl::RS_KeyGetKeySn()
{
	std::string result;
	result = Utility::SuperRequest("/RS_GetUserList", "");

	std::string id = Utility::formatUid(result);

	HTMLForm params;
	params.set("containerId", id);
	std::ostringstream body;
	params.write(body);
	result = Utility::SuperRequest("/RS_KeyGetKeySn", body.str());

	Parser ps;
	Var res = ps.parse(result);
	assert(res.type() == typeid(Object::Ptr));

	Object::Ptr object = res.extract<Object::Ptr>();
	assert(object);
	Object::Ptr data = object->getObject("data");
	if (data && data->has("containerId"))
		data->remove("containerId");
	object->set("data", data);
	DynamicStruct ds = *object;
	result = Utility::UTF8JSONStreamGBK(ds.toString());

	return _bstr_t(result.data());
}

BSTR CRSyncControlCtrl::RS_KeySignByP1(BSTR msg, BSTR containerId)
{
	/// containerId=806000119631708&asn1Msg=Today+is+nice+day
	std::string id = utility_bstr_check(_com_util::ConvertBSTRToString(containerId));
	std::string text = utility_bstr_check(_com_util::ConvertBSTRToString(msg));

	HTMLForm params;
	params.set("containerId", id);
	params.set("asn1Msg", Utility::GBKEncodingUTF8(text));

	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_KeySignByP1", body.str());


	return _bstr_t(result.data());
}

BSTR CRSyncControlCtrl::RS_VerifySignByP1(BSTR certBase64, BSTR msg, BSTR signdMsg)
{
	std::string cert = utility_bstr_check(_com_util::ConvertBSTRToString(certBase64));
	std::string text = utility_bstr_check(_com_util::ConvertBSTRToString(msg));
	std::string signature = utility_bstr_check(_com_util::ConvertBSTRToString(signdMsg));
	
	HTMLForm params;
	params.set("certBase64", cert);
	params.set("msg", Utility::GBKEncodingUTF8(text));
	params.set("signdMsg", signature);
	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_VerifySignByP1", body.str());

	return _bstr_t(result.data());
}

BSTR CRSyncControlCtrl::RS_KeySignByP7(BSTR msg, BSTR flag, BSTR containerId)
{
	std::string id = utility_bstr_check(_com_util::ConvertBSTRToString(containerId));
	std::string message = utility_bstr_check(_com_util::ConvertBSTRToString(msg));
	std::string mode = utility_bstr_check(_com_util::ConvertBSTRToString(flag));

	HTMLForm params;
	params.set("containerId", id);
	params.set("msg", Utility::GBKEncodingUTF8(message));
	params.set("flag", mode);
	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_KeySignByP7", body.str());

	return _bstr_t(result.data());
}

BSTR CRSyncControlCtrl::RS_VerifySignByP7(BSTR msg, BSTR signdMsg, BSTR flag)
{
	std::string message = utility_bstr_check(_com_util::ConvertBSTRToString(msg));
	std::string signedMessage = utility_bstr_check(_com_util::ConvertBSTRToString(signdMsg));
	std::string mode = utility_bstr_check(_com_util::ConvertBSTRToString(flag));

	HTMLForm params;
	params.set("msg", Utility::GBKEncodingUTF8(message));
	params.set("signdMsg", signedMessage);
	params.set("flag", mode);
	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_VerifySignByP7", body.str());

	return _bstr_t(result.data());
}

BSTR CRSyncControlCtrl::RS_KeyEncryptData(BSTR rsKey, BSTR certBase64)
{
	std::string cert = utility_bstr_check(_com_util::ConvertBSTRToString(certBase64));
	std::string rskey = utility_bstr_check(_com_util::ConvertBSTRToString(rsKey));

	HTMLForm params;
	params.set("certBase64", cert);
	params.set("rsKey", Utility::GBKEncodingUTF8(rskey));
	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_KeyEncryptData", body.str());

	return _bstr_t(result.data());
}

BSTR CRSyncControlCtrl::RS_KeyDecryptData(BSTR encRsKey, BSTR containerId)
{
	std::string id = utility_bstr_check(_com_util::ConvertBSTRToString(containerId));
	std::string rskey = utility_bstr_check(_com_util::ConvertBSTRToString(encRsKey));

	if (!IsLogined(id))
	{
		RSyncLoginView loginView(CString(id.c_str()), CString(""));
		if (loginView.DoModal() != IDOK) return onCancelRespone();

		std::string result = Login(loginView.name(), loginView.password());

		if (!OK(result)) return _bstr_t(result.data());
	}

	HTMLForm params;
	params.set("containerId", id);
	params.set("encRsKey", rskey);
	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_KeyDecryptData", body.str());

	return _bstr_t(result.data());
}

BSTR CRSyncControlCtrl::RS_GetCertInfo(BSTR certBase64, BSTR type)
{
	std::string cert = utility_bstr_check(_com_util::ConvertBSTRToString(certBase64));
	std::string ctype = utility_bstr_check(_com_util::ConvertBSTRToString(type));

	HTMLForm params;
	params.set("certBase64", cert);
	params.set("type", ctype);
	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_GetCertInfo", body.str());

	return _bstr_t(result.data());
}

BSTR CRSyncControlCtrl::RS_CloudLoginAuth(BSTR transid)
{
	std::string id = utility_bstr_check(_com_util::ConvertBSTRToString(transid));

	HTMLForm params;
	params.set("transid", id);
	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_CloudLoginAuth", body.str());

	return _bstr_t(result.data());
}

BSTR CRSyncControlCtrl::RS_CloudSealAuth(BSTR transid)
{
	std::string id = utility_bstr_check(_com_util::ConvertBSTRToString(transid));
	utility_message_f2("Begin %s transid:%s", std::string(__FUNCTION__), id);
	HTMLForm params;
	params.set("transid", id);
	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_CloudSealAuth", body.str());
	utility_message_f2("End %s transid:%s", std::string(__FUNCTION__), id);
	return _bstr_t(result.data());
}

BSTR CRSyncControlCtrl::RS_CloudGetAuth(BSTR transid)
{
	std::string TRANSID = utility_bstr_check(_com_util::ConvertBSTRToString(transid));
	utility_message_f2("Begin %s transid:%s", std::string(__FUNCTION__), TRANSID);
	HTMLForm params;
	params.set("transid", TRANSID);
	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_CloudGetAuth", body.str());
	utility_message_f2("End %s transid:%s", std::string(__FUNCTION__), TRANSID);
	return _bstr_t(result.data());
}

BSTR CRSyncControlCtrl::RS_CloudGetSealList(BSTR token)
{
	std::string TOKEN = utility_bstr_check(_com_util::ConvertBSTRToString(token));

	HTMLForm params;
	params.set("token", TOKEN);
	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_CloudGetSealList", body.str());
	return _bstr_t(result.data());
}

BSTR CRSyncControlCtrl::RS_CloudSignByP7(BSTR msg, BSTR keySn, BSTR transid, BSTR token)
{
	std::string MSG = utility_bstr_check(_com_util::ConvertBSTRToString(msg));
	std::string KEYSN = utility_bstr_check(_com_util::ConvertBSTRToString(keySn));
	std::string TRANSID = utility_bstr_check(_com_util::ConvertBSTRToString(transid));
	std::string TOKEN = utility_bstr_check(_com_util::ConvertBSTRToString(token));

	HTMLForm params;
	params.set("msg", Utility::GBKEncodingUTF8(MSG));
	params.set("keySn", Utility::GBKEncodingUTF8(KEYSN));
	params.set("transid", TRANSID);
	params.set("token", TOKEN);
	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_CloudSignByP7", body.str());

	return _bstr_t(result.data());
}

BSTR CRSyncControlCtrl::RS_CloudGetSignResult(BSTR transid)
{
	OutputDebugStringA("Enter CRSyncControlCtrl::RS_CloudGetSignResult()\n");
	std::string TRANSID = utility_bstr_check(_com_util::ConvertBSTRToString(transid));

	HTMLForm params;
	params.set("transid", TRANSID);
	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_CloudGetSignResult", body.str());

	OutputDebugStringA(result.c_str());
	OutputDebugStringA("Exit CRSyncControlCtrl::RS_CloudGetSignResult()\n");

	return _bstr_t(result.data());
}

BSTR CRSyncControlCtrl::RS_CloudLogoutAuth(BSTR token)
{
	std::string TOKEN = utility_bstr_check(_com_util::ConvertBSTRToString(token));

	HTMLForm params;
	params.set("token", TOKEN);
	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_CloudLogoutAuth", body.str());

	return _bstr_t(result.data());
}
BSTR CRSyncControlCtrl::RS_CloudEncryptAuth(BSTR transid)
{
	std::string TRANSID = utility_bstr_check(_com_util::ConvertBSTRToString(transid));

	HTMLForm params;
	params.set("transid", TRANSID);
	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_CloudEncryptAuth", body.str());

	return _bstr_t(result.data());
}

BSTR CRSyncControlCtrl::RS_CloudEncryptData(BSTR reachKey, BSTR transid, BSTR token)
{
	std::string REACHKEY = utility_bstr_check(_com_util::ConvertBSTRToString(reachKey));
	std::string TOKEN = utility_bstr_check(_com_util::ConvertBSTRToString(token));
	std::string TRANSID = utility_bstr_check(_com_util::ConvertBSTRToString(transid));

	HTMLForm params;
	params.set("symKey", REACHKEY);
	params.set("token", TOKEN);
	params.set("transid", TRANSID);
	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_CloudEncryptData", body.str());

	return _bstr_t(result.data());
}

BSTR CRSyncControlCtrl::RS_CloudEncryptFile(BSTR souceFilePath, BSTR encFilePath, BSTR transid, BSTR token)
{
	std::string SOUCEFILEPATH = utility_bstr_check(_com_util::ConvertBSTRToString(souceFilePath));
	std::string ENCFILEPATH = utility_bstr_check(_com_util::ConvertBSTRToString(encFilePath));
	std::string TOKEN = utility_bstr_check(_com_util::ConvertBSTRToString(token));
	std::string TRANSID = utility_bstr_check(_com_util::ConvertBSTRToString(transid));

	HTMLForm params;
	params.set("sourceFilePath", Utility::GBKEncodingUTF8(SOUCEFILEPATH));
	params.set("encFilePath", Utility::GBKEncodingUTF8(ENCFILEPATH));
	params.set("token", TOKEN);
	params.set("transid", TRANSID);
	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_CloudEncryptFile", body.str());

	return _bstr_t(result.data());
}
BSTR CRSyncControlCtrl::RS_CloudDevryptAuth(BSTR transid)
{
	std::string TRANSID = utility_bstr_check(_com_util::ConvertBSTRToString(transid));

	HTMLForm params;
	params.set("transid", TRANSID);
	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_CloudDevryptAuth", body.str());

	return _bstr_t(result.data());
}

BSTR CRSyncControlCtrl::RS_CloudDevryptData(BSTR encReachKey, BSTR url, BSTR transid, BSTR token)
{
	std::string ENCREACHKEY = utility_bstr_check(_com_util::ConvertBSTRToString(encReachKey));
	std::string URL = utility_bstr_check(_com_util::ConvertBSTRToString(url));
	std::string TRANSID = utility_bstr_check(_com_util::ConvertBSTRToString(transid));
	std::string TOKEN = utility_bstr_check(_com_util::ConvertBSTRToString(token));

	HTMLForm params;
	params.set("encSymKey", ENCREACHKEY);
	params.set("url", URL);
	params.set("transid", TRANSID);
	params.set("token", TOKEN);
	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_CloudDevryptData", body.str());

	return _bstr_t(result.data());
}

BSTR CRSyncControlCtrl::RS_CloudDevryptFile(BSTR encFilePath, BSTR dncFilePath, BSTR url, BSTR transid, BSTR token)
{
	std::string ENCFILEPATH = utility_bstr_check(_com_util::ConvertBSTRToString(encFilePath));
	std::string DNCFILEPATH = utility_bstr_check(_com_util::ConvertBSTRToString(dncFilePath));
	std::string TRANSID = utility_bstr_check(_com_util::ConvertBSTRToString(transid));
	std::string TOKEN = utility_bstr_check(_com_util::ConvertBSTRToString(token));
	std::string URL = utility_bstr_check(_com_util::ConvertBSTRToString(url));

	HTMLForm params;
	params.set("encFilePath", Utility::GBKEncodingUTF8(ENCFILEPATH));
	params.set("dncFilePath", Utility::GBKEncodingUTF8(DNCFILEPATH));
	params.set("url", URL);
	params.set("transid", TRANSID);
	params.set("token", TOKEN);
	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_CloudDevryptFile", body.str());

	return _bstr_t(result.data());
}

BSTR CRSyncControlCtrl::RS_CloudReceiveDevryptResult(BSTR transid, BSTR token, BSTR devResult)
{
	std::string TRANSID = utility_bstr_check(_com_util::ConvertBSTRToString(transid));
	std::string TOKEN = utility_bstr_check(_com_util::ConvertBSTRToString(token));
	std::string RESULT = utility_bstr_check(_com_util::ConvertBSTRToString(devResult));

	HTMLForm params;
	params.set("transid", TRANSID);
	params.set("token", TOKEN);
	params.set("devResult", RESULT);
	std::ostringstream body;
	params.write(body);
	std::string respond = Utility::SuperRequestGBK("/RS_CloudReceiveDevryptResult", body.str());

	return _bstr_t(respond.data());
}

BSTR CRSyncControlCtrl::RS_CloudGetCompanyCert(BSTR transid, BSTR token)
{
	std::string TRANSID = utility_bstr_check(_com_util::ConvertBSTRToString(transid));
	std::string TOKEN = utility_bstr_check(_com_util::ConvertBSTRToString(token));

	HTMLForm params;
	params.set("transid", TRANSID);
	params.set("token", TOKEN);
	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_CloudGetCompanyCert", body.str());

	return _bstr_t(result.data());
}

BSTR CRSyncControlCtrl::RS_CloudGetCertAuth(BSTR transid)
{
	std::string TRANSID = utility_bstr_check(_com_util::ConvertBSTRToString(transid));
	utility_message_f2("Begin %s transid:%s", std::string(__FUNCTION__), TRANSID);
	HTMLForm params;
	params.set("transid", TRANSID);
	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_CloudGetCertAuth", body.str());
	utility_message_f2("End %s transid:%s", std::string(__FUNCTION__), result);
	return _bstr_t(result.data());
}

BSTR CRSyncControlCtrl::RS_CloudGetCertBase64(BSTR transid, BSTR token)
{
	std::string TRANSID = utility_bstr_check(_com_util::ConvertBSTRToString(transid));
	std::string TOKEN = utility_bstr_check(_com_util::ConvertBSTRToString(token));

	HTMLForm params;
	params.set("transid", TRANSID);
	params.set("token", TOKEN);
	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_CloudGetCertBase64", body.str());

	return _bstr_t(result.data());
}

BSTR CRSyncControlCtrl::RS_CloudLogout(BSTR userId)
{
	std::string USERID = utility_bstr_check(_com_util::ConvertBSTRToString(userId));

	HTMLForm params;
	params.set("userId", USERID);
	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_CloudLogout", body.str());

	return _bstr_t(result.data());
}

BSTR CRSyncControlCtrl::RS_KeyStatus(BSTR containerId)
{
	std::string id = utility_bstr_check(_com_util::ConvertBSTRToString(containerId));
	HTMLForm params;
	params.set("containerId", id);
	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_KeyStatus", body.str());

	return _bstr_t(result.data());
}

// CRSyncControlCtrl 消息处理程序

BSTR CRSyncControlCtrl::RS_EncryptData(BSTR dataStr)
{
	std::string data = utility_bstr_check(_com_util::ConvertBSTRToString(dataStr));

	HTMLForm params;
	params.set("dataStr", Utility::GBKEncodingUTF8(data));
	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_EncryptData", body.str());

	return _bstr_t(result.data());
}

BSTR CRSyncControlCtrl::RS_DevryptData(BSTR symKey, BSTR encDataStr)
{
	std::string SYMKEY = utility_bstr_check(_com_util::ConvertBSTRToString(symKey));
	std::string ENCDATASTR = utility_bstr_check(_com_util::ConvertBSTRToString(encDataStr));
	HTMLForm params;
	params.set("symKey", SYMKEY);
	params.set("encDataStr", ENCDATASTR);
	std::ostringstream body;
	params.write(body);
	std::string result = Utility::SuperRequestGBK("/RS_DevryptData", body.str());
	return _bstr_t(result.data());
}

BOOL CRSyncControlCtrl::IsInvokeAllowed(DISPID /*dispid*/)
{
	// TODO: 在此添加专用代码和/或调用基类

	return TRUE;
}
