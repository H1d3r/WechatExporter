// View.h : interface of the CView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <thread>
#include "Core.h"
#include "LoggerImpl.h"
#include "ShellImpl.h"
#include "ExportNotifierImpl.h"

class CView : public CDialogImpl<CView>, public CDialogResize<CView>
{
private:
	ShellImpl			m_shell;
	LoggerImpl*			m_logger;
	ExportNotifierImpl* m_notifier;
	Exporter* m_exporter;

	
	



public:
	enum { IDD = IDD_WECHATEXPORTER_FORM };

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// m_shell = NULL;
		m_logger = NULL;
		m_notifier = NULL;
		m_exporter = NULL;

		// Init the CDialogResize code
		DlgResize_Init();

		CProgressBarCtrl pbc = GetDlgItem(IDC_PROGRESS);
		pbc.SetRange(0, 100);

		m_notifier = new ExportNotifierImpl(GetDlgItem(IDC_PROGRESS), GetInteractiveCtrls());
		m_logger = new LoggerImpl(GetDlgItem(IDC_LOG));

		::PostMessage(GetDlgItem(IDC_EXPORT), BM_CLICK, 0, 0L);
		
		return TRUE;
	}

	void OnFinalMessage(HWND hWnd)
	{
		if (NULL != m_notifier)
		{
			m_notifier->cancel();
		}
		if (NULL != m_exporter)
		{
			m_exporter->waitForComplition();
			delete m_exporter;
			m_exporter = NULL;
		}
		if (NULL != m_notifier)
		{
			delete m_notifier;
			m_notifier = NULL;
		}
		if (NULL != m_logger)
		{
			delete m_logger;
			m_logger = NULL;
		}
		// override to do something, if needed
	}

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		return CWindow::IsDialogMessage(pMsg);
	}

	BEGIN_MSG_MAP(CView)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		CHAIN_MSG_MAP(CDialogResize<CView>)
		COMMAND_HANDLER(IDC_CHOOSE_BKP, BN_CLICKED, OnBnClickedChooseBkp)
		COMMAND_HANDLER(IDC_CHOOSE_OUTPUT, BN_CLICKED, OnBnClickedChooseOutput)
		COMMAND_HANDLER(IDC_EXPORT, BN_CLICKED, OnBnClickedExport)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(CView)
		DLGRESIZE_CONTROL(IDC_CHOOSE_BKP, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_BACKUP, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_CHOOSE_OUTPUT, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_OUTPUT, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_LOG, DLSZ_SIZE_X | DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDC_PROGRESS, DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_EXPORT, DLSZ_MOVE_X | DLSZ_MOVE_Y)
	END_DLGRESIZE_MAP()


// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	LRESULT OnBnClickedChooseBkp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CFolderDialog folder(NULL, L"Select a dir", BIF_RETURNONLYFSDIRS | BIF_USENEWUI | BIF_NONEWFOLDERBUTTON);
		if (IDOK == folder.DoModal()) {
			// std::wstring dir = folder.m_szFolderPath;
			// OutputDebugString((dir + L"\n").c_str());
		}

		return 0;
	}
	LRESULT OnBnClickedChooseOutput(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CFolderDialog folder(NULL, L"Select a dir", BIF_RETURNONLYFSDIRS | BIF_USENEWUI | BIF_NONEWFOLDERBUTTON);
		if (IDOK == folder.DoModal()) {
			SetDlgItemText(IDC_OUTPUT, folder.m_szFolderPath);
			// std::wstring dir = folder.m_szFolderPath;
			// OutputDebugString((dir + L"\n").c_str());
		}

		return 0;
	}
	LRESULT OnBnClickedExport(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		Run();

		return 0;
	}


	void Run()
	{
		if (NULL != m_exporter)
		{
			return;
		}

		std::string backup = "D:\\Workspace\\iTunes\\MobileSync\\Backup\\11833774f1a5eed6ca84c0270417670f1483deae\\";
		std::string output = "D:\\pngs\\bak\\";

		TCHAR buffer[MAX_PATH] = { 0 };
		DWORD dwRet = 0;
		
		dwRet = GetCurrentDirectory(MAX_PATH, buffer);

		if (dwRet == 0)
		{
			// printf("GetCurrentDirectory failed (%d)\n", GetLastError());
			return;
		}

		// NSString *workDir = [[NSFileManager defaultManager] currentDirectoryPath];

		// workDir = [[NSBundle mainBundle] resourcePath];

		CT2A workDir(buffer, CP_UTF8);

		std::string resDir((LPCSTR)workDir);

#ifndef NDEBUG
		resDir = "D:\\Workspace\\github\\WechatExporter\\WechatExporter\\";
#endif

		m_exporter = new Exporter(resDir, backup, output, &m_shell, m_logger);
		m_exporter->setNotifier(m_notifier);
		if (m_exporter->run())
		{
			DisableInteractiveCtrls();
		}
	}
	// void 

	std::vector<HWND> GetInteractiveCtrls()
	{
		std::vector<HWND> ctrls;
		ctrls.push_back(GetDlgItem(IDC_BACKUP));
		ctrls.push_back(GetDlgItem(IDC_CHOOSE_BKP));
		ctrls.push_back(GetDlgItem(IDC_CHOOSE_OUTPUT));
		ctrls.push_back(GetDlgItem(IDC_EXPORT));

		return ctrls;
	}

	void DisableInteractiveCtrls()
	{
		::EnableWindow(GetDlgItem(IDC_BACKUP), FALSE);
		::EnableWindow(GetDlgItem(IDC_CHOOSE_BKP), FALSE);
		::EnableWindow(GetDlgItem(IDC_CHOOSE_OUTPUT), FALSE);
		::EnableWindow(GetDlgItem(IDC_EXPORT), FALSE);
	}
	

};
