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
#include <wx/notebook.h>
#include <wx/sizer.h>
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/frame.h>
#include <wx/statusbr.h>
//*)

class pk2_jarFrame: public wxFrame
{
    public:

        pk2_jarFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~pk2_jarFrame();

    private:

        //(*Handlers(pk2_jarFrame)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        //*)

        //(*Identifiers(pk2_jarFrame)
        static const long ID_PANEL1;
        static const long ID_NOTEBOOK1;
        static const long idMenuQuit;
        static const long idMenuAbout;
        static const long ID_STATUSBAR1;
        //*)

        //(*Declarations(pk2_jarFrame)
        wxNotebook* Notebook1;
        wxPanel* Panel1;
        wxStatusBar* StatusBar1;
        //*)

        DECLARE_EVENT_TABLE()
};

#endif // PK2_JARMAIN_H
