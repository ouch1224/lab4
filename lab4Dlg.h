﻿
// lab4Dlg.h: 头文件
//

#pragma once
#include "afxdialogex.h"

// Clab4Dlg 对话框
class Clab4Dlg : public CDialogEx
{
// 构造
public:
	Clab4Dlg(CWnd* pParent = nullptr);	// 标准构造函数
	

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LAB4_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOpen();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedParse();
	afx_msg void OnBnClickedFirst();
	afx_msg void OnBnClickedFollow();
	afx_msg void OnBnClickedLr0();
	afx_msg void OnBnClickedLr1dfa();
	afx_msg void OnBnClickedLr1table();
	
};
