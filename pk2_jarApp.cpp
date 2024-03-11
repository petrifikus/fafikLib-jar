/***************************************************************
 * Name:      pk2_jarApp.cpp
 * Purpose:   Code for Application Class
 * Author:    petrifikus ()
 * Created:   2024-03-11
 * Copyright: petrifikus ()
 * License:
 **************************************************************/

#include "pk2_jarApp.h"

//(*AppHeaders
#include "pk2_jarMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(pk2_jarApp);

bool pk2_jarApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    pk2_jarFrame* Frame = new pk2_jarFrame(0);
    Frame->Show();
    SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}
