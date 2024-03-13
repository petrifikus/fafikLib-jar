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

//(*InternalHeaders(pk2_jarFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)


#include "include/fafikLib/fafikLib_DB.h"

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
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
const long pk2_jarFrame::ID_PANEL1 = wxNewId();
const long pk2_jarFrame::ID_NOTEBOOK1 = wxNewId();
const long pk2_jarFrame::idMenuQuit = wxNewId();
const long pk2_jarFrame::idMenuAbout = wxNewId();
const long pk2_jarFrame::ID_STATUSBAR1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(pk2_jarFrame,wxFrame)
    //(*EventTable(pk2_jarFrame)
    //*)
END_EVENT_TABLE()

pk2_jarFrame::pk2_jarFrame(wxWindow* parent,wxWindowID id)
{
	ItemEntry tempItem("jar test build");
	wxLogMessage("tempItem: %s", tempItem.value);


    //(*Initialize(pk2_jarFrame)
    wxBoxSizer* BoxSizer4;
    wxMenuItem* MenuItem2;
    wxMenuItem* MenuItem1;
    wxBoxSizer* BoxSizer2;
    wxMenu* Menu1;
    wxBoxSizer* BoxSizer1;
    wxMenuBar* MenuBar1;
    wxFlexGridSizer* FlexGridSizer1;
    wxBoxSizer* BoxSizer3;
    wxMenu* Menu2;

    Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
    Notebook1 = new wxNotebook(this, ID_NOTEBOOK1, wxDefaultPosition, wxDefaultSize, 0, _T("ID_NOTEBOOK1"));
    Panel1 = new wxPanel(Notebook1, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    FlexGridSizer1 = new wxFlexGridSizer(0, 2, 0, 0);
    FlexGridSizer1->AddGrowableCol(0);
    BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    FlexGridSizer1->Add(BoxSizer1, 1, wxALL, 5);
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    FlexGridSizer1->Add(BoxSizer2, 1, wxALL, 5);
    BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    FlexGridSizer1->Add(BoxSizer3, 1, wxALL, 5);
    BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    FlexGridSizer1->Add(BoxSizer4, 1, wxALL, 5);
    Panel1->SetSizer(FlexGridSizer1);
    FlexGridSizer1->Fit(Panel1);
    FlexGridSizer1->SetSizeHints(Panel1);
    Notebook1->AddPage(Panel1, _("Page name"), false);
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
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
    StatusBar1->SetFieldsCount(1,__wxStatusBarWidths_1);
    StatusBar1->SetStatusStyles(1,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar1);

    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&pk2_jarFrame::OnQuit);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&pk2_jarFrame::OnAbout);
    //*)
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
