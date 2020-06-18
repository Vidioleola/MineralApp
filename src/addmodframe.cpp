
#include <vector>

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/richtext/richtextctrl.h>

#include <sqlite3.h> 

#include "mainframe.h"
#include "addmodframe.h"
#include "addtodb.hpp"

wxBEGIN_EVENT_TABLE(AddModFrame, wxFrame)
    EVT_BUTTON(ID_AddMod_Save, AddModFrame::OnSave)
    EVT_BUTTON(ID_AddMod_Cancel, AddModFrame::OnCancel)
wxEND_EVENT_TABLE()


AddModFrame::AddModFrame(MainFrame *parent, const wxString& title, sqlite3 *maindb, int uid)
        : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(1000,600), wxDEFAULT_FRAME_STYLE) {

    /* Initialize... */
    db = maindb;
    main_window = parent;
    int border=1;

    /* Main container */
    wxScrolledWindow *panel = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, title);
    wxBoxSizer *vsizer = new wxBoxSizer(wxVERTICAL);

    /* Name and MinID */
    entry_name = new wxTextCtrl(panel, -1);
    entry_minid = new wxTextCtrl(panel, -1);
    wxBoxSizer *hsizer_name = new wxBoxSizer(wxHORIZONTAL);
    hsizer_name->Add(new wxStaticText(panel, -1, "Name:"), 1, wxALIGN_CENTER|wxALL, border);
    hsizer_name->Add(entry_name, 2, wxALIGN_CENTER|wxALL, border);
    hsizer_name->Add(new wxStaticText(panel, -1, "ID:"), 1, wxALIGN_CENTER|wxALL, border);
    hsizer_name->Add(entry_minid, 1, wxALIGN_CENTER|wxALL, border);
    vsizer->Add(hsizer_name, 0, wxEXPAND|wxALL, border);

    /* Locality */
    entry_locality = new wxTextCtrl(panel, -1);
    entry_locid = new wxTextCtrl(panel, -1);
    wxBoxSizer *hsizer_locality = new wxBoxSizer(wxHORIZONTAL);
    hsizer_locality->Add(new wxStaticText(panel, -1, "Locality:"), 1, wxALIGN_CENTER|wxALL, border);
    hsizer_locality->Add(entry_locality, 2, wxALIGN_CENTER|wxALL, border);
    hsizer_locality->Add(new wxStaticText(panel, -1, "ID MINDAT:"), 1, wxALIGN_CENTER|wxALL, border);
    hsizer_locality->Add(entry_locid, 1, wxALIGN_CENTER|wxALL, border);
    vsizer->Add(hsizer_locality, 0, wxEXPAND|wxALL, border);

    /* Weight*/
    entry_weight = new wxTextCtrl(panel, -1);
    wxBoxSizer *hsizer_weight = new wxBoxSizer(wxHORIZONTAL);
    hsizer_weight->Add(new wxStaticText(panel, -1, "Weight:"), 1, wxALIGN_CENTER|wxALL, border);
    hsizer_weight->Add(entry_weight, 4, wxALIGN_CENTER|wxALL, border);
    vsizer->Add(hsizer_weight, 0, wxEXPAND|wxALL, border);

    /* Size */
    entry_size = new wxTextCtrl(panel, -1);
    wxBoxSizer *hsizer_size = new wxBoxSizer(wxHORIZONTAL);
    hsizer_size->Add(new wxStaticText(panel, -1, "Size:"), 1, wxALIGN_CENTER|wxALL, border);
    hsizer_size->Add(entry_size, 4, wxALIGN_CENTER|wxALL, border);
    vsizer->Add(hsizer_size, 0, wxEXPAND|wxALL, border);

    /* Acquisition */
    entry_acquisition = new wxTextCtrl(panel, -1);
    wxBoxSizer *hsizer_acquisition = new wxBoxSizer(wxHORIZONTAL);
    hsizer_acquisition->Add(new wxStaticText(panel, -1, "Acquisition:"), 1, wxALIGN_CENTER|wxALL, border);
    hsizer_acquisition->Add(entry_acquisition, 4, wxALIGN_CENTER|wxALL, border);
    vsizer->Add(hsizer_acquisition, 0, wxEXPAND|wxALL, border);

    /* Collection */
    entry_collection = new wxTextCtrl(panel, -1);
    wxBoxSizer *hsizer_collection = new wxBoxSizer(wxHORIZONTAL);
    hsizer_collection->Add(new wxStaticText(panel, -1, "Collection:"), 1, wxALIGN_CENTER|wxALL, border);
    hsizer_collection->Add(entry_collection, 4, wxALIGN_CENTER|wxALL, border);
    vsizer->Add(hsizer_collection, 0, wxEXPAND|wxALL, border);

    /* Value */
    entry_value = new wxTextCtrl(panel, -1);
    wxBoxSizer *hsizer_value = new wxBoxSizer(wxHORIZONTAL);
    hsizer_value->Add(new wxStaticText(panel, -1, "Value/Price:"), 1, wxALIGN_CENTER|wxALL, border);
    hsizer_value->Add(entry_value, 4, wxALIGN_CENTER|wxALL, border);
    vsizer->Add(hsizer_value, 0, wxEXPAND|wxALL, border);

    /* Species title row */
    wxBoxSizer *hsizer_species_l = new wxBoxSizer(wxHORIZONTAL);
    hsizer_species_l->Add(new wxStaticText(panel, -1, ""), 1, wxALL, border);
    hsizer_species_l->Add(new wxStaticText(panel, -1, "Species 1"), 1, wxALIGN_CENTER|wxALL, border);
    hsizer_species_l->Add(new wxStaticText(panel, -1, "Species 2"), 1, wxALL, border);
    hsizer_species_l->Add(new wxStaticText(panel, -1, "Species 3"), 1, wxALL, border);
    hsizer_species_l->Add(new wxStaticText(panel, -1, "Species 4"), 1, wxALL, border);
    vsizer->Add(hsizer_species_l, 0, wxEXPAND|wxALL, 5);

    /* Species */
    entry_s1_species = new wxTextCtrl(panel, -1);
    entry_s2_species = new wxTextCtrl(panel, -1);
    entry_s3_species = new wxTextCtrl(panel, -1);
    entry_s4_species = new wxTextCtrl(panel, -1);
    wxBoxSizer *hsizer_species = new wxBoxSizer(wxHORIZONTAL);
    hsizer_species->Add(new wxStaticText(panel, -1, "Species:"), 1, wxALIGN_CENTER|wxALL, border);
    hsizer_species->Add(entry_s1_species, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_species->Add(entry_s2_species, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_species->Add(entry_s3_species, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_species->Add(entry_s4_species, 1, wxALIGN_CENTER|wxALL, border);
    vsizer->Add(hsizer_species, 0, wxEXPAND|wxALL, border);
 
    /* Class */
    entry_s1_class = new wxTextCtrl(panel, -1);
    entry_s2_class = new wxTextCtrl(panel, -1);
    entry_s3_class = new wxTextCtrl(panel, -1);
    entry_s4_class = new wxTextCtrl(panel, -1);
    wxBoxSizer *hsizer_class = new wxBoxSizer(wxHORIZONTAL);
    hsizer_class->Add(new wxStaticText(panel, -1, "Class:"), 1, wxALIGN_CENTER|wxALL, border);
    hsizer_class->Add(entry_s1_class, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_class->Add(entry_s2_class, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_class->Add(entry_s3_class, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_class->Add(entry_s4_class, 1, wxALIGN_CENTER|wxALL, border);
    vsizer->Add(hsizer_class, 0, wxEXPAND|wxALL, border);
 
    /* Chemical Formula */
    entry_s1_chemf = new wxTextCtrl(panel, -1);
    entry_s2_chemf = new wxTextCtrl(panel, -1);
    entry_s3_chemf = new wxTextCtrl(panel, -1);
    entry_s4_chemf = new wxTextCtrl(panel, -1);
    wxBoxSizer *hsizer_chemf = new wxBoxSizer(wxHORIZONTAL);
    hsizer_chemf->Add(new wxStaticText(panel, -1, "Chemical Formula:"), 1, wxALIGN_CENTER|wxALL, border);
    hsizer_chemf->Add(entry_s1_chemf, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_chemf->Add(entry_s2_chemf, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_chemf->Add(entry_s3_chemf, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_chemf->Add(entry_s4_chemf, 1, wxALIGN_CENTER|wxALL, border);
    vsizer->Add(hsizer_chemf, 0, wxEXPAND|wxALL, border);

    /* Color */
    entry_s1_color = new wxTextCtrl(panel, -1);
    entry_s2_color = new wxTextCtrl(panel, -1);
    entry_s3_color = new wxTextCtrl(panel, -1);
    entry_s4_color = new wxTextCtrl(panel, -1);
    wxBoxSizer *hsizer_color = new wxBoxSizer(wxHORIZONTAL);
    hsizer_color->Add(new wxStaticText(panel, -1, "Color:"), 1, wxALIGN_CENTER|wxALL, border);
    hsizer_color->Add(entry_s1_color, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_color->Add(entry_s2_color, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_color->Add(entry_s3_color, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_color->Add(entry_s4_color, 1, wxALIGN_CENTER|wxALL, border);
    vsizer->Add(hsizer_color, 0, wxEXPAND|wxALL, border);
 
    /* Fluorescence (SW) */
    entry_s1_flsw = new wxTextCtrl(panel, -1);
    entry_s2_flsw = new wxTextCtrl(panel, -1);
    entry_s3_flsw = new wxTextCtrl(panel, -1);
    entry_s4_flsw = new wxTextCtrl(panel, -1);
    wxBoxSizer *hsizer_flsw = new wxBoxSizer(wxHORIZONTAL);
    hsizer_flsw->Add(new wxStaticText(panel, -1, "Fluorescence (SW):"), 1, wxALIGN_CENTER|wxALL, border);
    hsizer_flsw->Add(entry_s1_flsw, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_flsw->Add(entry_s2_flsw, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_flsw->Add(entry_s3_flsw, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_flsw->Add(entry_s4_flsw, 1, wxALIGN_CENTER|wxALL, border);
    vsizer->Add(hsizer_flsw, 0, wxEXPAND|wxALL, border);
 
    /* Fluorescence (MW) */
    entry_s1_flmw = new wxTextCtrl(panel, -1);
    entry_s2_flmw = new wxTextCtrl(panel, -1);
    entry_s3_flmw = new wxTextCtrl(panel, -1);
    entry_s4_flmw = new wxTextCtrl(panel, -1);
    wxBoxSizer *hsizer_flmw = new wxBoxSizer(wxHORIZONTAL);
    hsizer_flmw->Add(new wxStaticText(panel, -1, "Fluorescence (MW):"), 1, wxALIGN_CENTER|wxALL, border);
    hsizer_flmw->Add(entry_s1_flmw, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_flmw->Add(entry_s2_flmw, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_flmw->Add(entry_s3_flmw, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_flmw->Add(entry_s4_flmw, 1, wxALIGN_CENTER|wxALL, border);
    vsizer->Add(hsizer_flmw, 0, wxEXPAND|wxALL, border);
 
    /* Fluorescence (LW) */
    entry_s1_fllw = new wxTextCtrl(panel, -1);
    entry_s2_fllw = new wxTextCtrl(panel, -1);
    entry_s3_fllw = new wxTextCtrl(panel, -1);
    entry_s4_fllw = new wxTextCtrl(panel, -1);
    wxBoxSizer *hsizer_fllw = new wxBoxSizer(wxHORIZONTAL);
    hsizer_fllw->Add(new wxStaticText(panel, -1, "Fluorescence (LW):"), 1, wxALIGN_CENTER|wxALL, border);
    hsizer_fllw->Add(entry_s1_fllw, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_fllw->Add(entry_s2_fllw, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_fllw->Add(entry_s3_fllw, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_fllw->Add(entry_s4_fllw, 1, wxALIGN_CENTER|wxALL, border);
    vsizer->Add(hsizer_fllw, 0, wxEXPAND|wxALL, border);
 
    /* Fluorescence (405nm) */
    entry_s1_fl405 = new wxTextCtrl(panel, -1);
    entry_s2_fl405 = new wxTextCtrl(panel, -1);
    entry_s3_fl405 = new wxTextCtrl(panel, -1);
    entry_s4_fl405 = new wxTextCtrl(panel, -1);
    wxBoxSizer *hsizer_fl405 = new wxBoxSizer(wxHORIZONTAL);
    hsizer_fl405->Add(new wxStaticText(panel, -1, "Fluorescence (405nm):"), 1, wxALIGN_CENTER|wxALL, border);
    hsizer_fl405->Add(entry_s1_fl405, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_fl405->Add(entry_s2_fl405, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_fl405->Add(entry_s3_fl405, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_fl405->Add(entry_s4_fl405, 1, wxALIGN_CENTER|wxALL, border);
    vsizer->Add(hsizer_fl405, 0, wxEXPAND|wxALL, border);
 
    /* Phoshorescence (SW) */
    entry_s1_phsw = new wxTextCtrl(panel, -1);
    entry_s2_phsw = new wxTextCtrl(panel, -1);
    entry_s3_phsw = new wxTextCtrl(panel, -1);
    entry_s4_phsw = new wxTextCtrl(panel, -1);
    wxBoxSizer *hsizer_phsw = new wxBoxSizer(wxHORIZONTAL);
    hsizer_phsw->Add(new wxStaticText(panel, -1, "Phoshorescence (SW):"), 1, wxALIGN_CENTER|wxALL, border);
    hsizer_phsw->Add(entry_s1_phsw, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_phsw->Add(entry_s2_phsw, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_phsw->Add(entry_s3_phsw, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_phsw->Add(entry_s4_phsw, 1, wxALIGN_CENTER|wxALL, border);
    vsizer->Add(hsizer_phsw, 0, wxEXPAND|wxALL, border);
 
    /* Phoshorescence (MW) */
    entry_s1_phmw = new wxTextCtrl(panel, -1);
    entry_s2_phmw = new wxTextCtrl(panel, -1);
    entry_s3_phmw = new wxTextCtrl(panel, -1);
    entry_s4_phmw = new wxTextCtrl(panel, -1);
    wxBoxSizer *hsizer_phmw = new wxBoxSizer(wxHORIZONTAL);
    hsizer_phmw->Add(new wxStaticText(panel, -1, "Phoshorescence (MW):"), 1, wxALIGN_CENTER|wxALL, border);
    hsizer_phmw->Add(entry_s1_phmw, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_phmw->Add(entry_s2_phmw, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_phmw->Add(entry_s3_phmw, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_phmw->Add(entry_s4_phmw, 1, wxALIGN_CENTER|wxALL, border);
    vsizer->Add(hsizer_phmw, 0, wxEXPAND|wxALL, border);
 
    /* Phoshorescence (LW) */
    entry_s1_phlw = new wxTextCtrl(panel, -1);
    entry_s2_phlw = new wxTextCtrl(panel, -1);
    entry_s3_phlw = new wxTextCtrl(panel, -1);
    entry_s4_phlw = new wxTextCtrl(panel, -1);
    wxBoxSizer *hsizer_phlw = new wxBoxSizer(wxHORIZONTAL);
    hsizer_phlw->Add(new wxStaticText(panel, -1, "Phoshorescence (LW):"), 1, wxALIGN_CENTER|wxALL, border);
    hsizer_phlw->Add(entry_s1_phlw, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_phlw->Add(entry_s2_phlw, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_phlw->Add(entry_s3_phlw, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_phlw->Add(entry_s4_phlw, 1, wxALIGN_CENTER|wxALL, border);
    vsizer->Add(hsizer_phlw, 0, wxEXPAND|wxALL, border);
 
    /* Phoshorescence (405nm) */
    entry_s1_ph405 = new wxTextCtrl(panel, -1);
    entry_s2_ph405 = new wxTextCtrl(panel, -1);
    entry_s3_ph405 = new wxTextCtrl(panel, -1);
    entry_s4_ph405 = new wxTextCtrl(panel, -1);
    wxBoxSizer *hsizer_ph405 = new wxBoxSizer(wxHORIZONTAL);
    hsizer_ph405->Add(new wxStaticText(panel, -1, "Phoshorescence (405nm):"), 1, wxALIGN_CENTER|wxALL, border);
    hsizer_ph405->Add(entry_s1_ph405, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_ph405->Add(entry_s2_ph405, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_ph405->Add(entry_s3_ph405, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_ph405->Add(entry_s4_ph405, 1, wxALIGN_CENTER|wxALL, border);
    vsizer->Add(hsizer_ph405, 0, wxEXPAND|wxALL, border);
 
    /* Tenebrescence */
    entry_s1_tenebr = new wxTextCtrl(panel, -1);
    entry_s2_tenebr = new wxTextCtrl(panel, -1);
    entry_s3_tenebr = new wxTextCtrl(panel, -1);
    entry_s4_tenebr = new wxTextCtrl(panel, -1);
    wxBoxSizer *hsizer_tenebr = new wxBoxSizer(wxHORIZONTAL);
    hsizer_tenebr->Add(new wxStaticText(panel, -1, "Tenebrescence:"), 1, wxALIGN_CENTER|wxALL, border);
    hsizer_tenebr->Add(entry_s1_tenebr, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_tenebr->Add(entry_s2_tenebr, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_tenebr->Add(entry_s3_tenebr, 1, wxALIGN_CENTER|wxALL, border);
    hsizer_tenebr->Add(entry_s4_tenebr, 1, wxALIGN_CENTER|wxALL, border);
    vsizer->Add(hsizer_tenebr, 0, wxEXPAND|wxALL, border);
 
    /* Radioactivity */
    entry_radioact = new wxTextCtrl(panel, -1);
    wxBoxSizer *hsizer_radioact = new wxBoxSizer(wxHORIZONTAL);
    hsizer_radioact->Add(new wxStaticText(panel, -1, "Radioactivity:"), 1, wxALIGN_CENTER|wxALL, border);
    hsizer_radioact->Add(entry_radioact, 4, wxALIGN_CENTER|wxALL, border);
    vsizer->Add(hsizer_radioact, 0, wxEXPAND|wxALL, border);

    /* Comments */
    entry_comments = new wxTextCtrl(panel, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    wxBoxSizer *hsizer_comments = new wxBoxSizer(wxHORIZONTAL);
    hsizer_comments->Add(new wxStaticText(panel, -1, "Comments:"), 1, wxEXPAND|wxALL, border);
    hsizer_comments->Add(entry_comments, 4, wxEXPAND|wxALL, border);
    vsizer->Add(hsizer_comments, 1, wxEXPAND|wxALL, border);

    /* ok cancel buttons */
    wxBoxSizer *hsizer_buttons = new wxBoxSizer(wxHORIZONTAL);
    wxButton *button_save = new wxButton(panel, ID_AddMod_Save, wxString("Save"));
    wxButton *button_cancel = new wxButton(panel, ID_AddMod_Cancel, wxString("Cancel"));
    hsizer_buttons->Add(new wxStaticText(panel, -1, ""), 3, wxALL, border);
    hsizer_buttons->Add(button_cancel, 0, wxALL, border);
    hsizer_buttons->Add(button_save, 0, wxALL, border);
    vsizer->Add(hsizer_buttons, 0, wxEXPAND|wxALL, border);

    /* Fit everything in the panel and add scrollbars */
    panel->SetSizerAndFit(vsizer);
    panel->FitInside();
    panel->SetScrollRate(5, 5);

    /* If given uid, populate from with data from db */
    modifying = uid;
    if (uid>=0) {
        PopulateForm(uid);
    }
}

void AddModFrame::OnSave(wxCommandEvent& event) {

    std::vector<std::string> data;

    data.push_back(entry_minid->GetValue().ToStdString());

    data.push_back(entry_name->GetValue().ToStdString());
    data.push_back(entry_locality->GetValue().ToStdString());
    data.push_back(entry_locid->GetValue().ToStdString());
    data.push_back(entry_size->GetValue().ToStdString());
    data.push_back(entry_weight->GetValue().ToStdString());
    data.push_back(entry_acquisition->GetValue().ToStdString());
    data.push_back(entry_collection->GetValue().ToStdString());
    data.push_back(entry_value->GetValue().ToStdString());

    data.push_back(entry_s1_species->GetValue().ToStdString());
    data.push_back(entry_s1_class->GetValue().ToStdString());
    data.push_back(entry_s1_chemf->GetValue().ToStdString());
    data.push_back(entry_s1_color->GetValue().ToStdString());
    data.push_back(entry_s1_flsw->GetValue().ToStdString());
    data.push_back(entry_s1_flmw->GetValue().ToStdString());
    data.push_back(entry_s1_fllw->GetValue().ToStdString());
    data.push_back(entry_s1_fl405->GetValue().ToStdString());
    data.push_back(entry_s1_phsw->GetValue().ToStdString());
    data.push_back(entry_s1_phmw->GetValue().ToStdString());
    data.push_back(entry_s1_phlw->GetValue().ToStdString());
    data.push_back(entry_s1_ph405->GetValue().ToStdString());
    data.push_back(entry_s1_tenebr->GetValue().ToStdString());

    data.push_back(entry_s2_species->GetValue().ToStdString());
    data.push_back(entry_s2_class->GetValue().ToStdString());
    data.push_back(entry_s2_chemf->GetValue().ToStdString());
    data.push_back(entry_s2_color->GetValue().ToStdString());
    data.push_back(entry_s2_flsw->GetValue().ToStdString());
    data.push_back(entry_s2_flmw->GetValue().ToStdString());
    data.push_back(entry_s2_fllw->GetValue().ToStdString());
    data.push_back(entry_s2_fl405->GetValue().ToStdString());
    data.push_back(entry_s2_phsw->GetValue().ToStdString());
    data.push_back(entry_s2_phmw->GetValue().ToStdString());
    data.push_back(entry_s2_phlw->GetValue().ToStdString());
    data.push_back(entry_s2_ph405->GetValue().ToStdString());
    data.push_back(entry_s2_tenebr->GetValue().ToStdString());

    data.push_back(entry_s3_species->GetValue().ToStdString());
    data.push_back(entry_s3_class->GetValue().ToStdString());
    data.push_back(entry_s3_chemf->GetValue().ToStdString());
    data.push_back(entry_s3_color->GetValue().ToStdString());
    data.push_back(entry_s3_flsw->GetValue().ToStdString());
    data.push_back(entry_s3_flmw->GetValue().ToStdString());
    data.push_back(entry_s3_fllw->GetValue().ToStdString());
    data.push_back(entry_s3_fl405->GetValue().ToStdString());
    data.push_back(entry_s3_phsw->GetValue().ToStdString());
    data.push_back(entry_s3_phmw->GetValue().ToStdString());
    data.push_back(entry_s3_phlw->GetValue().ToStdString());
    data.push_back(entry_s3_ph405->GetValue().ToStdString());
    data.push_back(entry_s3_tenebr->GetValue().ToStdString());

    data.push_back(entry_s4_species->GetValue().ToStdString());
    data.push_back(entry_s4_class->GetValue().ToStdString());
    data.push_back(entry_s4_chemf->GetValue().ToStdString());
    data.push_back(entry_s4_color->GetValue().ToStdString());
    data.push_back(entry_s4_flsw->GetValue().ToStdString());
    data.push_back(entry_s4_flmw->GetValue().ToStdString());
    data.push_back(entry_s4_fllw->GetValue().ToStdString());
    data.push_back(entry_s4_fl405->GetValue().ToStdString());
    data.push_back(entry_s4_phsw->GetValue().ToStdString());
    data.push_back(entry_s4_phmw->GetValue().ToStdString());
    data.push_back(entry_s4_phlw->GetValue().ToStdString());
    data.push_back(entry_s4_ph405->GetValue().ToStdString());
    data.push_back(entry_s4_tenebr->GetValue().ToStdString());

    data.push_back(entry_radioact->GetValue().ToStdString());
    data.push_back(entry_comments->GetValue().ToStdString());

    std::string errmsg = "";
    int success_id = db_addmod_mineral(db, data, modifying, &errmsg);
    if (success_id<-1) {
        wxLogMessage(wxString(errmsg));
    }

    main_window->populate_listbox();
    main_window->draw_mineral_view(success_id);
    Close(true);
    return;
}

void AddModFrame::OnCancel(wxCommandEvent& event) {
    Close(true);
}

void AddModFrame::PopulateForm(int uid) {
    int ret;
    const char *query = "SELECT * FROM MINERALS WHERE MINID=?";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, uid);
    ret=sqlite3_step(stmt);
    if (ret!=SQLITE_ROW) {
        wxLogMessage("Impossible to find ID in database :(");
        return;
    }

    sqlite3_column_text(stmt, 1);

    int ndx = 0;
    entry_minid->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_name->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_locality->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_locid->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_size->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_weight->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_acquisition->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_collection->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_value->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;

    entry_s1_species->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s1_class->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s1_chemf->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s1_color->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s1_flsw->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s1_flmw->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s1_fllw->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s1_fl405->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s1_phsw->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s1_phmw->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s1_phlw->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s1_ph405->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s1_tenebr->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;

    entry_s2_species->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s2_class->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s2_chemf->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s2_color->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s2_flsw->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s2_flmw->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s2_fllw->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s2_fl405->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s2_phsw->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s2_phmw->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s2_phlw->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s2_ph405->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s2_tenebr->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;

    entry_s3_species->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s3_class->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s3_chemf->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s3_color->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s3_flsw->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s3_flmw->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s3_fllw->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s3_fl405->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s3_phsw->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s3_phmw->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s3_phlw->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s3_ph405->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s3_tenebr->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
   
    entry_s4_species->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s4_class->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s4_chemf->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s4_color->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s4_flsw->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s4_flmw->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s4_fllw->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s4_fl405->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s4_phsw->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s4_phmw->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s4_phlw->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s4_ph405->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;
    entry_s4_tenebr->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;

    entry_radioact->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;

    entry_comments->AppendText(wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8)); ndx+=1;

    sqlite3_finalize(stmt);

    return;
}

