
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/richtext/richtextctrl.h>

#include <sqlite3.h> 

#include "mainframe.h"
#include "addmodframe.h"

wxBEGIN_EVENT_TABLE(AddModFrame, wxFrame)
    EVT_BUTTON(ID_AddMod_Save, AddModFrame::OnSave)
    EVT_BUTTON(ID_AddMod_Cancel, AddModFrame::OnCancel)
wxEND_EVENT_TABLE()


AddModFrame::AddModFrame(MainFrame *parent, const wxString& title, sqlite3 *maindb, int uid)
        : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(1000,800), wxDEFAULT_FRAME_STYLE|wxFRAME_FLOAT_ON_PARENT) {

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

    const char *query_insert = "INSERT INTO MINERALS (MINID, NAME, LOCALITY, LOCID_MNDAT, SIZE, WEIGHT, ACQUISITION, COLLECTION, VALUE, S1_SPECIES, S1_CLASS, S1_CHEMF, S1_COLOR, S1_FLSW, S1_FLMW, S1_FLLW, S1_FL405, S1_PHSW, S1_PHMW, S1_PHLW, S1_PH405, S1_TENEBR, S2_SPECIES, S2_CLASS, S2_CHEMF, S2_COLOR, S2_FLSW, S2_FLMW, S2_FLLW, S2_FL405, S2_PHSW, S2_PHMW, S2_PHLW, S2_PH405, S2_TENEBR, S3_SPECIES, S3_CLASS, S3_CHEMF, S3_COLOR, S3_FLSW, S3_FLMW, S3_FLLW, S3_FL405, S3_PHSW, S3_PHMW, S3_PHLW, S3_PH405, S3_TENEBR, S4_SPECIES, S4_CLASS, S4_CHEMF, S4_COLOR, S4_FLSW, S4_FLMW, S4_FLLW, S4_FL405, S4_PHSW, S4_PHMW, S4_PHLW, S4_PH405, S4_TENEBR, RADIOACT, COMMENTS) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);";
    const char *query_insert_autoid = "INSERT INTO MINERALS (NAME, LOCALITY, LOCID_MNDAT, SIZE, WEIGHT, ACQUISITION, COLLECTION, VALUE, S1_SPECIES, S1_CLASS, S1_CHEMF, S1_COLOR, S1_FLSW, S1_FLMW, S1_FLLW, S1_FL405, S1_PHSW, S1_PHMW, S1_PHLW, S1_PH405, S1_TENEBR, S2_SPECIES, S2_CLASS, S2_CHEMF, S2_COLOR, S2_FLSW, S2_FLMW, S2_FLLW, S2_FL405, S2_PHSW, S2_PHMW, S2_PHLW, S2_PH405, S2_TENEBR, S3_SPECIES, S3_CLASS, S3_CHEMF, S3_COLOR, S3_FLSW, S3_FLMW, S3_FLLW, S3_FL405, S3_PHSW, S3_PHMW, S3_PHLW, S3_PH405, S3_TENEBR, S4_SPECIES, S4_CLASS, S4_CHEMF, S4_COLOR, S4_FLSW, S4_FLMW, S4_FLLW, S4_FL405, S4_PHSW, S4_PHMW, S4_PHLW, S4_PH405, S4_TENEBR, RADIOACT, COMMENTS) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);";
   const char *query_update = "UPDATE MINERALS SET MINID=?, NAME=?, LOCALITY=?, LOCID_MNDAT=?, SIZE=?, WEIGHT=?, ACQUISITION=?, COLLECTION=?, VALUE=?, S1_SPECIES=?, S1_CLASS=?, S1_CHEMF=?, S1_COLOR=?, S1_FLSW=?, S1_FLMW=?, S1_FLLW=?, S1_FL405=?, S1_PHSW=?, S1_PHMW=?, S1_PHLW=?, S1_PH405=?, S1_TENEBR=?, S2_SPECIES=?, S2_CLASS=?, S2_CHEMF=?, S2_COLOR=?, S2_FLSW=?, S2_FLMW=?, S2_FLLW=?, S2_FL405=?, S2_PHSW=?, S2_PHMW=?, S2_PHLW=?, S2_PH405=?, S2_TENEBR=?, S3_SPECIES=?, S3_CLASS=?, S3_CHEMF=?, S3_COLOR=?, S3_FLSW=?, S3_FLMW=?, S3_FLLW=?, S3_FL405=?, S3_PHSW=?, S3_PHMW=?, S3_PHLW=?, S3_PH405=?, S3_TENEBR=?, S4_SPECIES=?, S4_CLASS=?, S4_CHEMF=?, S4_COLOR=?, S4_FLSW=?, S4_FLMW=?, S4_FLLW=?, S4_FL405=?, S4_PHSW=?, S4_PHMW=?, S4_PHLW=?, S4_PH405=?, S4_TENEBR=?, RADIOACT=?, COMMENTS=? WHERE MINID=?;";

    /* Setup the db connection and choose the query to use: with specified minid, or auto, or updte */
    int ret;
    sqlite3_stmt *stmt;
    const char *minid_str = entry_minid->GetValue().ToStdString().c_str();
    int minid_int;
    int ndx = 1;
    ret = sscanf(minid_str, "%d", &minid_int);
    if (ret!=1) {
        minid_int = -1;
        if (strlen(minid_str)>0) {
            wxLogMessage("WARNING! You specified an ID but I could not make and integer from it. Using a random ID instead.");
        }
    }
    if (modifying<0) {
        if (minid_int<0) {
            sqlite3_prepare_v2(db, query_insert_autoid, strlen(query_insert_autoid), &stmt, NULL);
        } else {
            sqlite3_prepare_v2(db, query_insert, strlen(query_insert), &stmt, NULL);
            sqlite3_bind_int(stmt, ndx, minid_int); ndx+=1;
        }
    } else {
        sqlite3_prepare_v2(db, query_update, strlen(query_update), &stmt, NULL);
        if (minid_int<0) {
            minid_int = modifying;
        }
        sqlite3_bind_int(stmt, ndx, minid_int); ndx+=1;
    }

    /* Bind all 63 inputs... */
    sqlite3_bind_text(stmt, ndx, entry_name->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_locality->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_locid->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_size->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_weight->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_acquisition->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_collection->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_value->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;

    sqlite3_bind_text(stmt, ndx, entry_s1_species->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s1_class->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s1_chemf->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s1_color->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s1_flsw->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s1_flmw->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s1_fllw->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s1_fl405->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s1_phsw->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s1_phmw->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s1_phlw->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s1_ph405->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s1_tenebr->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;

    sqlite3_bind_text(stmt, ndx, entry_s2_species->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s2_class->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s2_chemf->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s2_color->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s2_flsw->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s2_flmw->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s2_fllw->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s2_fl405->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s2_phsw->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s2_phmw->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s2_phlw->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s2_ph405->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s2_tenebr->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;

    sqlite3_bind_text(stmt, ndx, entry_s3_species->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s3_class->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s3_chemf->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s3_color->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s3_flsw->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s3_flmw->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s3_fllw->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s3_fl405->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s3_phsw->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s3_phmw->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s3_phlw->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s3_ph405->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s3_tenebr->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;

    sqlite3_bind_text(stmt, ndx, entry_s4_species->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s4_class->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s4_chemf->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s4_color->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s4_flsw->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s4_flmw->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s4_fllw->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s4_fl405->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s4_phsw->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s4_phmw->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s4_phlw->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s4_ph405->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_s4_tenebr->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;

    sqlite3_bind_text(stmt, ndx, entry_radioact->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    sqlite3_bind_text(stmt, ndx, entry_comments->GetValue().utf8_str(), -1, SQLITE_TRANSIENT); ndx+=1;

    if (modifying>=0) {
        sqlite3_bind_int(stmt, ndx, modifying); ndx+=1;
    }

    ret = sqlite3_step(stmt);
    if (ret!=SQLITE_DONE) {
        wxLogMessage(wxString("error: ") + wxString(sqlite3_errmsg(db)));
    }
    sqlite3_finalize(stmt);

    main_window->populate_listbox();
    main_window->draw_mineral_view(minid_int);
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

