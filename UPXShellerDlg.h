// UPXShellerDlg.h: 头文件
//

#pragma once

#include <afxcmn.h> // CProgressCtrl

// CUPXShellerDlg 对话框
class CUPXShellerDlg : public CDialogEx
{
// 构造
public:
	CUPXShellerDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UPXSHELLER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 控件变量
CComboBox   m_comboLevel;
CComboBox   m_comboUPXVersion;
CEdit       m_editInput;
CEdit       m_editOutput;
CEdit       m_editCmdPreview;
CEdit       m_editOutputInfo;
//CProgressCtrl m_progress;

// 变量
CString     m_upxPath;
CString     m_inputFile;
CString     m_outputFile;

// 方法
void UpdateCmdPreview();
void EnableControls(BOOL bEnable);
void ScanUPXVersions();
CString GetSelectedUPXPath();
int DetectExeArchitecture(const CString& exePath); // 检测EXE文件架构
void SelectUPXVersionForArchitecture(int arch); // 选择适合架构的UPX版本
int CompareVersionStrings(const CString& version1, const CString& version2); // 比较版本字符串

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedBtnBrowseInput();
	afx_msg void OnBnClickedBtnDeleteInput();
	afx_msg void OnBnClickedBtnBrowseOutput();
	afx_msg void OnCbnSelchangeComboLevel();
	afx_msg void OnCbnSelchangeComboUPXVersion();
	afx_msg void OnRadioClicked();
	afx_msg void OnCheckClicked();
	afx_msg void OnEnChangeEdit();
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedBtnExit();
	afx_msg LRESULT OnUpdateProgress(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateOutput(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAutoSwitchMode(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnOpFailed(WPARAM wParam, LPARAM);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	DECLARE_MESSAGE_MAP()
};