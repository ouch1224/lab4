// Cfirst.cpp: 实现文件
//

#include "pch.h"
#include "lab4.h"
#include "afxdialogex.h"
#include "Cfirst.h"


// Cfirst 对话框

IMPLEMENT_DYNAMIC(Cfirst, CDialogEx)

Cfirst::Cfirst(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_first, pParent)
{

}

Cfirst::~Cfirst()
{
}

void Cfirst::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Cfirst, CDialogEx)
	ON_EN_CHANGE(IDC_SHOWFIR, &Cfirst::OnEnChangeShowfir)
END_MESSAGE_MAP()


// Cfirst 消息处理程序

BOOL Cfirst::OnInitDialog() {
	CDialogEx::OnInitDialog();

	m_EditOutput.SubclassDlgItem(IDC_SHOWFIR, this);
	m_EditOutput.SetWindowText(m_OutputText); // 设置文本框内容  

	return TRUE;  // 返回 TRUE 以设置焦点到默认的控件  
}

void Cfirst::OnEnChangeShowfir()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}
