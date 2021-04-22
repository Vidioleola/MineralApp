
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "mineraldb.hpp"
#include "genreportframe.h"

wxBEGIN_EVENT_TABLE(GenReportFrame, wxFrame)
    EVT_BUTTON(ID_GenReport_Save,   GenReportFrame::OnSave)
    EVT_BUTTON(ID_GenReport_Cancel, GenReportFrame::OnCancel)
wxEND_EVENT_TABLE()

GenReportFrame::GenReportFrame(wxFrame *parent, const wxString& title, sqlite3 *maindb, int uid, std::string db_file_path)
        : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX)) {

    /* Initialize... */
    db = maindb;
    int border=5;
    selected_uid = uid;
    db_path = db_file_path;

    /* Main container */
    wxBoxSizer *WxBoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    this->SetSizer(WxBoxSizer1);
    this->SetAutoLayout(true);
    wxPanel *panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
    WxBoxSizer1->Add(panel,1,wxEXPAND | wxALL,5);
    wxBoxSizer *vsizer = new wxBoxSizer(wxVERTICAL);
	panel->SetSizer(vsizer);
	panel->SetAutoLayout(true);

    /* Radio buttons */
    wxStaticText  *lab1 = new wxStaticText(this, -1, "Do you want to generate a report for the full database or only the selected mineral?");
    rb1a = new wxRadioButton(this, -1, "Full database", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    rb1b = new wxRadioButton(this, -1, "Selected mineral", wxDefaultPosition, wxDefaultSize);
    wxStaticText  *lab2 = new wxStaticText(this, -1, "Do you want to include the images and data?");
    rb2a = new wxRadioButton(this, -1, "Yes, please include the images and data", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    rb2b = new wxRadioButton(this, -1, "No, thanks", wxDefaultPosition, wxDefaultSize);
    wxStaticText  *lab3 = new wxStaticText(this, -1, "Do you prefer a report in HTML or MHT format?");
    rb3a = new wxRadioButton(this, -1, "HTML: images and data will be linked, filesize is small, view and print using any internet browser", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    rb3b = new wxRadioButton(this, -1, "MHT: images and data will be embedded, filesize could be huge, you can open and modify it in Microsoft Word", wxDefaultPosition, wxDefaultSize);
    rb1b->SetValue(true);
    rb2a->SetValue(true);
    rb3b->SetValue(true);
    vsizer->Add(lab1, 0, wxEXPAND|wxALL, border);
    vsizer->Add(rb1a, 0, wxEXPAND|wxALL, border);
    vsizer->Add(rb1b, 0, wxEXPAND|wxALL, border);
    vsizer->Add(lab2, 0, wxEXPAND|wxALL, border);
    vsizer->Add(rb2a, 0, wxEXPAND|wxALL, border);
    vsizer->Add(rb2b, 0, wxEXPAND|wxALL, border);
    vsizer->Add(lab3, 0, wxEXPAND|wxALL, border);
    vsizer->Add(rb3a, 0, wxEXPAND|wxALL, border);
    vsizer->Add(rb3b, 0, wxEXPAND|wxALL, border);

    /* ok cancel buttons */
    wxBoxSizer *hsizer_buttons = new wxBoxSizer(wxHORIZONTAL);
    wxButton *button_save = new wxButton(panel, ID_GenReport_Save, wxString("Save"));
    wxButton *button_cancel = new wxButton(panel, ID_GenReport_Cancel, wxString("Cancel"));
    hsizer_buttons->Add(new wxStaticText(panel, -1, ""), 3, wxALL, border);
    hsizer_buttons->Add(button_cancel, 0, wxALL, border);
    hsizer_buttons->Add(button_save, 0, wxALL, border);
    vsizer->Add(hsizer_buttons, 0, wxEXPAND|wxALL, border);

    /* Fit everything in the panel and add scrollbars */
    GetSizer()->Fit(this);
	GetSizer()->SetSizeHints(this);
}

void GenReportFrame::OnSave(wxCommandEvent& event) {

    bool fulldb = rb1a->GetValue();
    bool include_data = rb2a->GetValue();
    bool html = rb3a->GetValue();
    if (selected_uid<0 && !fulldb) {
        wxLogMessage("Sorry, no mineral was selected. Select one from the left panel.");
        Close(true);
    }
    std::string default_fname;
    std::string default_extension;
    if (html) {
        default_fname = "report.html";
        default_extension = "HTML files (*.html)|*.html";
    } else {
        default_fname = "report.mht";
        default_extension = "MHT files (*.mht)|*.mht";
    }
    wxFileDialog *save_report = new wxFileDialog(this, "Generate Report", wxEmptyString, default_fname, default_extension, wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
    if (save_report->ShowModal() == wxID_CANCEL) {
        Close(true);
    }
    std::string errmsg;
    db_generate_report(db, db_path, save_report->GetPath().ToStdString(), fulldb, include_data, html, selected_uid, &errmsg);
    if (errmsg.size()>0) {
        wxLogMessage(wxString("Writing failed!\n" + errmsg));
    }
    Close(true);
}

void GenReportFrame::OnCancel(wxCommandEvent& event) {
    Close(true);
}

