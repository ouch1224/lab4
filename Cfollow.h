#pragma once
#include "afxdialogex.h"


// Cfollow 对话框

class Cfollow : public CDialogEx
{
	DECLARE_DYNAMIC(Cfollow)

public:
	Cfollow(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~Cfollow();
	CString m_OutputText;
	virtual BOOL OnInitDialog();
	CEdit m_EditOutput;
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_fo };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEditFo();
};
