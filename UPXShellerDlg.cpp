
// UPXShellerDlg.cpp: 实现文件
//

#include "pch.h"
#include "UPXSheller.h"
#include "UPXShellerDlg.h"
#include "afxdialogex.h"
#include <shellapi.h>
#include <thread>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CUPXShellerDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BTN_BROWSE_INPUT, &CUPXShellerDlg::OnBnClickedBtnBrowseInput)
    ON_BN_CLICKED(IDC_BTN_DELETE_INPUT, &CUPXShellerDlg::OnBnClickedBtnDeleteInput)
    ON_BN_CLICKED(IDC_BTN_BROWSE_OUTPUT, &CUPXShellerDlg::OnBnClickedBtnBrowseOutput)
    ON_CBN_SELCHANGE(IDC_COMBO_LEVEL, &CUPXShellerDlg::OnCbnSelchangeComboLevel)
    ON_CBN_SELCHANGE(IDC_COMBO_UPX_VERSION, &CUPXShellerDlg::OnCbnSelchangeComboUPXVersion)
    ON_BN_CLICKED(IDC_RADIO_COMPRESS, &CUPXShellerDlg::OnRadioClicked)
    ON_BN_CLICKED(IDC_RADIO_DECOMPRESS, &CUPXShellerDlg::OnRadioClicked)
    ON_BN_CLICKED(IDC_RADIO_LIST, &CUPXShellerDlg::OnRadioClicked)
    ON_BN_CLICKED(IDC_RADIO_TEST, &CUPXShellerDlg::OnRadioClicked)
    ON_BN_CLICKED(IDC_RADIO_VERSION, &CUPXShellerDlg::OnRadioClicked)
    ON_BN_CLICKED(IDC_RADIO_HELP, &CUPXShellerDlg::OnRadioClicked)
    ON_BN_CLICKED(IDC_RADIO_LICENSE, &CUPXShellerDlg::OnRadioClicked)
    ON_BN_CLICKED(IDC_RADIO_QUIET, &CUPXShellerDlg::OnRadioClicked)
    ON_BN_CLICKED(IDC_RADIO_VERBOSE, &CUPXShellerDlg::OnRadioClicked)
    ON_BN_CLICKED(IDC_CHECK_FORCE, &CUPXShellerDlg::OnCheckClicked)
    ON_BN_CLICKED(IDC_CHECK_BACKUP, &CUPXShellerDlg::OnCheckClicked)
    ON_EN_CHANGE(IDC_EDIT_INPUT, &CUPXShellerDlg::OnEnChangeEdit)
    ON_EN_CHANGE(IDC_EDIT_OUTPUT, &CUPXShellerDlg::OnEnChangeEdit)
    ON_BN_CLICKED(IDC_BTN_START, &CUPXShellerDlg::OnBnClickedBtnStart)
    ON_BN_CLICKED(IDC_BTN_EXIT, &CUPXShellerDlg::OnBnClickedBtnExit)
    ON_MESSAGE(WM_USER + 2, &CUPXShellerDlg::OnUpdateProgress)
    ON_MESSAGE(WM_USER + 3, &CUPXShellerDlg::OnUpdateOutput)
    ON_MESSAGE(WM_USER + 10, &CUPXShellerDlg::OnAutoSwitchMode)
    ON_MESSAGE(WM_USER + 11, &CUPXShellerDlg::OnOpFailed)
    ON_WM_DROPFILES()
END_MESSAGE_MAP()

// 构造函数实现
CUPXShellerDlg::CUPXShellerDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_UPXSHELLER_DIALOG, pParent)
{
    // 可以初始化成员变量
}

void CUPXShellerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    // 如果你有 DDX/DDV 绑控件，可以在这里添加
    // 例如：DDX_Control(pDX, IDC_COMBO_LEVEL, m_comboLevel);
    // 例如：DDX_Control(pDX, IDC_EDIT_INPUT, m_editInput);
    // 例如：DDX_Control(pDX, IDC_PROGRESS1, m_progress);
}

BOOL CUPXShellerDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    m_comboLevel.SubclassDlgItem(IDC_COMBO_LEVEL, this);
    m_comboUPXVersion.SubclassDlgItem(IDC_COMBO_UPX_VERSION, this);
    m_editInput.SubclassDlgItem(IDC_EDIT_INPUT, this);
    m_editOutput.SubclassDlgItem(IDC_EDIT_OUTPUT, this);
    m_editCmdPreview.SubclassDlgItem(IDC_EDIT_CMD_PREVIEW, this);
    m_editOutputInfo.SubclassDlgItem(IDC_EDIT_OUTPUTINFO, this);
    //m_progress.SubclassDlgItem(IDC_PROGRESS1, this); // 你需要在资源编辑器里加一个进度条控件，ID为IDC_PROGRESS1
    // 初始化压缩级别（带标签和建议）
    m_comboLevel.AddString(_T("-1 (最快，速度优先)"));
    m_comboLevel.AddString(_T("-2"));
    m_comboLevel.AddString(_T("-3"));
    m_comboLevel.AddString(_T("-4"));
    m_comboLevel.AddString(_T("-5 (默认，推荐)"));
    m_comboLevel.AddString(_T("-6"));
    m_comboLevel.AddString(_T("-7"));
    m_comboLevel.AddString(_T("-8"));
    m_comboLevel.AddString(_T("-9 (最小体积，最慢)"));
    m_comboLevel.SetCurSel(4); // 默认-5
    // 默认选中"压缩"、"安静"
    CheckDlgButton(IDC_RADIO_COMPRESS, BST_CHECKED);
    CheckDlgButton(IDC_RADIO_QUIET, BST_CHECKED);
    // 扫描并填充UPX版本
    ScanUPXVersions();
    // 如果ScanUPXVersions没有找到任何版本，设置默认路径
    if(m_comboUPXVersion.GetCount() == 0) {
        m_comboUPXVersion.AddString(_T("upx-5.0.1-win64"));
        m_comboUPXVersion.SetCurSel(0);
        m_upxPath = _T("upx\\upx-5.0.1-win64\\upx.exe");
    }
    UpdateCmdPreview();
    //m_progress.SetRange(0, 100);
    //m_progress.SetPos(0);
    // 设置压缩等级建议（假设你在界面上加了一个静态文本控件IDC_STATIC_LEVEL_HINT）
//     CStatic* pHint = (CStatic*)GetDlgItem(IDC_STATIC_LEVEL_HINT);
//     if (pHint) {
//         pHint->SetWindowText(_T("建议：-1 速度最快，-5 推荐，-9 体积最小但最慢"));
//     }
    DragAcceptFiles(TRUE);
    return TRUE;
}

// 扫描UPX版本
void CUPXShellerDlg::ScanUPXVersions()
{
    m_comboUPXVersion.ResetContent(); // 清空现有内容

    // 首先检查当前目录下的upx子目录
    CString upxDir = _T("upx");
    CFileFind finder;
    CString searchPath = upxDir + _T("\\*.*");
    
    BOOL bWorking = finder.FindFile(searchPath);
    BOOL foundAny = FALSE;
    
    // 创建一个临时数组来存储找到的版本
    CStringArray versionArray;
    
    while (bWorking)
    {
        bWorking = finder.FindNextFile();
        
        // 跳过.和..
        if (finder.IsDots())
            continue;
            
        // 只处理目录
        if (finder.IsDirectory())
        {
            CString folderName = finder.GetFileName();
            // 检查是否以"upx-"开头，表示是UPX版本目录
            if (folderName.Find(_T("upx-")) == 0)
            {
                // 检查目录中是否存在upx.exe
                CString upxExePath = upxDir + _T("\\") + folderName + _T("\\upx.exe");
                if (GetFileAttributes(upxExePath) != INVALID_FILE_ATTRIBUTES)
                {
                    versionArray.Add(folderName);
                    foundAny = TRUE;
                }
            }
        }
    }
    finder.Close();
    
    // 如果在upx子目录中没有找到，扫描当前目录
    if (!foundAny)
    {
        CFileFind finder2;
        CString searchPath2 = _T("*.*");
        
        bWorking = finder2.FindFile(searchPath2);
        while (bWorking)
        {
            bWorking = finder2.FindNextFile();
            
            // 跳过.和..
            if (finder2.IsDots())
                continue;
                
            // 只处理目录
            if (finder2.IsDirectory())
            {
                CString folderName = finder2.GetFileName();
                // 检查是否以"upx-"开头，表示是UPX版本目录
                if (folderName.Find(_T("upx-")) == 0)
                {
                    // 检查目录中是否存在upx.exe
                    CString upxExePath = folderName + _T("\\upx.exe");
                    if (GetFileAttributes(upxExePath) != INVALID_FILE_ATTRIBUTES)
                    {
                        versionArray.Add(folderName);
                        foundAny = TRUE;
                    }
                }
            }
        }
        finder2.Close();
    }
    
    // 对版本进行排序（按字母顺序，最新版本在后）
    int nCount = (int)versionArray.GetSize();
    for (int i = 0; i < nCount - 1; i++)
    {
        for (int j = i + 1; j < nCount; j++)
        {
            if (versionArray[i] > versionArray[j]) // 按字母顺序排序
            {
                CString temp = versionArray[i];
                versionArray[i] = versionArray[j];
                versionArray[j] = temp;
            }
        }
    }
    
    // 将排序后的版本添加到下拉框中
    for (int i = 0; i < nCount; i++)
    {
        m_comboUPXVersion.AddString(versionArray[i]);
    }
    
    // 如果找到版本，选择最后一个（最新版本）；否则添加默认选项
    int count = m_comboUPXVersion.GetCount();
    if (count > 0)
    {
        m_comboUPXVersion.SetCurSel(count - 1); // 选择最新版本（最后一个）
        // 更新当前UPX路径
        m_upxPath = GetSelectedUPXPath();
    }
    else
    {
        // 如果没有找到版本目录，添加默认选项
        m_comboUPXVersion.AddString(_T("upx-5.0.1-win64"));
        m_comboUPXVersion.SetCurSel(0);
        m_upxPath = _T("upx\\upx-5.0.1-win64\\upx.exe");
    }
}

// 获取选中的UPX路径
CString CUPXShellerDlg::GetSelectedUPXPath()
{
    int sel = m_comboUPXVersion.GetCurSel();
    if (sel == CB_ERR)
        return _T("upx\\upx-5.0.1-win64\\upx.exe"); // 默认路径
    
    CString versionDir;
    m_comboUPXVersion.GetLBText(sel, versionDir);
    return _T("upx\\") + versionDir + _T("\\upx.exe");
}

// UPX版本选择改变事件
void CUPXShellerDlg::OnCbnSelchangeComboUPXVersion()
{
    m_upxPath = GetSelectedUPXPath();
    UpdateCmdPreview();
}

void CUPXShellerDlg::OnBnClickedBtnBrowseInput()
{
    CFileDialog dlg(TRUE, _T("exe"), NULL, OFN_FILEMUSTEXIST, _T("可执行文件 (*.exe)|*.exe||"));
    if (dlg.DoModal() == IDOK)
    {
        m_inputFile = dlg.GetPathName();
        m_editInput.SetWindowText(m_inputFile);
        UpdateCmdPreview();

        // 自动执行列出命令
        CString cmd;
        cmd.Format(_T("\"%s\" -l \"%s\""), m_upxPath, m_inputFile);
        
        m_editCmdPreview.SetWindowText(cmd);
        m_editOutputInfo.SetWindowText(_T("正在获取文件信息..."));
        EnableControls(FALSE);
        std::thread([=]() {
            SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
            HANDLE hRead, hWrite;
            CreatePipe(&hRead, &hWrite, &sa, 0);
            STARTUPINFO si = { sizeof(STARTUPINFO) };
            si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
            si.hStdOutput = hWrite;
            si.hStdError = hWrite;
            si.wShowWindow = SW_HIDE;
            PROCESS_INFORMATION pi;
            CStringA cmdA(cmd);
            BOOL ret = CreateProcessA(NULL, (LPSTR)(LPCSTR)cmdA, NULL, NULL, TRUE, 0, NULL, NULL, (LPSTARTUPINFOA)&si, &pi);
            CloseHandle(hWrite);
            CString output;
            bool isPacked = false, isNotPacked = false;
            if (!ret)
            {
                output = _T("列出文件信息失败！");
            }
            else
            {
                char buffer[4096] = { 0 };
                DWORD read = 0;
                while (ReadFile(hRead, buffer, sizeof(buffer) - 1, &read, NULL) && read > 0)
                {
                    buffer[read] = 0;
                    output += CString(buffer);
                }
                WaitForSingleObject(pi.hProcess, INFINITE);
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
                // 检查是否已压缩
                CString lower = output;
                lower.MakeLower();
                if (output.Find(_T("->")) != -1 && output.Find(_T("%")) != -1)
                    isPacked = true;
                if (lower.Find(_T("notpackedexception")) != -1)
                    isNotPacked = true;
            }
            CloseHandle(hRead);
            // 自动切换操作类型
            if (isPacked)
                PostMessage(WM_USER + 10, 1, 0); // 1=解压
            else if (isNotPacked)
                PostMessage(WM_USER + 10, 0, 0); // 0=压缩
            PostMessage(WM_USER + 3, (WPARAM)new CString(output), 0);
            EnableControls(TRUE);
        }).detach();
    }
}

void CUPXShellerDlg::OnBnClickedBtnDeleteInput()
{
    m_inputFile.Empty();
    m_editInput.SetWindowText(_T(""));
    UpdateCmdPreview();
}

void CUPXShellerDlg::OnBnClickedBtnBrowseOutput()
{
    CFileDialog dlg(FALSE, _T("exe"), NULL, 0, _T("可执行文件 (*.exe)|*.exe||"));
    if (dlg.DoModal() == IDOK)
    {
        m_outputFile = dlg.GetPathName();
        m_editOutput.SetWindowText(m_outputFile);
        UpdateCmdPreview();
    }
}

void CUPXShellerDlg::UpdateCmdPreview()
{
    CString cmd;
    cmd.Format(_T("\"%s\" "), m_upxPath);
    // 操作类型
    if (IsDlgButtonChecked(IDC_RADIO_COMPRESS)) {}
    else if (IsDlgButtonChecked(IDC_RADIO_DECOMPRESS)) cmd += _T("-d ");
    else if (IsDlgButtonChecked(IDC_RADIO_LIST)) cmd += _T("-l ");
    else if (IsDlgButtonChecked(IDC_RADIO_TEST)) cmd += _T("-t ");
    else if (IsDlgButtonChecked(IDC_RADIO_VERSION)) cmd += _T("-V ");
    else if (IsDlgButtonChecked(IDC_RADIO_HELP)) cmd += _T("-h ");
    else if (IsDlgButtonChecked(IDC_RADIO_LICENSE)) cmd += _T("-L ");
    // 压缩级别
    if (IsDlgButtonChecked(IDC_RADIO_COMPRESS))
    {
        int sel = m_comboLevel.GetCurSel();
        if (sel != CB_ERR)
        {
            CString level;
            m_comboLevel.GetLBText(sel, level);
            CString levelShort = level.Left(2); // 只取“-1”~“-9”
            cmd += levelShort + _T(" ");
        }
    }
    // 模式
    if (IsDlgButtonChecked(IDC_RADIO_QUIET)) cmd += _T("-q ");
    if (IsDlgButtonChecked(IDC_RADIO_VERBOSE)) cmd += _T("-v ");
    // 选项
    if (IsDlgButtonChecked(IDC_CHECK_FORCE)) cmd += _T("-f ");
    if (IsDlgButtonChecked(IDC_CHECK_BACKUP)) cmd += _T("-k ");
    // 输出文件
    CString out;
    m_editOutput.GetWindowText(out);
    if (!out.IsEmpty())
        cmd += _T("-o \"") + out + _T("\" ");
    // 输入文件
    CString in;
    m_editInput.GetWindowText(in);
    if (!in.IsEmpty())
        cmd += _T("\"") + in + _T("\"");
    m_editCmdPreview.SetWindowText(cmd);
}

void CUPXShellerDlg::OnCbnSelchangeComboLevel() { UpdateCmdPreview(); }
void CUPXShellerDlg::OnRadioClicked()
{
    UpdateCmdPreview();
    // 检查压缩/解压选择与实际状态
    CString in;
    m_editInput.GetWindowText(in);
    if (in.IsEmpty()) return;
    // 执行upx -l检测
    CString listCmd;
    listCmd.Format(_T("\"%s\" -l \"%s\""), m_upxPath, in);
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    HANDLE hRead, hWrite;
    CreatePipe(&hRead, &hWrite, &sa, 0);
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.hStdOutput = hWrite;
    si.hStdError = hWrite;
    si.wShowWindow = SW_HIDE;
    PROCESS_INFORMATION pi;
    CStringA cmdA(listCmd);
    BOOL ret = CreateProcessA(NULL, (LPSTR)(LPCSTR)cmdA, NULL, NULL, TRUE, 0, NULL, NULL, (LPSTARTUPINFOA)&si, &pi);
    CloseHandle(hWrite);
    CString output;
    bool isPacked = false, isNotPacked = false;
    if (ret)
    {
        char buffer[4096] = { 0 };
        DWORD read = 0;
        while (ReadFile(hRead, buffer, sizeof(buffer) - 1, &read, NULL) && read > 0)
        {
            buffer[read] = 0;
            output += CString(buffer);
        }
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        CString lower = output;
        lower.MakeLower();
        if (output.Find(_T("->")) != -1 && output.Find(_T("%")) != -1)
            isPacked = true;
        if (lower.Find(_T("notpackedexception")) != -1)
            isNotPacked = true;
    }
    CloseHandle(hRead);
    BOOL wantDecompress = IsDlgButtonChecked(IDC_RADIO_DECOMPRESS);
    BOOL wantCompress = IsDlgButtonChecked(IDC_RADIO_COMPRESS);
    if (isPacked && wantCompress)
    {
        AfxMessageBox(_T("该文件已压缩，请选择解压模式！"));
        CheckRadioButton(IDC_RADIO_COMPRESS, IDC_RADIO_LICENSE, IDC_RADIO_DECOMPRESS);
        UpdateCmdPreview();
    }
    if (isNotPacked && wantDecompress)
    {
        AfxMessageBox(_T("该文件未压缩，请选择压缩模式！"));
        CheckRadioButton(IDC_RADIO_COMPRESS, IDC_RADIO_LICENSE, IDC_RADIO_COMPRESS);
        UpdateCmdPreview();
    }
}
void CUPXShellerDlg::OnCheckClicked() { UpdateCmdPreview(); }
void CUPXShellerDlg::OnEnChangeEdit() { UpdateCmdPreview(); }

void CUPXShellerDlg::EnableControls(BOOL bEnable)
{
    GetDlgItem(IDC_BTN_BROWSE_INPUT)->EnableWindow(bEnable);
    GetDlgItem(IDC_BTN_DELETE_INPUT)->EnableWindow(bEnable);
    GetDlgItem(IDC_BTN_BROWSE_OUTPUT)->EnableWindow(bEnable);
    GetDlgItem(IDC_BTN_START)->EnableWindow(bEnable);
    // 其他控件...
}

// 新增消息处理，自动切换操作类型
LRESULT CUPXShellerDlg::OnAutoSwitchMode(WPARAM wParam, LPARAM)
{
    if (wParam == 1) // 已压缩，切解压
        CheckRadioButton(IDC_RADIO_COMPRESS, IDC_RADIO_LICENSE, IDC_RADIO_DECOMPRESS);
    else // 未压缩，切压缩
        CheckRadioButton(IDC_RADIO_COMPRESS, IDC_RADIO_LICENSE, IDC_RADIO_COMPRESS);
    UpdateCmdPreview();
    return 0;
}

void CUPXShellerDlg::OnBnClickedBtnStart()
{
    CString cmd;
    m_editCmdPreview.GetWindowText(cmd);
    m_editInput.GetWindowText(m_inputFile);
    if (m_inputFile.IsEmpty())
    {
        AfxMessageBox(_T("请选择输入文件！"));
        return;
    }
    // 再次列出，判断实际状态
    CString listCmd;
    listCmd.Format(_T("\"%s\" -l \"%s\""), m_upxPath, m_inputFile);
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    HANDLE hRead, hWrite;
    CreatePipe(&hRead, &hWrite, &sa, 0);
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.hStdOutput = hWrite;
    si.hStdError = hWrite;
    si.wShowWindow = SW_HIDE;
    PROCESS_INFORMATION pi;
    CStringA cmdA(listCmd);
    BOOL ret = CreateProcessA(NULL, (LPSTR)(LPCSTR)cmdA, NULL, NULL, TRUE, 0, NULL, NULL, (LPSTARTUPINFOA)&si, &pi);
    CloseHandle(hWrite);
    CString output;
    bool isPacked = false, isNotPacked = false;
    if (ret)
    {
        char buffer[4096] = { 0 };
        DWORD read = 0;
        while (ReadFile(hRead, buffer, sizeof(buffer) - 1, &read, NULL) && read > 0)
        {
            buffer[read] = 0;
            output += CString(buffer);
        }
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        CString lower = output;
        lower.MakeLower();
        if (output.Find(_T("->")) != -1 && output.Find(_T("%")) != -1)
            isPacked = true;
        if (lower.Find(_T("notpackedexception")) != -1)
            isNotPacked = true;
    }
    CloseHandle(hRead);
    BOOL wantDecompress = IsDlgButtonChecked(IDC_RADIO_DECOMPRESS);
    BOOL wantCompress = IsDlgButtonChecked(IDC_RADIO_COMPRESS);
    if (isPacked && wantCompress)
    {
        AfxMessageBox(_T("该文件已压缩，请选择解压模式！"));
        return;
    }
    if (isNotPacked && wantDecompress)
    {
        AfxMessageBox(_T("该文件未压缩，请选择压缩模式！"));
        return;
    }
    EnableControls(FALSE);
    //m_progress.SetPos(0);
    m_editOutputInfo.SetWindowText(_T(""));
    std::thread([=]() {
        SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
        HANDLE hRead, hWrite;
        CreatePipe(&hRead, &hWrite, &sa, 0);
        STARTUPINFO si = { sizeof(STARTUPINFO) };
        si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
        si.hStdOutput = hWrite;
        si.hStdError = hWrite;
        si.wShowWindow = SW_HIDE;
        PROCESS_INFORMATION pi;
        CStringA cmdA(cmd);
        BOOL ret = CreateProcessA(NULL, (LPSTR)(LPCSTR)cmdA, NULL, NULL, TRUE, 0, NULL, NULL, (LPSTARTUPINFOA)&si, &pi);
        CloseHandle(hWrite);
        if (!ret)
        {
            PostMessage(WM_USER + 1, 0, 0); // 失败
            return;
        }
        char buffer[4096] = { 0 };
        DWORD read = 0;
        CString output;
        int progress = 0;
        while (ReadFile(hRead, buffer, sizeof(buffer) - 1, &read, NULL) && read > 0)
        {
            buffer[read] = 0;
            output += CString(buffer);
            // 简单进度模拟
            progress += 10;
            if (progress > 100) progress = 100;
            PostMessage(WM_USER + 2, progress, 0);
            PostMessage(WM_USER + 3, (WPARAM)new CString(output), 0);
        }
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        CloseHandle(hRead);
        PostMessage(WM_USER + 2, 100, 0); // 进度100%
        // 命令执行完毕后，检测状态
        CString checkCmd;
        checkCmd.Format(_T("\"%s\" -l \"%s\""), m_upxPath, m_inputFile);
        SECURITY_ATTRIBUTES sa2 = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
        HANDLE hRead2, hWrite2;
        CreatePipe(&hRead2, &hWrite2, &sa2, 0);
        STARTUPINFO si2 = { sizeof(STARTUPINFO) };
        si2.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
        si2.hStdOutput = hWrite2;
        si2.hStdError = hWrite2;
        si2.wShowWindow = SW_HIDE;
        PROCESS_INFORMATION pi2;
        CStringA cmdA2(checkCmd);
        BOOL ret2 = CreateProcessA(NULL, (LPSTR)(LPCSTR)cmdA2, NULL, NULL, TRUE, 0, NULL, NULL, (LPSTARTUPINFOA)&si2, &pi2);
        CloseHandle(hWrite2);
        CString output2;
        bool isPacked = false, isNotPacked = false;
        if (ret2)
        {
            char buffer2[4096] = { 0 };
            DWORD read2 = 0;
            while (ReadFile(hRead2, buffer2, sizeof(buffer2) - 1, &read2, NULL) && read2 > 0)
            {
                buffer2[read2] = 0;
                output2 += CString(buffer2);
            }
            WaitForSingleObject(pi2.hProcess, INFINITE);
            CloseHandle(pi2.hProcess);
            CloseHandle(pi2.hThread);
            CString lower2 = output2;
            lower2.MakeLower();
            if (output2.Find(_T("->")) != -1 && output2.Find(_T("%")) != -1)
                isPacked = true;
            if (lower2.Find(_T("notpackedexception")) != -1)
                isNotPacked = true;
        }
        CloseHandle(hRead2);
        BOOL wantDecompress = IsDlgButtonChecked(IDC_RADIO_DECOMPRESS);
        BOOL wantCompress = IsDlgButtonChecked(IDC_RADIO_COMPRESS);
        // 检查是否成功
        if (wantCompress && !isPacked)
        {
            PostMessage(WM_USER + 11, 0, 0); // 压缩失败
        }
        else if (wantDecompress && !isNotPacked)
        {
            PostMessage(WM_USER + 11, 1, 0); // 解压失败
        }
        else
        {
            // 成功后自动切换操作类型
            if (isPacked)
                PostMessage(WM_USER + 10, 1, 0); // 1=解压
            else if (isNotPacked)
                PostMessage(WM_USER + 10, 0, 0); // 0=压缩
        }
        EnableControls(TRUE);
    }).detach();
}

LRESULT CUPXShellerDlg::OnUpdateProgress(WPARAM wParam, LPARAM)
{
    //m_progress.SetPos((int)wParam);
    return 0;
}
LRESULT CUPXShellerDlg::OnUpdateOutput(WPARAM wParam, LPARAM)
{
    CString* pStr = (CString*)wParam;
    m_editOutputInfo.SetWindowText(*pStr);
    delete pStr;
    return 0;
}

// 新增消息处理，提示失败并切换操作类型
LRESULT CUPXShellerDlg::OnOpFailed(WPARAM wParam, LPARAM)
{
    if (wParam == 0)
        AfxMessageBox(_T("压缩失败，文件未被压缩！"));
    else
        AfxMessageBox(_T("解压失败，文件仍为压缩状态！"));
    // 自动切换操作类型
    if (wParam == 0)
        CheckRadioButton(IDC_RADIO_COMPRESS, IDC_RADIO_LICENSE, IDC_RADIO_COMPRESS);
    else
        CheckRadioButton(IDC_RADIO_COMPRESS, IDC_RADIO_LICENSE, IDC_RADIO_DECOMPRESS);
    UpdateCmdPreview();
    return 0;
}

// 检测EXE文件架构
int CUPXShellerDlg::DetectExeArchitecture(const CString& exePath)
{
    CFile file;
    if (!file.Open(exePath, CFile::modeRead | CFile::typeBinary | CFile::shareDenyWrite))
    {
        return -1; // 无法打开文件
    }

    try
    {
        // 读取DOS头
        WORD dosSignature;
        file.Read(&dosSignature, sizeof(dosSignature));
        if (dosSignature != 0x5A4D) // "MZ" 
        {
            file.Close();
            return -1; // 不是有效的PE文件
        }

        // 跳转到PE头位置
        file.Seek(0x3C, CFile::begin); // e_lfanew字段位置
        DWORD peHeaderOffset;
        file.Read(&peHeaderOffset, sizeof(peHeaderOffset));

        // 跳转到PE头
        file.Seek(peHeaderOffset, CFile::begin);

        // 读取PE签名
        DWORD peSignature;
        file.Read(&peSignature, sizeof(peSignature));
        if (peSignature != 0x00004550) // "PE\0\0"
        {
            file.Close();
            return -1; // 不是有效的PE文件
        }

        // 读取Machine字段（PE头的前2字节）
        WORD machine;
        file.Read(&machine, sizeof(machine));

        file.Close();

        // 判断架构
        if (machine == 0x8664) // IMAGE_FILE_MACHINE_AMD64
            return 64; // x64
        else if (machine == 0x014C) // IMAGE_FILE_MACHINE_I386
            return 32; // x86
        else
            return 0; // 其他架构
    }
    catch (CException* e)
    {
        e->Delete();
        file.Close();
        return -1; // 错误
    }
}

void CUPXShellerDlg::OnDropFiles(HDROP hDropInfo)
{
    TCHAR szFile[MAX_PATH] = {0};
    if (DragQueryFile(hDropInfo, 0, szFile, MAX_PATH)) {
        m_inputFile = szFile;
        m_editInput.SetWindowText(m_inputFile);
        
        // 检测EXE架构并自动选择合适的UPX版本
        int arch = DetectExeArchitecture(m_inputFile);
        if (arch == 64) {
            // 选择64位UPX版本
            SelectUPXVersionForArchitecture(64);
        }
        else if (arch == 32) {
            // 选择32位UPX版本
            SelectUPXVersionForArchitecture(32);
        }
        
        // 自动执行列出命令（与OnBnClickedBtnBrowseInput一致）
        CString cmd;
        cmd.Format(_T("\"%s\" -l \"%s\""), m_upxPath, m_inputFile);
        m_editCmdPreview.SetWindowText(cmd);
        m_editOutputInfo.SetWindowText(_T("正在获取文件信息..."));
        EnableControls(FALSE);
        std::thread([=]() {
            SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
            HANDLE hRead, hWrite;
            CreatePipe(&hRead, &hWrite, &sa, 0);
            STARTUPINFO si = { sizeof(STARTUPINFO) };
            si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
            si.hStdOutput = hWrite;
            si.hStdError = hWrite;
            si.wShowWindow = SW_HIDE;
            PROCESS_INFORMATION pi;
            CStringA cmdA(cmd);
            BOOL ret = CreateProcessA(NULL, (LPSTR)(LPCSTR)cmdA, NULL, NULL, TRUE, 0, NULL, NULL, (LPSTARTUPINFOA)&si, &pi);
            CloseHandle(hWrite);
            CString output;
            bool isPacked = false, isNotPacked = false;
            if (!ret)
            {
                output = _T("列出文件信息失败！");
            }
            else
            {
                char buffer[4096] = { 0 };
                DWORD read = 0;
                while (ReadFile(hRead, buffer, sizeof(buffer) - 1, &read, NULL) && read > 0)
                {
                    buffer[read] = 0;
                    output += CString(buffer);
                }
                WaitForSingleObject(pi.hProcess, INFINITE);
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
                // 检查是否已压缩
                CString lower = output;
                lower.MakeLower();
                if (output.Find(_T("->")) != -1 && output.Find(_T("%")) != -1)
                    isPacked = true;
                if (lower.Find(_T("notpackedexception")) != -1)
                    isNotPacked = true;
            }
            CloseHandle(hRead);
            // 自动切换操作类型
            if (isPacked)
                PostMessage(WM_USER + 10, 1, 0); // 1=解压
            else if (isNotPacked)
                PostMessage(WM_USER + 10, 0, 0); // 0=压缩
            PostMessage(WM_USER + 3, (WPARAM)new CString(output), 0);
            EnableControls(TRUE);
        }).detach();
    }
    DragFinish(hDropInfo);
}

// 选择适合架构的UPX版本
void CUPXShellerDlg::SelectUPXVersionForArchitecture(int arch)
{
    CString targetSuffix = (arch == 64) ? _T("win64") : _T("win32");
    int count = m_comboUPXVersion.GetCount();
    int bestMatchIndex = -1;
    CString bestVersion = _T("");
    
    // 查找匹配架构的最新版本
    for (int i = 0; i < count; i++)
    {
        CString versionName;
        m_comboUPXVersion.GetLBText(i, versionName);
        
        // 检查是否匹配架构
        if (versionName.MakeLower().Find(targetSuffix) != -1)
        {
            // 提取版本号进行比较
            // 假设格式为 upx-x.x.x-platform
            int dashPos = versionName.Find(_T('-'), 4); // 从"upx-"之后开始找
            if (dashPos != -1)
            {
                int secondDash = versionName.Find(_T('-'), dashPos );
                if (secondDash != -1)
                {
                    CString versionPart = versionName.Mid(dashPos + 1, secondDash - dashPos - 1);
                    
                    // 比较版本号，选择最新的
                    if (bestVersion.IsEmpty() || CompareVersionStrings(versionPart, bestVersion) > 0)
                    {
                        bestVersion = versionPart;
                        bestMatchIndex = i;
                    }
                }
            }
        }
    }
    
    // 如果找到了匹配架构的版本，选择它
    if (bestMatchIndex != -1)
    {
        m_comboUPXVersion.SetCurSel(bestMatchIndex);
        m_upxPath = GetSelectedUPXPath();
        UpdateCmdPreview();
    }
}

// 比较版本字符串
int CUPXShellerDlg::CompareVersionStrings(const CString& version1, const CString& version2)
{
    // 简单的版本比较，例如 "5.0.1" 与 "5.0.2"
    CString v1 = version1;
    CString v2 = version2;
    
    // 分割版本号
    int pos1 = v1.Find(_T('.'));
    int pos2 = v2.Find(_T('.'));
    
    if (pos1 == -1 || pos2 == -1) {
        // 如果没有点号，直接比较字符串
        return v1.Compare(v2);
    }
    
    // 比较主版本号
    CString main1 = v1.Left(pos1);
    CString main2 = v2.Left(pos2);
    int mainVer1 = _ttoi(main1);
    int mainVer2 = _ttoi(main2);
    
    if (mainVer1 != mainVer2) {
        return mainVer1 - mainVer2;
    }
    
    // 比较次版本号
    CString sub1 = v1.Mid(pos1 + 1);
    CString sub2 = v2.Mid(pos2 + 1);
    pos1 = sub1.Find(_T('.'));
    pos2 = sub2.Find(_T('.'));
    
    if (pos1 == -1 || pos2 == -1) {
        int subVer1 = _ttoi(sub1);
        int subVer2 = _ttoi(sub2);
        return subVer1 - subVer2;
    }
    
    CString minor1 = sub1.Left(pos1);
    CString minor2 = sub2.Left(pos2);
    int minorVer1 = _ttoi(minor1);
    int minorVer2 = _ttoi(minor2);
    
    if (minorVer1 != minorVer2) {
        return minorVer1 - minorVer2;
    }
    
    // 比较修订版本号
    CString patch1 = sub1.Mid(pos1 + 1);
    CString patch2 = sub2.Mid(pos2 + 1);
    int patchVer1 = _ttoi(patch1);
    int patchVer2 = _ttoi(patch2);
    
    return patchVer1 - patchVer2;
}

// 退出按钮事件处理
void CUPXShellerDlg::OnBnClickedBtnExit()
{
    OnCancel(); // 调用基类的OnCancel方法关闭对话框
}


