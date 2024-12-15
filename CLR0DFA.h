#pragma once
#include "afxdialogex.h"


// CLR0DFA 对话框

class CLR0DFA : public CDialogEx
{
	DECLARE_DYNAMIC(CLR0DFA)

public:
	CLR0DFA(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CLR0DFA();
	CString m_OutputText;
	virtual BOOL OnInitDialog();
	CEdit m_EditOutput;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_LR0 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEdit1Lr0();
};
