
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/richtext/richtextctrl.h>

#include <sqlite3.h> 

#include "mainframe.h"
#include "addmodframe.h"

class MineralApp: public wxApp {
    public:
        virtual bool OnInit();
    };

wxIMPLEMENT_APP(MineralApp);

bool MineralApp::OnInit() {
    MainFrame *frame = new MainFrame("MineralApp v" VERSION, wxDefaultPosition, wxSize(800,600) );
    frame->Maximize(true);
    frame->Show(true);
    return true;
}

