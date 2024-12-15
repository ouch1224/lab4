#pragma once
#include "afxdialogex.h"


// CLR1DFA 对话框

class CLR1DFA : public CDialogEx
{
	DECLARE_DYNAMIC(CLR1DFA)

public:
	CLR1DFA(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CLR1DFA();
	CString m_OutputText;
	virtual BOOL OnInitDialog();
	CEdit m_EditOutput;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1_DFA };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEdit1Dfa();
};
