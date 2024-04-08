/***************************************************************
 * Name:      pk2_jarMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    petrifikus ()
 * Created:   2024-03-11
 * Copyright: petrifikus ()
 * License:
 **************************************************************/

#include "pk2_jarMain.h"
#include <wx/msgdlg.h>
#include <wx/filedlg.h>
#include <wx/log.h>

 //(*InternalHeaders(pk2_jarFrame)
#include <wx/artprov.h>
#include <wx/bitmap.h>
#include <wx/font.h>
#include <wx/intl.h>
#include <wx/image.h>
#include <wx/string.h>
//*)

#include <math.h>
#include <algorithm>

#undef min
#undef max

//helper functions
enum wxbuildinfoformat {
	short_f, long_f
};

wxString wxbuildinfo(wxbuildinfoformat format)
{
	wxString wxbuild(wxVERSION_STRING);

	if (format == long_f)
	{
#if defined(__WXMSW__)
		wxbuild << _T("-Windows");
#elif defined(__UNIX__)
		wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
		wxbuild << _T("-Unicode build");
#else
		wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
	}

	return wxbuild;
}

//(*IdInit(pk2_jarFrame)
const long pk2_jarFrame::ID_STATICTEXT1 = wxNewId();
const long pk2_jarFrame::ID_TEXTCTRL1 = wxNewId();
const long pk2_jarFrame::ID_BITMAPBUTTON1 = wxNewId();
const long pk2_jarFrame::ID_BUTTON1 = wxNewId();
const long pk2_jarFrame::ID_STATICLINE1 = wxNewId();
const long pk2_jarFrame::ID_BUTTON2 = wxNewId();
const long pk2_jarFrame::ID_PANEL1 = wxNewId();
const long pk2_jarFrame::ID_TREECTRL1 = wxNewId();
const long pk2_jarFrame::ID_PANEL2 = wxNewId();
const long pk2_jarFrame::ID_NOTEBOOK1 = wxNewId();
const long pk2_jarFrame::idMenuOpen = wxNewId();
const long pk2_jarFrame::idMenuSave = wxNewId();
const long pk2_jarFrame::idMenuQuit = wxNewId();
const long pk2_jarFrame::idMenuAbout = wxNewId();
const long pk2_jarFrame::ID_STATUSBAR1 = wxNewId();
const long pk2_jarFrame::idMenuCInsert = wxNewId();
const long pk2_jarFrame::idMenuCRemove = wxNewId();
const long pk2_jarFrame::idMenuCappend = wxNewId();
const long pk2_jarFrame::idMenuCpop = wxNewId();
const long pk2_jarFrame::idMenuCextract = wxNewId();
//*)

BEGIN_EVENT_TABLE(pk2_jarFrame, wxFrame)
//(*EventTable(pk2_jarFrame)
//*)
END_EVENT_TABLE()

pk2_jarFrame::pk2_jarFrame(wxWindow* parent, wxWindowID id)
{
	//(*Initialize(pk2_jarFrame)
	wxMenuItem* MenuItem8;
	wxMenuItem* MenuItem7;
	wxMenuItem* MenuItem5;
	wxMenuItem* MenuItem2;
	wxMenuItem* MenuItem1;
	wxMenuItem* MenuItem4;
	wxMenu* Menu1;
	wxMenuItem* MenuItem3;
	wxMenuItem* MenuItem6;
	wxBoxSizer* BoxSizer1;
	wxMenuBar* MenuBar1;
	wxMenu* Menu2;
	wxMenuItem* MenuItem9;

	Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
	SetMinSize(wxSize(400, 600));
	SetBackgroundColour(wxColour(191, 205, 219));
	wxFont thisFont(12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, _T("Arial"), wxFONTENCODING_DEFAULT);
	SetFont(thisFont);
	Notebook1 = new wxNotebook(this, ID_NOTEBOOK1, wxDefaultPosition, wxDefaultSize, 0, _T("ID_NOTEBOOK1"));
	Panel1 = new wxPanel(Notebook1, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer1->AddGrowableCol(0);
	StaticText1 = new wxStaticText(Panel1, ID_STATICTEXT1, _("Open archive file"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer1->Add(StaticText1, 1, wxALL, 5);
	BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
	TextCtrlOpen = new wxTextCtrl(Panel1, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	TextCtrlOpen->SetMaxLength(256);
	BoxSizer1->Add(TextCtrlOpen, 1, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
	BitmapButtonOpen = new wxBitmapButton(Panel1, ID_BITMAPBUTTON1, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FILE_OPEN")), wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON1"));
	BoxSizer1->Add(BitmapButtonOpen, 0, wxTOP | wxBOTTOM | wxRIGHT | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(BoxSizer1, 1, wxALL | wxEXPAND, 0);
	ButtonOpen = new wxButton(Panel1, ID_BUTTON1, _("Open"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	FlexGridSizer1->Add(ButtonOpen, 1, wxALL, 5);
	StaticLine1 = new wxStaticLine(Panel1, ID_STATICLINE1, wxDefaultPosition, wxSize(10, -1), wxLI_HORIZONTAL, _T("ID_STATICLINE1"));
	FlexGridSizer1->Add(StaticLine1, 1, wxALL | wxEXPAND, 1);
	ButtonSave = new wxButton(Panel1, ID_BUTTON2, _("Save"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	FlexGridSizer1->Add(ButtonSave, 1, wxALL, 5);
	Panel1->SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(Panel1);
	FlexGridSizer1->SetSizeHints(Panel1);
	Panel2 = new wxPanel(Notebook1, ID_PANEL2, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL2"));
	FlexGridSizer2 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer2->AddGrowableCol(0);
	FlexGridSizer2->AddGrowableRow(0);
	TreeCtrl1 = new wxTreeCtrl(Panel2, ID_TREECTRL1, wxDefaultPosition, wxDefaultSize, wxTR_HIDE_ROOT | wxTR_DEFAULT_STYLE, wxDefaultValidator, _T("ID_TREECTRL1"));
	wxTreeItemId TreeCtrl1_Item1 = TreeCtrl1->AddRoot(_T("root"));
	wxTreeItemId TreeCtrl1_Item2 = TreeCtrl1->AppendItem(TreeCtrl1_Item1, _T("Tree"));
	wxTreeItemId TreeCtrl1_Item3 = TreeCtrl1->AppendItem(TreeCtrl1_Item1, _T("Files"));
	TreeCtrl1->ScrollTo(TreeCtrl1_Item2);
	TreeCtrl1->SetBackgroundColour(wxColour(191, 205, 219));
	FlexGridSizer2->Add(TreeCtrl1, 9, wxALL | wxEXPAND, 0);
	Panel2->SetSizer(FlexGridSizer2);
	FlexGridSizer2->Fit(Panel2);
	FlexGridSizer2->SetSizeHints(Panel2);
	Notebook1->AddPage(Panel1, _("Jar"), false);
	Notebook1->AddPage(Panel2, _("Archive"), false);
	MenuBar1 = new wxMenuBar();
	Menu1 = new wxMenu();
	MenuItem3 = new wxMenuItem(Menu1, idMenuOpen, _("&Open archive\tCtrl-O"), _("Opens archive file"), wxITEM_NORMAL);
	Menu1->Append(MenuItem3);
	MenuItem4 = new wxMenuItem(Menu1, idMenuSave, _("&Save\tCtrl-S"), _("Saves changes"), wxITEM_NORMAL);
	Menu1->Append(MenuItem4);
	Menu1->AppendSeparator();
	MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("Quit\tAlt-F4"), _("Quit the application"), wxITEM_NORMAL);
	Menu1->Append(MenuItem1);
	MenuBar1->Append(Menu1, _("&File"));
	Menu2 = new wxMenu();
	MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("About\tF1"), _("Show info about this application"), wxITEM_NORMAL);
	Menu2->Append(MenuItem2);
	MenuBar1->Append(Menu2, _("Help"));
	SetMenuBar(MenuBar1);
	StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
	int __wxStatusBarWidths_1[1] = { -1 };
	int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
	StatusBar1->SetFieldsCount(1, __wxStatusBarWidths_1);
	StatusBar1->SetStatusStyles(1, __wxStatusBarStyles_1);
	SetStatusBar(StatusBar1);
	MenuItem5 = new wxMenuItem((&MenuPopup), idMenuCInsert, _("&Insert"), wxEmptyString, wxITEM_NORMAL);
	MenuPopup.Append(MenuItem5);
	MenuItem6 = new wxMenuItem((&MenuPopup), idMenuCRemove, _("&Remove"), wxEmptyString, wxITEM_NORMAL);
	MenuPopup.Append(MenuItem6);
	MenuPopup.AppendSeparator();
	MenuItem7 = new wxMenuItem((&MenuPopup), idMenuCappend, _("&Append"), wxEmptyString, wxITEM_NORMAL);
	MenuPopup.Append(MenuItem7);
	MenuItem8 = new wxMenuItem((&MenuPopup), idMenuCpop, _("&Pop last item"), wxEmptyString, wxITEM_NORMAL);
	MenuPopup.Append(MenuItem8);
	MenuPopup.AppendSeparator();
	MenuItem9 = new wxMenuItem((&MenuPopup), idMenuCextract, _("&Extract"), wxEmptyString, wxITEM_NORMAL);
	MenuPopup.Append(MenuItem9);

	Connect(ID_TEXTCTRL1, wxEVT_COMMAND_TEXT_UPDATED, (wxObjectEventFunction)&pk2_jarFrame::OnTextCtrlOpenText);
	Connect(ID_BITMAPBUTTON1, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&pk2_jarFrame::OnButtonOpenFile);
	Connect(ID_BUTTON1, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&pk2_jarFrame::OnButtonOpen);
	Connect(ID_BUTTON2, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&pk2_jarFrame::OnButtonSave);
	Connect(ID_TREECTRL1, wxEVT_COMMAND_TREE_KEY_DOWN, (wxObjectEventFunction)&pk2_jarFrame::OnTreeKeyDown);
	Connect(ID_TREECTRL1, wxEVT_COMMAND_TREE_ITEM_MENU, (wxObjectEventFunction)&pk2_jarFrame::OnTreeContextMenu);
	Connect(idMenuOpen, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&pk2_jarFrame::OnButtonOpenFile);
	Connect(idMenuSave, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&pk2_jarFrame::OnButtonSave);
	Connect(idMenuQuit, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&pk2_jarFrame::OnQuit);
	Connect(idMenuAbout, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&pk2_jarFrame::OnAbout);
	Connect(idMenuCInsert, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&pk2_jarFrame::OnTreeContextMenuItem);
	Connect(idMenuCRemove, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&pk2_jarFrame::OnTreeContextMenuItem);
	Connect(idMenuCappend, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&pk2_jarFrame::OnTreeContextMenuItem);
	Connect(idMenuCpop, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&pk2_jarFrame::OnTreeContextMenuItem);
	Connect(idMenuCextract, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&pk2_jarFrame::OnTreeContextMenuItem);
	//*)

	this->SetMinSize(wxSize(
		std::max(this->GetMinSize().GetWidth(), this->GetSize().GetWidth()),
		std::max(this->GetMinSize().GetHeight(), this->GetSize().GetHeight())
	));
}

pk2_jarFrame::~pk2_jarFrame()
{
	//(*Destroy(pk2_jarFrame)
	//*)
}

void pk2_jarFrame::OnQuit(wxCommandEvent& event)
{
	Close();
}

void pk2_jarFrame::OnAbout(wxCommandEvent& event)
{
	wxString msg = wxbuildinfo(long_f);
	wxMessageBox(msg, _("Welcome to..."));
}

void pk2_jarFrame::OnButtonOpenFile(wxCommandEvent& event)
{
	//show open file dialog
	wxFileDialog openArFile(this, "Open Archive", wxGetCwd(), wxEmptyString, "*.arMdfc", wxFD_OPEN);
	if (openArFile.ShowModal() == wxID_OK) {
		TextCtrlOpen->ChangeValue(openArFile.GetPath());
	}
}

void pk2_jarFrame::OnButtonOpen(wxCommandEvent& event)
{
	//opens archive file of set name
	wxString fileName = TextCtrlOpen->GetValue();
	if (wxDirExists(fileName)) {
		wxLogError("\"%s\" is a directory, not a file!", fileName);
		return;
	}
	Archive.Open(fileName);
}

void pk2_jarFrame::OnButtonSave(wxCommandEvent& event)
{
	//saves archive file of set name
	Archive.Save();
}



void pk2_jarFrame::OnTreeContextMenu(wxTreeEvent& event)
{
	PopupMenu(&MenuPopup);
}

void pk2_jarFrame::OnTextCtrlOpenText(wxCommandEvent& event)
{
	//extract "path" --> path
	if (event.GetString().StartsWith("\"") && event.GetString().EndsWith("\"")) {
		wxString modStr = event.GetString();
		modStr.RemoveLast();
		modStr.Remove(0, 1);
		TextCtrlOpen->ChangeValue(modStr);
		TextCtrlOpen->SetInsertionPointEnd();
	}
}

void pk2_jarFrame::OnTreeKeyDown(wxTreeEvent& event)
{
	//event.GetKeyCode()
}

void pk2_jarFrame::OnTreeContextMenuItem(wxCommandEvent& event)
{
	//on of context menu items was used
}





