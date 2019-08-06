#pragma once

#include <afxwin.h>
#include <common\utils.h>

#define SET_BTST(shadeButtons, buttonSize, bkColor) \
for (int i = 0; i < buttonSize; i++){ \
	shadeButtons[i].SetShade(CShadeButtonST::SHS_HSHADE, 4, 20, 15, bkColor); \
}\

#define SET_DEFAULT_BTST(shadeButtons, buttonSize) SET_BTST(shadeButtons, buttonSize, RGB(91, 130, 180))


class HelperDlg{
protected:
	HelperDlg(CDialog* dlg, 
		COLORREF bkColor = RGB(255, 255, 255),
		int bts = 0,
		bool en= false
		):
		m_dlg(dlg),
		m_en(en),
		m_bkColor(bkColor){
		m_brush.CreateSolidBrush(bkColor);
	}

	void CreatFont(int width){
		m_font.CreateFontW(width, 0, 0, 0, FW_HEAVY, FALSE, FALSE, 0, ANSI_CHARSET,OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Î¢ÈíÑÅºÚ"));
	}

	void ShowMessage(const std::string& msg){
		if (m_en){
			MessageBoxA(m_dlg->GetSafeHwnd(), msg.c_str(), "message", MB_OK); 
		}
		else{
			MessageBoxA(m_dlg->GetSafeHwnd(), msg.c_str(), "ÌáÊ¾", MB_OK);
		}
	}

	void SetDlgText(int id, const std::string& str){
		SetDlgItemTextA(m_dlg->GetSafeHwnd(), id, str.c_str());
	}

	std::string GetDlgText(int id){
		static char buff[1024] = { 0 };
		GetDlgItemTextA(m_dlg->GetSafeHwnd(), id, buff, 1024);
		return utils::Trim(buff);
	}

	void SetDlgInt(int id, int value){
		return SetDlgText(id, utils::Itos(value));
	}

	int GetDlgInt(int id){
		return utils::Stoi(GetDlgText(id));
	}

	void SetShow(int id, bool show){
		::ShowWindow(::GetDlgItem(m_dlg->GetSafeHwnd(), id), show);
	}

	bool IsCheck(int id){
		return ((CButton*)m_dlg->GetDlgItem(id))->GetCheck();
	}

	void SetCheck(int id, bool checked){
		((CButton*)m_dlg->GetDlgItem(id))->SetCheck(checked);
	}

	//interface
	HBRUSH SetCtlColor(CDC* pDC){
		pDC->SetBkColor(m_bkColor);
		return m_brush;
	}
protected:
	CDialog* m_dlg;
	bool m_en;
	CBrush m_brush;
	CFont m_font;
	COLORREF m_bkColor;
};
