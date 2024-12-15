
// lab4Dlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "lab4.h"
#include "lab4Dlg.h"
#include "afxdialogex.h"
#include "scan.h"
#include "global.h"
#include "Cfirst.h"
#include "Cfollow.h"
#include "CTable.h"
#include "CLR1DFA.h"
#include "CLR0DFA.h"
#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Clab4Dlg 对话框



Clab4Dlg::Clab4Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LAB4_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Clab4Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(Clab4Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OPEN, &Clab4Dlg::OnBnClickedOpen)
	ON_BN_CLICKED(IDC_SAVE, &Clab4Dlg::OnBnClickedSave)
	ON_BN_CLICKED(IDC_PARSE, &Clab4Dlg::OnBnClickedParse)
	ON_BN_CLICKED(IDC_FIRST, &Clab4Dlg::OnBnClickedFirst)
	ON_BN_CLICKED(IDC_FOLLOW, &Clab4Dlg::OnBnClickedFollow)
	ON_BN_CLICKED(IDC_LR0, &Clab4Dlg::OnBnClickedLr0)
	ON_BN_CLICKED(IDC_LR1DFA, &Clab4Dlg::OnBnClickedLr1dfa)
	ON_BN_CLICKED(IDC_LR1TABLE, &Clab4Dlg::OnBnClickedLr1table)
END_MESSAGE_MAP()


// Clab4Dlg 消息处理程序

BOOL Clab4Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void Clab4Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void Clab4Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR Clab4Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void Clab4Dlg::OnBnClickedOpen()
{
	// TODO: 在此添加控件通知处理程序代码
	CString path;
	CFileDialog dlg(TRUE);
	if (dlg.DoModal() == IDOK)
	{
		path = dlg.GetPathName();
		CStdioFile file;
		CString text, line;
		file.Open(path, CFile::modeRead);
		while (file.ReadString(line))
		{
			text = text+ line+ L"\r\n";
		}
		file.Close();
		SetDlgItemText(IDC_INPUT, text);
	}
	else
	{
		MessageBox(L"未选择文件");
		return;
	}
}


void Clab4Dlg::OnBnClickedSave()
{
	// TODO: 在此添加控件通知处理程序代码
	CString text, path, fileName = L"temp.txt";
	CFileDialog dlg(FALSE);
	dlg.m_ofn.lpstrTitle = L"我的文件保存对话框";
	dlg.m_ofn.lpstrFilter = L"Text Files(*.txt)\0*.txt\0All Files(*.*)\0*.*\0\0";
	dlg.m_ofn.lpstrDefExt = L"txt";
	if (IDOK == dlg.DoModal())
	{
		path = dlg.GetPathName();
		fileName = dlg.GetFileName();
		GetDlgItemText(IDC_INPUT, text);
		//创建文件
		CStdioFile file;
		file.Open(path, CFile::modeCreate | CFile::modeWrite);
		file.SeekToEnd();
		file.WriteString(text);
		file.Flush();
		//关闭文件
		file.Close();
	}
}

scan ss;


void Clab4Dlg::OnBnClickedParse()
{
	// TODO: 在此添加控件通知处理程序代码
	//从编辑框里获取文法，转为string，存入vector<string> lines; 
	
	CString ctext;
	GetDlgItemText(IDC_INPUT, ctext);
	if (ctext.IsEmpty())
	{
		MessageBox(L"请选择文件或直接输入源代码！", L"错误", MB_OK | MB_ICONWARNING);
		return;
	}
	else {
		string text;
		
		text = CT2A(ctext);
		istringstream stream(text);
		string line;
		ss.initialize();
		// 按行读取并添加到 vector  
		while (getline(stream, line)) {
			ss.lines.push_back(line);
		}
		ss.scanGrams();  // 扫描全部文法
		ss.splitGrams();
		ss.extendStartGram();  // 扩充开始文法
		ss.buildFirstAll();  // 求全部First集合
		ss.buildFollowAll();  // 求全部Follow集合
		//ss.buildLR0Items();
		int slr = ss.SLR1Analyse();
		if (slr == 0) SetDlgItemText(IDC_SLR, L"是");
		if (slr == 1) SetDlgItemText(IDC_SLR, L"不是，有移进-规约冲突");
		if (slr == 2) SetDlgItemText(IDC_SLR, L"不是，有规约-规约冲突");
		if (slr == 3) SetDlgItemText(IDC_SLR, L"不是，有移进-规约冲突和规约-规约冲突");
		ss.buildLR1Item();  // 构建LR(1)项目集
		ss.buildLR1Table();
		MessageBox(L"解析成功！");
	}
}


void Clab4Dlg::OnBnClickedFirst()
{
	// TODO: 在此添加控件通知处理程序代码
	
    if(ss.First.empty())
	{
		MessageBox(L"请先解析文法！", L"错误", MB_OK | MB_ICONWARNING);
		return;
	}
	else
	{
		string firtext="";
		for (auto x : ss.First)
		{
			firtext = firtext + x.first + '\t';
			for (auto& s : ss.First[x.first])
				firtext = firtext + s + ' ';
			firtext += "\r\n";
		}
		CString cfirtext = CString(firtext.c_str());
		Cfirst dlg;
		dlg.m_OutputText = cfirtext;
		// 显示对话框  
		dlg.DoModal();

	}
}


void Clab4Dlg::OnBnClickedFollow()
{
	// TODO: 在此添加控件通知处理程序代码
	if (ss.Follow.empty())
	{
		MessageBox(L"请先解析文法！", L"错误", MB_OK | MB_ICONWARNING);
		return;
	}
	else
	{
		string fotext = "";
		for (auto x : ss.Follow)
		{
			fotext = fotext + x.first + '\t';
			for (auto& s : ss.Follow[x.first])
				fotext = fotext + s + ' ';
			fotext += "\r\n";
		}
		CString cfotext = CString(fotext.c_str());
		Cfollow dlg;
		dlg.m_OutputText = cfotext;
		// 显示对话框  
		dlg.DoModal();

	}
}



void Clab4Dlg::OnBnClickedLr0()
{
	// TODO: 在此添加控件通知处理程序代码
	string dfa0;
	for (int i = 0; i < ss.LR0Items.size(); ++i)
	{
		dfa0 = dfa0 + "--Item[" + to_string(i) + "]--\r\n";
		for (auto& x : ss.LR0Items[i].LR0Grams) {
			dfa0 = dfa0 + "[ " + x.gram.name + " -> ";
			for (auto& y : x.former)
				dfa0 = dfa0 + y + " ";
			dfa0 = dfa0 + "·";
			for (auto& y : x.gram.tokens)
				dfa0 = dfa0 + y + " ";
			dfa0 = dfa0 + ", "  + " ]\r\n";
		}
	}
	dfa0 = dfa0 + "\r\n边集：\r\n";
	for (auto& e : ss.LR0Edges)
		dfa0 = dfa0 + to_string(e.x) + '\t' + e.s + '\t' + to_string(e.y) + "\r\n";
	CString ctext = CString(dfa0.c_str());
	CLR0DFA dlg;
	dlg.m_OutputText = ctext;
	// 显示对话框  
	dlg.DoModal();

}


void Clab4Dlg::OnBnClickedLr1dfa()
{
	// TODO: 在此添加控件通知处理程序代码
	string dfa1;
	for (int i = 0; i < ss.LR1Items.size(); ++i)
	{
		dfa1 = dfa1 + "--Item[" + to_string(i) + "]--\r\n";
		for (auto& x : ss.LR1Items[i].LRGrams) {
			dfa1 = dfa1+ "[ " + x.gram.name + " -> ";
			for (auto& y : x.former)
				dfa1 = dfa1+ y + " ";
			dfa1 = dfa1+ "·";
			for (auto& y : x.gram.tokens)
				dfa1=dfa1+ y + " ";
			dfa1=dfa1+ ", " + x.term + " ]\r\n";
		}
	}
	dfa1 = dfa1 + "\r\n边集：\r\n";
	for (auto& e : ss.LR1Edges)
		dfa1 = dfa1 + to_string(e.x) + '\t' + e.s + '\t' + to_string(e.y) + "\r\n";
	CString ctext = CString(dfa1.c_str());
	CLR1DFA dlg;
	dlg.m_OutputText = ctext;
	// 显示对话框  
	dlg.DoModal();
}


void Clab4Dlg::OnBnClickedLr1table()
{
	// TODO: 在此添加控件通知处理程序代码

	string table = "--ACTION表--\r\n";
	for (auto& x : ss.LR1ACTION)
	    table= table + to_string(x.first.first) + '\t' + x.first.second + '\t' + x.second + "\r\n";
	table = table+ "--GOTO表--\r\n";
	for (auto& x : ss.LR1GOTO)
	    table = table+to_string( x.first.first )+ '\t' + x.first.second + '\t' + x.second +"\r\n";
	CString ctext = CString(table.c_str());
	CTable dlg;
	dlg.m_OutputText = ctext;
	// 显示对话框  
	dlg.DoModal();
}
