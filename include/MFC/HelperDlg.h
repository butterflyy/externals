#pragma once

#include <afxwin.h>
#include <common\utils.h>

class HelperDlg{
protected:
	HelperDlg(CDialog* dlg, bool en= false):
		m_dlg(dlg),
		m_en(en){
	}

	void ShowMessage(const std::string& msg){
		if (m_en){
			MessageBoxA(m_dlg->GetSafeHwnd(), msg.c_str(), "message", MB_OK); 
		}
		else{
			MessageBoxA(m_dlg->GetSafeHwnd(), msg.c_str(), "ÌבÊ¾", MB_OK);
		}
	}

	void SetDlgText(int id, const std::string& str){
		SetDlgItemTextA(m_dlg->GetSafeHwnd(), id, str.c_str());
	}

	std::string GetDlgText(int id){
		static char buff[1024] = { 0 };
		GetDlgItemTextA(m_dlg->GetSafeHwnd(), id, buff, 1024);
		return buff;
	}

	void SetDlgInt(int id, int value){
		return SetDlgText(id, utils::IntToStr(value));
	}

	int GetDlgInt(int id){
		return utils::StrToInt(GetDlgText(id));
	}

protected:
	CDialog* m_dlg;
	bool m_en;
};