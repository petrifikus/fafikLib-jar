/***************************************************************
 * Name:      pk2_jarMain.h
 * Purpose:   Defines Application Frame
 * Author:    petrifikus ()
 * Created:   2024-03-11
 * Copyright: petrifikus ()
 * License:
 **************************************************************/

#ifndef PK2_JARMAIN_H
#define PK2_JARMAIN_H

 //(*Headers(pk2_jarFrame)
#include <wx/treectrl.h>
#include <wx/notebook.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/menu.h>
#include <wx/textctrl.h>
#include <wx/statline.h>
#include <wx/panel.h>
#include <wx/bmpbuttn.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/statusbr.h>
//*)

#include "include/fafikLib/ArchiveFile.h"

class pk2_jarFrame : public wxFrame
{
public:

	pk2_jarFrame(wxWindow* parent, wxWindowID id = -1);
	virtual ~pk2_jarFrame();

private:
	 //main archive
	fafik77::ArchiveFile Archive;

	//(*Handlers(pk2_jarFrame)
	 //exit was pressed
	void OnQuit(wxCommandEvent& event);
	 //show 
	void OnAbout(wxCommandEvent& event);
	 //show context menu for treeCtrl
	void OnTreeContextMenu(wxTreeEvent& event);
	 //show open file dialog
	void OnButtonOpenFile(wxCommandEvent& event);
	 //open archive
	void OnButtonOpen(wxCommandEvent& event);
	 //save archive
	void OnButtonSave(wxCommandEvent& event);
	 //text changed
	void OnTextCtrlOpenText(wxCommandEvent& event);
	 //key was pressed
	void OnTreeKeyDown(wxTreeEvent& event);
	 //item from context menu was pressed
	void OnTreeContextMenuItem(wxCommandEvent& event);
	//*)

	//(*Identifiers(pk2_jarFrame)
	static const long ID_STATICTEXT1;
	static const long ID_TEXTCTRL1;
	static const long ID_BITMAPBUTTON1;
	static const long ID_BUTTON1;
	static const long ID_STATICLINE1;
	static const long ID_BUTTON2;
	static const long ID_PANEL1;
	static const long ID_TREECTRL1;
	static const long ID_PANEL2;
	static const long ID_NOTEBOOK1;
	static const long idMenuOpen;
	static const long idMenuSave;
	static const long idMenuQuit;
	static const long idMenuAbout;
	static const long ID_STATUSBAR1;
	static const long idMenuCInsert;
	static const long idMenuCRemove;
	static const long idMenuCappend;
	static const long idMenuCpop;
	static const long idMenuCextract;
	//*)

	//(*Declarations(pk2_jarFrame)
	wxMenu MenuPopup;
	wxNotebook* Notebook1;
	wxBitmapButton* BitmapButtonOpen;
	wxPanel* Panel1;
	wxFlexGridSizer* FlexGridSizer2;
	wxStaticText* StaticText1;
	wxTextCtrl* TextCtrlOpen;
	wxButton* ButtonSave;
	wxStatusBar* StatusBar1;
	wxStaticLine* StaticLine1;
	wxTreeCtrl* TreeCtrl1;
	wxPanel* Panel2;
	wxFlexGridSizer* FlexGridSizer1;
	wxButton* ButtonOpen;
	//*)

	DECLARE_EVENT_TABLE()
};

#endif // PK2_JARMAIN_H
