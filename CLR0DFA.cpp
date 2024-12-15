// CLR0DFA.cpp: 实现文件
//

#include "pch.h"
#include "lab4.h"
#include "afxdialogex.h"
#include "CLR0DFA.h"


// CLR0DFA 对话框

IMPLEMENT_DYNAMIC(CLR0DFA, CDialogEx)

CLR0DFA::CLR0DFA(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_LR0, pParent)
{

}

CLR0DFA::~CLR0DFA()
{
}

void CLR0DFA::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLR0DFA, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT1_LR0, &CLR0DFA::OnEnChangeEdit1Lr0)
END_MESSAGE_MAP()


// CLR0DFA 消息处理程序
BOOL CLR0DFA::OnInitDialog() {
	CDialogEx::OnInitDialog();

	m_EditOutput.SubclassDlgItem(IDC_EDIT1_LR0, this);
	m_EditOutput.SetWindowText(m_OutputText); // 设置文本框内容  

	return TRUE;  // 返回 TRUE 以设置焦点到默认的控件  
}

void CLR0DFA::OnEnChangeEdit1Lr0()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}
