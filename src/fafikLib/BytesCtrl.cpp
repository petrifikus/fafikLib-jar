#include "BytesCtrl.h"
#include <wx/regex.h>
#include <wx/msgdlg.h>

DEFINE_EVENT_TYPE(wxEVT_IPCTRL_CHANGED);


BytesCtrl::BytesCtrl(wxWindow *parent, wxWindowID id,const wxPoint& pos,const wxSize& size)
: wxPanel(parent, id, pos, size, wxSUNKEN_BORDER|wxTAB_TRAVERSAL)
{
	CreateGui();
	Connect( wxEVT_CHAR, wxKeyEventHandler( BytesCtrl::OnChar ) );
	for(size_t temp_u=0; temp_u<5; ++temp_u){
		m_edit[temp_u]->Connect( wxEVT_CHAR, wxKeyEventHandler( BytesCtrl::OnChar ), NULL, this );
		m_edit[temp_u]->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( BytesCtrl::OnTextUpdated ), NULL, this );
	}

	Bytes_ShowInRange();
	Bytes_PresetLoad();
}
void BytesCtrl::CreateGui()
{
	SetBackgroundColour(*wxWHITE);
	bSizer1Main= new wxBoxSizer(wxVERTICAL);
	bSizer1= new wxBoxSizer(wxHORIZONTAL);

	Text_showLogic= new wxStaticText( this, wxID_ANY, "KB = 1024 Bytes", wxDefaultPosition, wxDefaultSize, 0 );
	Text_showLogic->SetForegroundColour( wxColour(182, 12, 114) );

	for(size_t temp_u=0; temp_u<5; ++temp_u) {
		bSizers[temp_u]= new wxBoxSizer(wxVERTICAL);

		m_text[temp_u]= CreateDot(temp_u);
		bSizers[temp_u]->Add(m_text[temp_u], 0, wxALIGN_CENTER, 0);
		m_edit[temp_u]= CreateTextCtrl();
		bSizers[temp_u]->Add(m_edit[temp_u], 0, 0, 0);
		bSizer1->Add(bSizers[temp_u], 0, 0, 0);
	}
	bSizer1Main->Add(bSizer1);
	bSizer1Main->Add(Text_showLogic, 0, wxALIGN_CENTER, 0);
//	SetSizer(bSizer1);
//	Layout();
//	bSizer1->Fit(this);
	SetSizer(bSizer1Main);
	Layout();
	bSizer1Main->Fit(this);
}

wxTextCtrl_num* BytesCtrl::CreateTextCtrl()
{
	wxTextCtrl_num* txtCtrl= new wxTextCtrl_num(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(GetTextExtent("8").GetWidth()*5, -1), wxTE_CENTRE/*|wxNO_BORDER*/);
	txtCtrl->SetMaxLength(4);
	return txtCtrl;
}
wxStaticText* BytesCtrl::CreateDot(size_t number)
{
	wxStaticText* staticTxt= new wxStaticText( this, wxID_ANY, values_names[number%5], wxDefaultPosition, wxDefaultSize, 0 );
	staticTxt->Wrap( -1 );
	return staticTxt;
}

BytesCtrl::~BytesCtrl()
{
	//(*Destroy(BytesCtrl)
	//*)
}
int BytesCtrl::Bytes_ShowInRange(BYTE showHigh, BYTE showLow)	//, bool resetHidden
{
	int ret_enabled= 0;
	if( showHigh<showLow ) std::swap( showHigh, showLow );
	if( showHigh>4 ) showHigh= 4;
		//save show (from to) for later
	priv_Bytes_h= showHigh;
	priv_Bytes_l= showLow;

	for(size_t temp_show=0; temp_show<5; temp_show++){
		bool showItem=false;
		if( temp_show>= showLow && temp_show<= showHigh ){
			showItem=true;
			++ret_enabled;
		}
		if( temp_show==showHigh ){
				//set highest to be less limited here..
				//to avoid previous value change
			values_max[4-showHigh]= 100000;
		}
		else if( priv_preset==0 ){
			values_max[4-temp_show]= 1000;
		} else {
			values_max[4-temp_show]= 1024;
		}
		bSizers[4-temp_show]->Show( showItem );
	}
	//refresh at end
//	bSizer1->Fit(this);
	bSizer1Main->Fit(this);
	Layout();
//	StaticBoxSizer1->Fit(Panel1);
return ret_enabled;
}

void BytesCtrl::Bytes_PresetLoad(BYTE preset)
{
	if( preset>2 ) preset= 2;
	priv_preset= preset;

	if( preset== 2 ){
		m_text[0]->SetLabel( "TiB" );
		m_text[1]->SetLabel( "GiB" );
		m_text[2]->SetLabel( "MiB" );
		m_text[3]->SetLabel( "KiB" );
		Text_showLogic->SetLabel( "KiB = 1024 Bytes" );
	} else {
		m_text[0]->SetLabel( "TB" );
		m_text[1]->SetLabel( "GB" );
		m_text[2]->SetLabel( "MB" );
		m_text[3]->SetLabel( "KB" );
		if( preset== 1 ) Text_showLogic->SetLabel( "KB = 1024 Bytes" );
		else if( preset== 0 ) Text_showLogic->SetLabel( "KB = 1000 Bytes" );
	}
		//now to refresh it
	Bytes_ShowInRange( priv_Bytes_h, priv_Bytes_l );
}



//bool BytesCtrl::CheckIp(const wxString& ipaddr, wxArrayString& ipParts) const
//{
//	wxRegEx strregex(wxT("([0-9]{1,3})([.])([0-9]{1,3})([.])([0-9]{1,3})([.])([0-9]{1,3})"));
//	if (!strregex.Matches(ipaddr)) {
//		return false;
//	}
//
//	ipParts.clear();
//	ipParts.Add( strregex.GetMatch(ipaddr, 1) );
//	ipParts.Add( strregex.GetMatch(ipaddr, 3) );
//	ipParts.Add( strregex.GetMatch(ipaddr, 5) );
//	ipParts.Add( strregex.GetMatch(ipaddr, 7) );
//
//	unsigned long ip1, ip2, ip3, ip4;
//	ipParts[0].ToULong(&ip1);
//	ipParts[1].ToULong(&ip2);
//	ipParts[2].ToULong(&ip3);
//	ipParts[3].ToULong(&ip4);
//return true;
//}


void BytesCtrl::OnChar(wxKeyEvent& event)
{
	const int keyCode= event.GetKeyCode();

	if(keyCode>= '0' && keyCode<= '9') {
		event.Skip();	//entering a number
		return;
	}
	bool uncontroledAction= false;
	size_t local_l= 4-priv_Bytes_h;
	size_t local_h= 4-priv_Bytes_l;

	const wxObject* evtObject= event.GetEventObject();
	if(keyCode== ',' || keyCode== '.' ) {
		for(size_t temp_u=local_l; temp_u<local_h; ++temp_u) {
			if(evtObject== m_edit[temp_u]) {
				m_edit[temp_u+1]->SetFocus();
				m_edit[temp_u+1]->SetInsertionPoint(0);
				m_edit[temp_u+1]->SetSelection(-1, -1);
				uncontroledAction= true;
			}
		}
	}
	else if(keyCode== WXK_BACK) {
		for(size_t temp_u=local_l+1; temp_u<local_h+1; ++temp_u) {
			if(evtObject== m_edit[temp_u] && m_edit[temp_u]->GetValue().IsEmpty()) {
				m_edit[temp_u-1]->SetFocus();
				m_edit[temp_u-1]->SetInsertionPointEnd();
				uncontroledAction= true;
			}
		}
		event.Skip();
	}
	else if(keyCode== WXK_LEFT ) {
		for(size_t temp_u=local_l+1; temp_u<local_h+1; ++temp_u) {
			if(evtObject== m_edit[temp_u] && m_edit[temp_u]->GetInsertionPoint()== 0 ) {
				m_edit[temp_u-1]->SetFocus();
				m_edit[temp_u-1]->SetInsertionPointEnd();
				m_edit[temp_u-1]->SetSelection(-1, -1);
				uncontroledAction= true;
			}
		}
		event.Skip();
	}
	else if(keyCode== WXK_RIGHT ) {
		for(size_t temp_u=local_l; temp_u<local_h; ++temp_u) {
			if(evtObject== m_edit[temp_u]) {
				long tempSelTo= 0; m_edit[temp_u]->GetSelection(nullptr, &tempSelTo);

				if(m_edit[temp_u]->GetInsertionPoint()== m_edit[temp_u]->GetLastPosition() || tempSelTo>= (long)m_edit[temp_u]->GetValue().Length()) {
					m_edit[temp_u+1]->SetFocus();
					m_edit[temp_u+1]->SetInsertionPoint(0);
					m_edit[temp_u+1]->SetSelection(-1, -1);
					uncontroledAction= true;
				}
			}
		}
		event.Skip();
	}
	else if(keyCode== WXK_UP) {
		for(size_t temp_u=local_l; temp_u<local_h+1; ++temp_u) {
			if(evtObject== m_edit[temp_u] ) {
				updateUp(temp_u, 1);	//will increment
			}
		}
	}
	else if(keyCode== WXK_DOWN) {
		for(size_t temp_u=local_l; temp_u<local_h+1; ++temp_u) {
			if(evtObject== m_edit[temp_u] ) {
				updateUp(temp_u, -1, false);	//will decrement only(wont set to full anything before)
			}
		}
	}
	else if(keyCode== WXK_HOME) {
//		uncontroledAction= true;
		m_edit[local_l]->SetFocus();
		m_edit[local_l]->SetInsertionPoint(0);
	}
	else if(keyCode== WXK_END) {
//		uncontroledAction= true;
		m_edit[local_h]->SetFocus();
		m_edit[local_h]->SetInsertionPointEnd();
	}
	else if(keyCode== WXK_DELETE) {
//		uncontroledAction= true;
		event.Skip();
	}
	else if(keyCode== WXK_TAB) {
//		uncontroledAction= true;
		event.Skip();
	}
	else if( event.GetModifiers() & wxMOD_CONTROL ) {
		event.Skip();
	}

	if(uncontroledAction) {
		for(size_t temp_u=local_l; temp_u<local_h+1; ++temp_u) {
			if(evtObject== m_edit[temp_u] && m_edit[temp_u]->GetNumber()== 0 ) {
			m_edit[temp_u]->RefreshNumber();
			}
		}
	}
}
void wxTextCtrl_num::RefreshNumber()
{
	if( GetNumber()== 0 )
		ChangeValue("0");
}


bool BytesCtrl::updateUp( const size_t& pos, const wxLongLong& valueChange, const bool setOnly )
{
	BYTE local_l= 4-priv_Bytes_h;
	BYTE local_h= 4-priv_Bytes_l;

	if(pos>= local_l && pos<= local_h){}else return false;
	wxLongLong temp_i= m_edit[pos]->GetNumber()+ valueChange;
	wxLongLong temp_next= temp_i/ values_max[pos];
	wxLongLong temp_left= valueChange;
	if(valueChange>=0 ){
		if( !setOnly )
			temp_left= temp_i% values_max[pos];
		m_edit[pos]->SetNumber( temp_left, true );

		if( temp_next>0 && pos>=0 ) {
			updateUp( pos-1, temp_next, setOnly );
		}
	} else {
		temp_left= temp_i;
		if(temp_left< 0){
			temp_left= values_max[pos]+ valueChange;
			if(setOnly) temp_left= 0;
			updateUp(pos-1, -1, true);
		}
		m_edit[pos]->SetNumber( temp_left, true );
	}
	return valueChange!=0;
}
void BytesCtrl::OnTextUpdated(wxCommandEvent& event)
{
	const wxString& str= event.GetString();
	if(str.IsEmpty()) {
		event.Skip();
		return;
	}
	long strII=0; str.ToLong(&strII);
	wxLongLong strI(strII);
	size_t local_l= 4-priv_Bytes_h;
	size_t local_h= 4-priv_Bytes_l;

	bool bSendEvtChanged= false;
	const wxObject* evtObject= event.GetEventObject();
	for(size_t temp_u=local_l; temp_u<local_h+1; ++temp_u) {
		if(evtObject== m_edit[temp_u] ) {
			wxLongLong v_next= strI/ values_max[temp_u];
			wxLongLong v_left= strI% values_max[temp_u];
			if(strI== 0){
				m_edit[temp_u]->ChangeValue("0");
				m_edit[temp_u]->SetInsertionPointEnd();
			} else if( str.StartsWith("0") ) {
				m_edit[temp_u]->SetNumber(strI, true);
				m_edit[temp_u]->SetInsertionPointEnd();
			}
			else if( v_next!= 0 ) {
				m_edit[temp_u]->SetNumber(v_left, true);
				updateUp( temp_u-1, v_next, v_left!=0 );
				size_t tempw= 1;
				if(temp_u==local_h) tempw= 0;
				m_edit[temp_u+tempw]->SetFocus();
				m_edit[temp_u+tempw]->SetSelection(-1, -1);
				bSendEvtChanged= true;
			}
		}
	}

	if(bSendEvtChanged) {
		wxCommandEvent evtChanged(wxEVT_IPCTRL_CHANGED, GetId() );
//		evtChanged.SetString( Bytes_getInBytes(3) );
		evtChanged.SetEventObject(this);
		AddPendingEvent(evtChanged);
	}

	event.Skip();
}


void BytesCtrl::Bytes_defaultValues( const wxArrayInt &vals, bool onlyInVisibleRange )
{
	size_t arrayAt= 0;
	for(size_t temp_u=0; temp_u<5; temp_u++){
		if( onlyInVisibleRange ){
			if( temp_u>= priv_Bytes_l && temp_u<= priv_Bytes_h ){	//in shown
				if( arrayAt< vals.size() )
					m_edit[4-temp_u]->SetNumber( vals.Item( arrayAt++ ) );
				else return;
			}
		} else {
			if( temp_u< vals.size() )
				m_edit[4-temp_u]->SetNumber( vals.Item( temp_u ) );
			else return;
		}
	}
}

long long BytesCtrl::Bytes_getInBytes( BYTE getFromVisible )
{
	long long ret_total= 0;
	long multiplier_2= 1024;
	long long multiplier_curr= 1;
	if( this->priv_preset== 0 )	multiplier_2= 1000;

	for(size_t temp_u=0; temp_u<5; temp_u++){
		long long temp_valToadd= multiplier_curr * m_edit[4-temp_u]->GetNumber();

		if( temp_u>= priv_Bytes_l && temp_u<= priv_Bytes_h ){	//in shown
			if( getFromVisible & 1 ){
				ret_total+= temp_valToadd;
			}
		} else {	//in hidden
			if( getFromVisible & 2 ){
				ret_total+= temp_valToadd;
			}
		}
		multiplier_curr*= multiplier_2;
	}
return ret_total;
}



int BytesCtrl::LoadCfg( fafikLib_DB::itemEntry &item )
{
	fafikLib_DB::itemEntry *IE_BytesUI= nullptr;
	fafikLib_DB::itemEntry *IE_now= nullptr;
	long temp_long= 0;
	bool wasOk= 0;
	IE_BytesUI= item.getOrAdd( "BytesUI", fafikLib_DB::ie_Table, &wasOk );

  if(saveLoadEls & saveLoad_preset){
	IE_now= IE_BytesUI->getOrAdd( "BytesPreset", fafikLib_DB::ie_value, &wasOk );
	if(wasOk){
		wxStringInputStream temp_sis( &IE_now->value );
		temp_sis>> priv_preset;
	}
  }

  if(saveLoadEls & saveLoad_range){
	IE_now= IE_BytesUI->getOrAdd( "BytesRange", fafikLib_DB::ie_value, &wasOk );
	if(wasOk){
		wxStringInputStream temp_sis( &IE_now->value );
		temp_sis>> priv_Bytes_h >> priv_Bytes_l;
	}
		//refresh range, preset will take partial effect
	Bytes_ShowInRange( priv_Bytes_h, priv_Bytes_l );
  }

  if(saveLoadEls & saveLoad_values){
	IE_now= IE_BytesUI->getOrAdd( "BytesValues", fafikLib_DB::ie_value, &wasOk );
	if(wasOk){
		wxStringInputStream temp_sis( &IE_now->value );
		for(size_t temp_u=0; temp_u<5; temp_u++){
			temp_sis>> temp_long;
			m_edit[4-temp_u]->SetNumber( temp_long );
		}
	}
  }
	//refresh all
	Bytes_PresetLoad( priv_preset );

return wasOk;
}
int BytesCtrl::SaveCfg( fafikLib_DB::itemEntry &item )
{
	fafikLib_DB::itemEntry *IE_BytesUI= nullptr;
	fafikLib_DB::itemEntry *IE_now= nullptr;
	IE_BytesUI= item.getOrAdd( "BytesUI", fafikLib_DB::ie_Table );

	IE_now= IE_BytesUI->getOrAdd( "BytesPreset", fafikLib_DB::ie_value );
		if( !(saveLoadEls & saveLoad_preset) ) IE_now->setUnwritable();
		IE_now->value.clear();
		IE_now->value<< long(priv_preset);

	IE_now= IE_BytesUI->getOrAdd( "BytesRange", fafikLib_DB::ie_value );
		if( !(saveLoadEls & saveLoad_range) ) IE_now->setUnwritable();
		IE_now->value.clear();
		IE_now->value<< long(priv_Bytes_h) << " " << long(priv_Bytes_l);

	IE_now= IE_BytesUI->getOrAdd( "BytesValues", fafikLib_DB::ie_value );
		if( !(saveLoadEls & saveLoad_values) ) IE_now->setUnwritable();
		IE_now->value.clear();
		for(size_t temp_u=0; temp_u<5; temp_u++){
			IE_now->value<< m_edit[4-temp_u]->GetNumberStrRepr() << " ";
		}
		IE_now->value.RemoveLast();

	IE_now= IE_BytesUI->getOrAdd( "Bytes_getInBytes", fafikLib_DB::ie_value );
		IE_now->value= wxLongLong( this->Bytes_getInBytes() ).ToString();
		IE_now->setUnwritable();
return 0;
}



wxTextCtrl_num::wxTextCtrl_num(wxWindow *parent, wxWindowID id,
		const wxString& value,
		const wxPoint& pos,
		const wxSize& size,
		long style,
		const wxValidator& validator,
		const wxString& name
	) : wxTextCtrl(parent, id, value, pos, size, style, validator, name)
{}
long long wxTextCtrl_num::GetNumber() const
{
	long long temp_ll= 0;
	this->GetValue().ToLongLong(&temp_ll);
	return temp_ll;
}
wxString wxTextCtrl_num::GetNumberStrRepr() const
{
	wxLongLong temp_ret= GetNumber();
	return temp_ret.ToString();
}

void wxTextCtrl_num::SetNumber(wxLongLong val, bool silent)
{
	if(silent)
		this->ChangeValue( val.ToString() );
	else
		this->SetValue( val.ToString() );
}
inline void wxTextCtrl_num::SetNumber(long long val, bool silent){
	SetNumber( wxLongLong(val), silent );
}





