#pragma once
#include "afxdialogex.h"


// CTable 对话框

class CTable : public CDialogEx
{
	DECLARE_DYNAMIC(CTable)

public:
	CTable(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CTable();
	CString m_OutputText;
	virtual BOOL OnInitDialog();
	CEdit m_EditOutput;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_lr1t };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	
	afx_msg void OnEnChangeEditTaa();
};
