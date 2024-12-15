// CTable.cpp: 实现文件
//

#include "pch.h"
#include "lab4.h"
#include "afxdialogex.h"
#include "CTable.h"


// CTable 对话框

IMPLEMENT_DYNAMIC(CTable, CDialogEx)

CTable::CTable(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_lr1t, pParent)
{

}

CTable::~CTable()
{
}

void CTable::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTable, CDialogEx)
	
	
	
{
	0x0111, (WORD)0x0300, (WORD)IDC_EDIT_TAA, (WORD)IDC_EDIT_TAA, AfxSigCmd_v, (static_cast< AFX_PMSG > (&CTable::OnEnChangeEditTaa))
},
END_MESSAGE_MAP()


// CTable 消息处理程序
BOOL CTable::OnInitDialog() {
	CDialogEx::OnInitDialog();

	m_EditOutput.SubclassDlgItem(IDC_EDIT_TAA, this);
	m_EditOutput.SetWindowText(m_OutputText); // 设置文本框内容  

	return TRUE;  // 返回 TRUE 以设置焦点到默认的控件  
}





void CTable::OnEnChangeEditTaa()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}
