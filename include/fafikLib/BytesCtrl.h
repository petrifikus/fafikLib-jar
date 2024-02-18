/** \brief A simple spin ctrl that allows recalculations for Bytes(B, KB, MB, GB, TB)
 *
 */
#ifndef BYTESCTRL_H_INCLUDED
#define BYTESCTRL_H_INCLUDED
//based on template from	https://forums.wxwidgets.org/viewtopic.php?t=29549
//made by fafik77,	included in fafikLib,	useful for specifying file sizes

#ifndef WX_PRECOMP
	//(*HeadersPCH(BytesCtrl)
	#include <wx/sizer.h>
	#include <wx/stattext.h>
	#include <wx/panel.h>
	//*)
#endif


#include "LoadCfg_call.h"
#include "wxStringInputStream.h"


DECLARE_EVENT_TYPE(wxEVT_IPCTRL_CHANGED, wxID_ANY);

class wxTextCtrl_num : public wxTextCtrl
{
 public:
	wxTextCtrl_num(wxWindow *parent, wxWindowID id,
		const wxString& value= wxEmptyString,
		const wxPoint& pos= wxDefaultPosition,
		const wxSize& size= wxDefaultSize,
		long style= 0,
		const wxValidator& validator= wxDefaultValidator,
		const wxString& name= wxTextCtrlNameStr
	);
	wxTextCtrl_num() : wxTextCtrl(){}

	virtual ~wxTextCtrl_num(){}

	inline long long GetNumber() const;
	inline wxString GetNumberStrRepr() const;

	inline void SetNumber(wxLongLong val, bool silent=false);
	void SetNumber(long long val, bool silent=false);
	wxTextCtrl_num& operator =(const long long& ll){
		SetNumber(ll);
		return *this;
	}
		///silent refresh missing "0"
	void RefreshNumber();
};

/** \brief A simple spin ctrl that allows recalculations for Bytes(B, KB, MB, GB, TB)
 *
 */
class BytesCtrl : public wxPanel, public LoadCfg_call
{
 public:
	BytesCtrl(wxWindow *parent, wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
	virtual ~BytesCtrl();

 /// [ fafik77
 protected:
	wxTextCtrl_num* m_edit[5] ={};
	wxStaticText* m_text[5] ={};
 public:
 	enum BytesShow{
		show_B,
		show_KB,
		show_MB,
		show_GB,
		show_TB,
	};
	enum saveLoadElsEnum
	{
		saveLoad_values= 1,
		saveLoad_preset= 2,
		saveLoad_range= 4,
		saveLoad_all= saveLoad_values | saveLoad_preset | saveLoad_range,
	};
		///control which elements are to be saved & loaded
	BYTE saveLoadEls= saveLoad_all;

	int LoadCfg( fafikLib_DB::itemEntry &item ) override;
	int SaveCfg( fafikLib_DB::itemEntry &item ) override;


		///show elements 4 TB,3 GB,2 MB,1 KB,0 B
	int Bytes_ShowInRange(BYTE showHigh=3, BYTE showLow=1);
		///show elements 4 TB,3 GB,2 MB,1 KB,0 B
	int Bytes_ShowUpTo( BYTE showLowToHigh=1 ){
		return Bytes_ShowInRange( 4, showLowToHigh );
	}
		///preset 0:(KB = 1000 Bytes), 1:(KB = 1024 Bytes), 2:(KiB = 1024 Bytes).. showLogic shows this Description
	void Bytes_PresetLoad(BYTE preset=1);
	inline BYTE Bytes_PresetIs() const{ return priv_preset; }
		///show logic or not
	inline void LogicShow(bool show= true){ Text_showLogic->Show( show ); bSizer1Main->Fit(this); }
	inline bool LogicIsShown() const { return Text_showLogic->IsShown(); }

		///if config was not loaded those values will be used
	void Bytes_defaultValues( const wxArrayInt &vals, bool onlyInVisibleRange= false );

	//Bytes_getAs2pow(  );
		///@param B: getFromVisible: 1 shown, 2 hidden, 3 both
		///get info from control translated into amount-Bytes
	long long Bytes_getInBytes( BYTE getFromVisible=3 );


	wxLongLong values_max[5]= {1024,1024,1024,1024,1024,};
		///display names based on preset
	wxString values_names[5]= {"TB","GB","MB","KB","B"};

 protected:
		///logic preset
	BYTE priv_preset= 0;
		///high
	BYTE priv_Bytes_h= 0;
		///low
	BYTE priv_Bytes_l= 0;

	bool updateUp( const size_t& pos, const wxLongLong& valueChange, const bool setOnly= false );


	wxBoxSizer* bSizer1= nullptr;
	wxBoxSizer* bSizer1Main= nullptr;
	wxBoxSizer* bSizers[5]= {};

 /// ]end
 protected:
	void CreateGui();
	wxTextCtrl_num* CreateTextCtrl();
	wxStaticText* CreateDot(size_t number);
	wxStaticText* Text_showLogic= nullptr;

	virtual void OnChar(wxKeyEvent& event);
	virtual void OnTextUpdated(wxCommandEvent& event);
};


#endif // BYTESCTRL_H_INCLUDED
