
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/richtext/richtextctrl.h>
#include <wx/aboutdlg.h>

#include <sqlite3.h> 

#include "mainframe.h"
#include "addmodframe.h"
#include "utils.h"
#include "csv.hpp"
#include "addtodb.hpp"

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(ID_ExportCSV,        MainFrame::export_csv)
    EVT_MENU(ID_ImportCSV,        MainFrame::import_csv)
    EVT_MENU(ID_NewMineral,       MainFrame::OnNewMineral)
    EVT_MENU(ID_ModifyMineral,    MainFrame::OnModifyMineral)
    EVT_MENU(ID_DuplicateMineral, MainFrame::OnDuplicateMineral)
    EVT_MENU(ID_DeleteMineral,    MainFrame::OnDeleteMineral)
    EVT_MENU(wxID_OPEN,  MainFrame::OnOpen)
    EVT_MENU(wxID_SAVE,  MainFrame::OnSave)
    EVT_MENU(wxID_CLOSE, MainFrame::OnClose)
    EVT_MENU(wxID_EXIT,  MainFrame::OnExit)
    EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
    EVT_MENU(wxID_HELP,  MainFrame::OnHelp)
    EVT_LISTBOX(ID_SelectMineral, MainFrame::OnSelectMineral)
    EVT_TEXT(ID_SearchMineral, MainFrame::populate_listbox_evt)
    EVT_RADIOBOX(ID_OrderByMineral, MainFrame::populate_listbox_evt)
    EVT_CHOICE(ID_FilterCountry, MainFrame::populate_listbox_evt)
    EVT_TEXT_URL(wxID_ANY, MainFrame::OnURL)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
        : wxFrame(NULL, wxID_ANY, title, pos, size) {
    /* Initialize DB */
    db = NULL;
    db_file_path = "";
    wxImage::AddHandler(new wxPNGHandler);
    wxImage::AddHandler(new wxJPEGHandler);
    wxImage::AddHandler(new wxGIFHandler);
    wxImage::AddHandler(new wxTIFFHandler);
    /* Menu Bar */
    menuFile = new wxMenu;
    menuFile->Append(wxID_OPEN);
    menuFile->Append(wxID_SAVE);
    menuFile->Append(wxID_CLOSE);
    menuFile->Append(ID_ImportCSV, "&Import CSV", "Import mineral database from a CSV file");
    menuFile->Append(ID_ExportCSV, "&Export CSV", "Export mineral database as CSV file");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
    menuMineral = new wxMenu;
    menuMineral->Append(ID_NewMineral, "&Add", "Add a new mineral to the database");
    menuMineral->Append(ID_ModifyMineral, "&Modify", "Modify the selected mineral");
    menuMineral->Append(ID_DuplicateMineral, "&Duplicate", "Duplicate the selected mineral");
    menuMineral->Append(ID_DeleteMineral, "&Delete", "Delete the selected mineral");
    menuMineral->AppendSeparator();
    menuMineral->AppendCheckItem(ID_HIDEVALUE, "&Hide mineral value");
    menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
    menuHelp->Append(wxID_HELP, "&Read the manual online", "Open the MineralApp manual on the browser");
    menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File" );
    menuBar->Append(menuMineral, "&Mineral" );
    menuBar->Append(menuHelp, "&Help" );
    SetMenuBar( menuBar );
    /* Status Bar */
    CreateStatusBar();
    SetStatusText("Welcome to MineralApp!" );
    /* ListBox */
    mineral_listbox = new wxListBox(this, ID_SelectMineral);
    /* Search entry */
    wxStaticText *mineral_search_label = new wxStaticText(this, -1, "Filter:");
    mineral_search = new wxTextCtrl(this, ID_SearchMineral, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    /* Order by */
    wxStaticText *mineral_orderby_label = new wxStaticText(this, -1, "Order by:");
    wxArrayString orderby_choices;
    orderby_choices.Add("Unique ID");
    orderby_choices.Add("Name");
    mineral_orderby = new wxRadioBox(this, ID_OrderByMineral, wxEmptyString, wxDefaultPosition, wxDefaultSize, orderby_choices, 2, wxRA_HORIZONTAL);
    /* Country filter */
    wxStaticText *mineral_country_label = new wxStaticText(this, -1, "Country:");
    mineral_country = new wxChoice(this, ID_FilterCountry, wxDefaultPosition, wxDefaultSize, 0, NULL);
    /* Grid sizer */
    wxFlexGridSizer *leftgrid = new wxFlexGridSizer(2,0,0);
    leftgrid->AddGrowableCol(1,1);
    leftgrid->Add(mineral_search_label,  0, wxEXPAND | wxALIGN_CENTER_VERTICAL | wxALL, 2);
    leftgrid->Add(mineral_search,        1, wxEXPAND | wxALIGN_CENTER_VERTICAL | wxALL, 2);
    leftgrid->Add(mineral_orderby_label, 0, wxEXPAND | wxALIGN_CENTER_VERTICAL | wxALL, 2);
    leftgrid->Add(mineral_orderby,       1, wxEXPAND | wxALIGN_CENTER_VERTICAL | wxALL, 2);
    leftgrid->Add(mineral_country_label, 0, wxEXPAND | wxALIGN_CENTER_VERTICAL | wxALL, 2);
    leftgrid->Add(mineral_country,       1, wxEXPAND | wxALIGN_CENTER_VERTICAL | wxALL, 2);
    /* Viewbox */
    mineral_view = new wxRichTextCtrl(this, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY, wxDefaultValidator, wxTextCtrlNameStr);
    /* Sizers */
    wxBoxSizer *hsizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *leftvsizer = new wxBoxSizer(wxVERTICAL);
    leftvsizer->Add(mineral_listbox, 1, wxEXPAND | wxALL, 5);
    leftvsizer->Add(leftgrid, 0, wxEXPAND | wxALL, 5);
    //leftvsizer->SetSizeHints(this);
    hsizer->Add(leftvsizer, 0, wxEXPAND | wxALL, 5);
    hsizer->Add(mineral_view, 1, wxEXPAND | wxALL, 5);
    //hsizer->SetSizeHints(this);
    SetSizerAndFit(hsizer);
    /* Read config file */
    read_config();
}

void MainFrame::OnOpen(wxCommandEvent& event) {
    /* Check if some db is already opened and warn the user */
    if (db) {
        wxMessageDialog *dial = new wxMessageDialog(NULL, "You have already an open database. Do you want to discard it and open a new one?", "Question", wxYES_NO|wxNO_DEFAULT|wxICON_QUESTION);
        if (dial->ShowModal()!=wxID_YES) {
            return;
        }
    }
    /* Get the filename of the db to read */
    wxFileDialog openFileDialog(this, "Open database", wxEmptyString, "minerals.sqlite3",  "Database files (*.sqlite3)|*.sqlite3", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal()==wxID_CANCEL) {
        return;
    }
    wxString fname = openFileDialog.GetPath();
    /* Open it */
    open_dbfile(fname.ToStdString());
}

void MainFrame::open_dbfile(std::string fname) {

    int ret;

    /* Open the db from file */
    sqlite3 *db_tmp;
    ret = sqlite3_open(static_cast<const char*>(fname.c_str()), &db_tmp);
    if (ret!=SQLITE_OK) {
        wxLogMessage("Failed to open file for reading!");
        return;
    }
    /* Create a new db in memory */
    ret = sqlite3_open(":memory:", &db);
    if (ret!=SQLITE_OK) {
        wxLogMessage("Can't create memory database: %s", sqlite3_errmsg(db));
        return;
    }
    /* Make the copy */
    sqlite3_backup *bkp;
    bkp = sqlite3_backup_init(db, "main", db_tmp, "main");
    if (!bkp) {
        wxLogMessage("Failed to backup file!");
        return;
    }
    sqlite3_backup_step(bkp, -1);
    sqlite3_backup_finish(bkp);
    sqlite3_close(db_tmp);

    /* Store info */
    db_file_path = fs::path(fname);
    write_config();
    update_gui();

    return;
}


void MainFrame::OnSave(wxCommandEvent& event) {
    if (!db) {
        wxLogMessage("Nothing to save!");
        return;
    }
    wxFileDialog saveFileDialog(this, "Save database", wxEmptyString, "minerals.sqlite3",  "Database files (*.sqlite3)|*.sqlite3", wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
    if (saveFileDialog.ShowModal() == wxID_CANCEL) {
        return;
    }
    wxString fname2 = saveFileDialog.GetPath();
    const char *fname = static_cast<const char*>(fname2.c_str());
    sqlite3 *db_tmp;
    int ret = sqlite3_open(fname, &db_tmp);
    if (ret!=SQLITE_OK) {
        wxLogMessage("Failed to open file for writing!");
        return;
    }
    sqlite3_backup *bkp;
    bkp = sqlite3_backup_init(db_tmp, "main", db, "main");
    if (!bkp) {
        wxLogMessage("Failed to backup file!");
        return;
    }
    sqlite3_backup_step(bkp, -1);
    sqlite3_backup_finish(bkp);
    sqlite3_close(db_tmp);
}

void MainFrame::OnClose(wxCommandEvent& event) {
    std::string msg = "You are going to close the current database. If you did not save it any edits will be lost. This operation cannot be undone.";
    wxMessageDialog dial(this, msg, "Are you sure you want to close the current database??", wxYES_NO | wxCANCEL | wxNO_DEFAULT);
    if (dial.ShowModal() != wxID_YES) return;
    int ret;
    ret = sqlite3_close(db);
    if (ret!=SQLITE_OK) {
        wxLogMessage("Cannot close current db. Sorry!");
        return;
    }
    db = NULL;
    mineral_listbox->Clear();
    mineral_view->Clear();
}

void MainFrame::OnExit(wxCommandEvent& event) {
    Close(true);
}

void MainFrame::OnAbout(wxCommandEvent& event) {
    wxAboutDialogInfo info;
    info.SetName("MineralApp");
    info.SetVersion(VERSION);
    info.SetDescription( ""
        "A small and simple application to create a database of "
        "your minerals. You can add your mineral collection, storing any details "
        "you are interested in, helping you (hopefully...) to keep your mineral "
        "collection well organized!");
    info.SetCopyright("(C) 2019-2020 Simone Conti <3dz2.com/mineralapp/>");
    wxAboutBox(info);
}

void MainFrame::OnHelp(wxCommandEvent& event) {
    wxLaunchDefaultBrowser("https://3dz2.com/mineralapp/manual/");
}

void MainFrame::OnURL(wxTextUrlEvent& event) {
    wxLaunchDefaultBrowser(event.GetString());
    mineral_view->SetCaretPosition(0);
    mineral_view->SetDefaultStyleToCursorStyle();
}

void MainFrame::OnNewMineral(wxCommandEvent& event) {
    if (!db) {
        db_initialize();
    }
    if (!db) {
        wxMessageBox("DB initialization failed! Sorry, try to close everything and retry...");
        return;
    }
    AddModFrame *frame = new AddModFrame(this, "Add new mineral", db, -1);
    frame->Show();
}

int MainFrame::get_minid_from_listbox() {
    int minid;
    int selected = mineral_listbox->GetSelection();
    if (selected==wxNOT_FOUND) {
        wxLogMessage("Please, select a mineral from the left panel.");
        return -1;
    }
    wxString label = mineral_listbox->GetString(selected);
    int ndxi = label.rfind('[');
    int ndxf = label.rfind(']');
    int ret = sscanf(label.substr(ndxi,ndxf).c_str(), "[%d]", &minid);
    if (ret!=1) {
        wxLogMessage("Please, select a mineral from the left panel.");
        return -1;
    }
    return minid;
}

void MainFrame::OnModifyMineral(wxCommandEvent& event) {
    int minid = get_minid_from_listbox();
    if (minid<0) return;
    AddModFrame *frame = new AddModFrame(this, "Add new mineral", db, minid);
    frame->Show();
}

void MainFrame::OnDuplicateMineral(wxCommandEvent& event) {
    int minid = get_minid_from_listbox();
    if (minid<0) return;
    std::string  query = " \
        CREATE TEMPORARY TABLE temp_table as SELECT * FROM  MINERALS WHERE minid=" + std::to_string(minid) + "; \
        UPDATE temp_table SET MINID=NULL; \
        INSERT INTO MINERALS SELECT * FROM temp_table; \
        DROP TABLE temp_table; \
    ";
    int ret;
    char *errmsg;
    ret = sqlite3_exec(db, query.c_str(), NULL, 0, &errmsg);
    if (ret!=SQLITE_OK) {
        wxLogMessage("SQL error: %s", errmsg);
        sqlite3_free(errmsg);
        return;
    }
    update_gui();
    wxLogMessage("Mineral duplicated!");
}

void MainFrame::OnDeleteMineral(wxCommandEvent& event) {
    int minid = get_minid_from_listbox();
    if (minid<0) return;
    std::string msg = "You are going to delete mineral ID " + std::to_string(minid) +  ". This operation cannot be undone.";
    wxMessageDialog dial(this, msg, "Are you sure you want to delete this mineral?", wxYES_NO | wxCANCEL | wxNO_DEFAULT);
    if (dial.ShowModal() != wxID_YES) return;
    std::string query = "DELETE FROM MINERALS WHERE minid=" + std::to_string(minid);
    int ret;
    char *errmsg;
    ret = sqlite3_exec(db, query.c_str(), NULL, 0, &errmsg);
    if (ret!=SQLITE_OK) {
        wxLogMessage("SQL error: %s", errmsg);
        sqlite3_free(errmsg);
        return;
    }
    update_gui();
    mineral_view->Clear();
}

void MainFrame::OnSelectMineral(wxCommandEvent& event) {
    int minid = get_minid_from_listbox();
    if (minid<0) return;
    draw_mineral_view(minid);
    return;
}

void MainFrame::draw_mineral_view(int minid) {
    mineral_view->Clear();
    //mineral_view->SetDefaultStyleToCursorStyle();
    if (minid<0) {
        return;
    }
    int ret, ndx;
    const char *query = "SELECT * FROM MINERALS WHERE MINID=?";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, minid);
    ret=sqlite3_step(stmt);
    if (ret!=SQLITE_ROW) {
        wxLogMessage("Impossible to find ID in database :(");
        return;
    }

    /* Start formatting */
    wxRichTextCtrl *r = mineral_view;

    /* Set monospace */
    wxFont monospace = wxFont(wxFontInfo().FaceName("Andale Mono"));
    if (!monospace.IsFixedWidth()) {
        monospace = wxFont(wxFontInfo().FaceName("Consolas"));
    }
    r->BeginFont(monospace);

    /* Make a style suitable for showing a URL */
    wxRichTextAttr urlStyle;
    urlStyle.SetTextColour(*wxBLUE);
    urlStyle.SetFontUnderlined(true);

    /* Title */
    r->BeginFontSize(16);
    wxString title = wxString(sqlite3_column_text(stmt, 1), wxConvUTF8);
    r->WriteText(title);
    r->EndFontSize();
    r->Newline();
    r->Newline();
    
    /* ID */
    wxString uid = wxString(sqlite3_column_text(stmt, 0), wxConvUTF8);
    r->BeginBold();
    r->WriteText("Unique ID      : ");
    r->EndBold();
    r->WriteText(uid);
    r->Newline();

    /* Locality */
    wxString locality = wxString(sqlite3_column_text(stmt, 2), wxConvUTF8);
    wxString locid = wxString(sqlite3_column_text(stmt, 3), wxConvUTF8);
    r->BeginBold();
    r->WriteText("Locality       : ");
    r->EndBold();
    if (!locality.empty()) r->WriteText(locality);
    if (!locid.empty()) {
        r->WriteText(" (");
        r->BeginStyle(urlStyle);
        r->BeginURL(wxString("https://www.mindat.org/loc-")+locid+".html");
        r->WriteText("See on MINDAT");
        r->EndURL();
        r->EndStyle();
        r->WriteText(")");
    }
    r->Newline();
    
    /* Size, weight, acquisition, collection value */
    wxString size = wxString(sqlite3_column_text(stmt, 4), wxConvUTF8);
    wxString weight = wxString(sqlite3_column_text(stmt, 5), wxConvUTF8);
    wxString acquisition = wxString(sqlite3_column_text(stmt, 6), wxConvUTF8);
    wxString collection = wxString(sqlite3_column_text(stmt, 7), wxConvUTF8);
    wxString value = wxString(sqlite3_column_text(stmt, 8), wxConvUTF8);
    r->BeginBold(); r->WriteText("Size           : "); r->EndBold(); if (size.length()>0) r->WriteText(size); r->Newline();
    r->BeginBold(); r->WriteText("Weight         : "); r->EndBold(); if (weight.length()>0) r->WriteText(weight); r->Newline();
    r->BeginBold(); r->WriteText("Acquisition    : "); r->EndBold(); if (acquisition.length()>0) r->WriteText(acquisition); r->Newline();
    r->BeginBold(); r->WriteText("Collection     : "); r->EndBold(); if (collection.length()>0) r->WriteText(collection); r->Newline();
    r->BeginBold(); r->WriteText("Value          : "); r->EndBold();
    if (menuMineral->IsChecked(ID_HIDEVALUE)) {
        r->WriteText("<hidden>"); r->Newline();
    } else {
        if (value.length()>0) r->WriteText(value); r->Newline();
    }
    r->Newline();

    /* Table of species */
    ndx = 9;
    write_table_row(stmt, "Species        ", ndx); ndx+=1;
    write_table_row(stmt, "Class          ", ndx); ndx+=1;
    write_table_row_chemf(stmt, "Chem. Formula  ", ndx); ndx+=1;
    write_table_row(stmt, "Color          ", ndx); ndx+=1;
    write_table_row(stmt, "Fluorescence SW", ndx); ndx+=1;
    write_table_row(stmt, "Fluorescence MW", ndx); ndx+=1;
    write_table_row(stmt, "Fluorescence LW", ndx); ndx+=1;
    write_table_row(stmt, "Fluor. 405nm   ", ndx); ndx+=1;
    write_table_row(stmt, "Phosphor. SW   ", ndx); ndx+=1;
    write_table_row(stmt, "Phosphor. MW   ", ndx); ndx+=1;
    write_table_row(stmt, "Phosphor. LW   ", ndx); ndx+=1;
    write_table_row(stmt, "Phosphor. 405nm", ndx); ndx+=1;
    write_table_row(stmt, "Tenebrescence  ", ndx); ndx+=1;
    ndx += 13*3;
    write_link_row(stmt);
    r->Newline();

    /* Radioactivity */
    wxString radioact = wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8); ndx+=1;
    if (!radioact.empty()) {r->BeginBold(); r->WriteText("Radioactivity  : "); r->EndBold(); r->WriteText(radioact); r->Newline();}

    /* Comments */
    wxString comments = wxString(sqlite3_column_text(stmt, ndx), wxConvUTF8); ndx+=1;
    if (!comments.empty()) {r->BeginBold(); r->WriteText("Comments       : "); r->EndBold(); r->WriteText(comments); r->Newline();}

    /* Data */
    ReadData(uid.ToStdString());

    r->EndFont();

    sqlite3_finalize(stmt);

    return;
}

static inline int guess_column_width(sqlite3_stmt *stmt, int column) {
    int n, ln;
    int rowbegin = 9;
    int nrows = 13;
    int length = 16;
    for (n=rowbegin+column*nrows; n<rowbegin+column*nrows+nrows; n++) {
        ln = wxString(sqlite3_column_text(stmt, n), wxConvUTF8).length();
        length = std::max(length, ln);
    }
    return length+1;
}

void MainFrame::write_table_row(sqlite3_stmt *stmt, wxString name, int ndx) {
    wxRichTextCtrl *r = mineral_view;
    wxString s1 = wxString(sqlite3_column_text(stmt, ndx+0*13), wxConvUTF8);
    wxString s2 = wxString(sqlite3_column_text(stmt, ndx+1*13), wxConvUTF8);
    wxString s3 = wxString(sqlite3_column_text(stmt, ndx+2*13), wxConvUTF8);
    wxString s4 = wxString(sqlite3_column_text(stmt, ndx+3*13), wxConvUTF8);
    if (s1=="No" || s1=="no") s1=wxString("");
    if (s2=="No" || s2=="no") s2=wxString("");
    if (s3=="No" || s3=="no") s3=wxString("");
    if (s4=="No" || s4=="no") s4=wxString("");
    int l1 = s1.length();
    int l2 = s2.length();
    int l3 = s3.length();
    int l4 = s4.length();
    if (l1+l2+l3+l4>0) {
        r->BeginBold(); r->WriteText(name+": "); r->EndBold();
        r->WriteText(s1+std::string(guess_column_width(stmt, 0)-s1.length(), ' '));
        if (l2+l3+l4>0) r->WriteText(s2+std::string(guess_column_width(stmt, 1)-s2.length(), ' '));
        if (l3+l4>0) r->WriteText(s3+std::string(guess_column_width(stmt, 2)-s3.length(), ' '));
        if (l4>0) r->WriteText(s4+std::string(guess_column_width(stmt, 3)-s4.length(), ' '));
        r->Newline();
    }
}

/*
    All the subsup mess here is because "monospace" is not monospace when subscripts or
    superscripts are used. These occupy approximately (?) 1/3 of the space normally
    used.
*/
static int write_chemf(wxRichTextCtrl *r, wxString chemfwx, int *subsup) {

    std::string chemf = chemfwx.ToStdString();
    strip_unicode(chemf);

    if (chemf=="") return 0;

    int inserted = 0;
    bool subscript = true;

    if (chemf.find(" . ")!=std::string::npos) {
        chemf = chemf.replace(chemf.find(" . "), 3, "*");
    }
    for (size_t n=0; n<chemf.length(); n++) {
        char tchar = chemf[n];
        char nchar = 0;
        if (n<chemf.length()-1) {
            nchar = chemf[n+1];
        }
        if (tchar=='+' || tchar=='-' || nchar=='+' || nchar=='-') {
            r->WriteText(wxString(1,tchar));
            r->SetSelection(r->GetCaretPosition(), r->GetCaretPosition()+1);
            r->ApplyTextEffectToSelection(wxTEXT_ATTR_EFFECT_SUPERSCRIPT);
            r->SetSelection(0,0);
            (*subsup)++;
        } else if ((isdigit(tchar) || tchar=='.') && subscript) {
            r->WriteText(wxString(1,tchar));
            r->SetSelection(r->GetCaretPosition(), r->GetCaretPosition()+1);
            r->ApplyTextEffectToSelection(wxTEXT_ATTR_EFFECT_SUBSCRIPT);
            r->SetSelection(0,0);
            (*subsup)++;
        } else if (tchar=='*') {
            //r->WriteText("\u00b7");   /* Does not work on Ubuntu :( */
            r->WriteText(wxString(1,'.'));
            r->SetSelection(r->GetCaretPosition(), r->GetCaretPosition()+1);
            r->ApplyTextEffectToSelection(wxTEXT_ATTR_EFFECT_SUPERSCRIPT);
            r->SetSelection(0,0);
            subscript = false;
            (*subsup)++;
        } else {
            r->WriteText(wxString(1,tchar));
            if (isalpha(tchar)) {
                subscript = true;
            }
        }
        inserted++;
    }

    return inserted;
}


void MainFrame::write_table_row_chemf(sqlite3_stmt *stmt, wxString name, int ndx) {
    wxRichTextCtrl *r = mineral_view;
    wxString s1 = wxString(sqlite3_column_text(stmt, ndx+0*13), wxConvUTF8);
    wxString s2 = wxString(sqlite3_column_text(stmt, ndx+1*13), wxConvUTF8);
    wxString s3 = wxString(sqlite3_column_text(stmt, ndx+2*13), wxConvUTF8);
    wxString s4 = wxString(sqlite3_column_text(stmt, ndx+3*13), wxConvUTF8);
    if (s1=="No" || s1=="no") s1=wxString("");
    if (s2=="No" || s2=="no") s2=wxString("");
    if (s3=="No" || s3=="no") s3=wxString("");
    if (s4=="No" || s4=="no") s4=wxString("");
    int l1 = s1.length();
    int l2 = s2.length();
    int l3 = s3.length();
    int l4 = s4.length();
    int subsup=0;
    if (l1+l2+l3+l4>0) {
        r->BeginBold(); r->WriteText(name+": "); r->EndBold();
        l1 = write_chemf(r, s1, &subsup);
        r->WriteText(std::string(guess_column_width(stmt, 0)-l1+(int)(subsup/3), ' '));
        subsup-=3*(int)(subsup/3);
        if (l2+l3+l4>0) {
            l2 = write_chemf(r, s2, &subsup);
            r->WriteText(std::string(guess_column_width(stmt, 1)-l2+(int)(subsup/3), ' '));
            subsup-=3*(int)(subsup/3);
        }
        if (l3+l4>0) {
            l3 = write_chemf(r, s3, &subsup);
            r->WriteText(std::string(guess_column_width(stmt, 2)-l3+(int)(subsup/3), ' '));
            subsup-=3*(int)(subsup/3);
        }
        if (l4>0) {
            l4 = write_chemf(r, s4, &subsup);
            r->WriteText(std::string(guess_column_width(stmt, 3)-l4+(int)(subsup/3), ' '));
            subsup-=3*(int)(subsup/3);
        }
        r->Newline();
    }
}


void MainFrame::write_link_row(sqlite3_stmt *stmt) {
    int ndx = 9;
    wxRichTextAttr urlStyle;
    urlStyle.SetTextColour(*wxBLUE);
    urlStyle.SetFontUnderlined(true);
    wxRichTextCtrl *r = mineral_view;
    wxString s1 = wxString(sqlite3_column_text(stmt, ndx+0*13), wxConvUTF8);
    wxString s2 = wxString(sqlite3_column_text(stmt, ndx+1*13), wxConvUTF8);
    wxString s3 = wxString(sqlite3_column_text(stmt, ndx+2*13), wxConvUTF8);
    wxString s4 = wxString(sqlite3_column_text(stmt, ndx+3*13), wxConvUTF8);
    if (s1=="No" || s1=="no") s1=wxString("");
    if (s2=="No" || s2=="no") s2=wxString("");
    if (s3=="No" || s3=="no") s3=wxString("");
    if (s4=="No" || s4=="no") s4=wxString("");
    int l1 = s1.length();
    int l2 = s2.length();
    int l3 = s3.length();
    int l4 = s4.length();
    std::string s1p = url_encode(s1.ToStdString());
    std::string s2p = url_encode(s2.ToStdString());
    std::string s3p = url_encode(s3.ToStdString());
    std::string s4p = url_encode(s4.ToStdString());
    if (l1+l2+l3+l4>0) {
        r->BeginBold(); r->WriteText("                 "); r->EndBold();
        r->BeginStyle(urlStyle); r->BeginURL(wxString("http://www.mindat.org/show.php?name=")+s1p);r->WriteText("M");r->EndURL();r->EndStyle();
        r->WriteText(" ");
        r->BeginStyle(urlStyle); r->BeginURL(wxString("https://rruff.info/")+s1p);r->WriteText("R");r->EndURL();r->EndStyle();
        r->WriteText(std::string(guess_column_width(stmt, 0)-3, ' '));
        if (l2+l3+l4>0) {
            r->BeginStyle(urlStyle);r->BeginURL(wxString("http://www.mindat.org/show.php?name=")+s2p);r->WriteText("M");r->EndURL();r->EndStyle();
            r->WriteText(" ");
            r->BeginStyle(urlStyle); r->BeginURL(wxString("https://rruff.info/")+s2p);r->WriteText("R");r->EndURL();r->EndStyle();
            r->WriteText(std::string(guess_column_width(stmt, 1)-3, ' '));
        }
        if (l3+l4>0) {
            r->BeginStyle(urlStyle);r->BeginURL(wxString("http://www.mindat.org/show.php?name=")+s3p);r->WriteText("M");r->EndURL();r->EndStyle();
            r->WriteText(" ");
            r->BeginStyle(urlStyle); r->BeginURL(wxString("https://rruff.info/")+s3p);r->WriteText("R");r->EndURL();r->EndStyle();
            r->WriteText(std::string(guess_column_width(stmt, 2)-3, ' '));
        }
        if (l4>0) {
            r->BeginStyle(urlStyle);r->BeginURL(wxString("http://www.mindat.org/show.php?name=")+s4p);r->WriteText("M");r->EndURL();r->EndStyle();
            r->WriteText(" ");
            r->BeginStyle(urlStyle); r->BeginURL(wxString("https://rruff.info/")+s4p);r->WriteText("R");r->EndURL();r->EndStyle();
            r->WriteText(std::string(guess_column_width(stmt, 3)-3, ' '));
        }
        r->Newline();
    }
}


void MainFrame::ReadData(std::string uid) {

    if (db_file_path.empty()) return;

    wxRichTextAttr urlStyle;
    urlStyle.SetTextColour(*wxBLUE);
    urlStyle.SetFontUnderlined(true);

    fs::path basepath;

    basepath = fs::path(db_file_path).remove_filename() / "data";
    if (!fs::is_directory(basepath)) return;

    basepath = fs::path(db_file_path).remove_filename() / "data" / uid;
    if (!fs::is_directory(basepath)) {
        std::string prefix = uid + " ";
        basepath = fs::path();
        for (const auto & entry : fs::directory_iterator(fs::path(db_file_path).remove_filename() / "data")) {
            if (entry.is_directory() && strncmp(entry.path().filename().c_str(), prefix.c_str(), prefix.size())==0) {
                basepath = entry.path();
                break;
            }
        }
        if (basepath.empty()) return;
    }

    std::vector<std::string> formats = { ".png", ".jpg", ".jpeg", ".gif", ".tiff", ".tif" };
    wxRichTextCtrl *r = mineral_view;
    int width, height;
    float scale;
    r->Newline();

    r->BeginBold(); r->WriteText("Figures & Data :"); r->EndBold();
    r->Newline();
    r->Newline();

    std::vector<fs::directory_entry> files;
    std::copy(fs::directory_iterator(basepath), fs::directory_iterator(), std::back_inserter(files));
    std::sort(files.begin(), files.end());

    for (const auto & entry : files) {
        if (entry.path().filename().string().at(0) == '.') continue;
        std::string ext = std::string(entry.path().extension().c_str());
        if (std::find(formats.begin(), formats.end(), ext) != formats.end()) {
            wxImage image = wxImage(wxString(entry.path().string()));
            width = image.GetWidth();
            height = image.GetHeight();
            if (width>400 || height>400) {
                scale = 400.0/std::max(width, height);
                image.Rescale(int(width*scale), int(height*scale));
            }
            r->WriteText("    ");
            r->WriteImage(image);
        } else {
            r->WriteText("    ");
            r->WriteText(wxString("file: ")+wxString(entry.path().filename()));
        }
        r->WriteText("    ");
        r->BeginStyle(urlStyle);r->BeginURL(wxString("file://")+url_encode(entry.path()));r->WriteText(wxString("Open original"));r->EndURL();r->EndStyle();
        r->Newline();
        r->Newline();
    }
}

void MainFrame::db_initialize() {
    int ret;
    char *errmsg;
    ret = sqlite3_open(":memory:", &db);
    if (ret!=SQLITE_OK) {
        wxLogMessage("Can't open database: %s", sqlite3_errmsg(db));
        return;
    }
    const char *query_minerals_create = "CREATE TABLE MINERALS (MINID INTEGER PRIMARY KEY, NAME TEXT NOT NULL, LOCALITY TEXT, LOCID_MNDAT TEXT, SIZE TEXT, WEIGHT TEXT, ACQUISITION TEXT, COLLECTION TEXT, VALUE TEXT, S1_SPECIES TEXT, S1_CLASS TEXT, S1_CHEMF TEXT, S1_COLOR TEXT, S1_FLSW TEXT, S1_FLMW TEXT, S1_FLLW TEXT, S1_FL405 TEXT, S1_PHSW TEXT, S1_PHMW TEXT, S1_PHLW TEXT, S1_PH405 TEXT, S1_TENEBR TEXT, S2_SPECIES TEXT, S2_CLASS TEXT, S2_CHEMF TEXT, S2_COLOR TEXT, S2_FLSW TEXT, S2_FLMW TEXT, S2_FLLW TEXT, S2_FL405 TEXT, S2_PHSW TEXT, S2_PHMW TEXT, S2_PHLW TEXT, S2_PH405 TEXT, S2_TENEBR TEXT, S3_SPECIES TEXT, S3_CLASS TEXT, S3_CHEMF TEXT, S3_COLOR TEXT, S3_FLSW TEXT, S3_FLMW TEXT, S3_FLLW TEXT, S3_FL405 TEXT, S3_PHSW TEXT, S3_PHMW TEXT, S3_PHLW TEXT, S3_PH405 TEXT, S3_TENEBR TEXT, S4_SPECIES TEXT, S4_CLASS TEXT, S4_CHEMF TEXT, S4_COLOR TEXT, S4_FLSW TEXT, S4_FLMW TEXT, S4_FLLW TEXT, S4_FL405 TEXT, S4_PHSW TEXT, S4_PHMW TEXT, S4_PHLW TEXT, S4_PH405 TEXT, S4_TENEBR TEXT, RADIOACT TEXT, COMMENTS TEXT );";
    ret = sqlite3_exec(db, query_minerals_create, NULL, 0, &errmsg);
    if (ret!=SQLITE_OK) {
        wxLogMessage("SQL error: %s", errmsg);
        sqlite3_free(errmsg);
        sqlite3_close(db);
        db=NULL;
        return;
    }
    const char *query_settings_create = "CREATE TABLE SETTINGS (VERSION_MAJOR INT, VERSION_MINOR INT)";
    ret = sqlite3_exec(db, query_settings_create, NULL, 0, &errmsg);
    if (ret!=SQLITE_OK) {
        wxLogMessage("SQL error: %s", errmsg);
        sqlite3_free(errmsg);
        sqlite3_close(db);
        db=NULL;
        return;
    }
    const char *query_set_version = "INSERT INTO SETTINGS (VERSION_MAJOR, VERSION_MINOR) VALUES (" VERSION_MAJOR ", " VERSION_MINOR ");";
    ret = sqlite3_exec(db, query_set_version, NULL, 0, &errmsg);
    if (ret!=SQLITE_OK) {
        wxLogMessage("SQL error: %s", errmsg);
        sqlite3_free(errmsg);
        sqlite3_close(db);
        db=NULL;
        return;
    }
    return;
}

void MainFrame::populate_listbox_evt(wxCommandEvent& event) {
    populate_listbox();
}

void MainFrame::populate_listbox() {

    mineral_listbox->Clear();
    int orderby = mineral_orderby->GetSelection();
    std::string searchstr = str_tolower(mineral_search->GetValue().ToStdString());
    int country_id = mineral_country->GetSelection();
    wxString country;
    if (country_id!=wxNOT_FOUND) {
        country = mineral_country->GetString(country_id);
    }
    if (country=="Any") country="";
    const char *query;
    if (orderby==1) {
        query = "SELECT MINID,NAME,LOCALITY FROM MINERALS ORDER BY NAME";
    } else {
        query = "SELECT MINID,NAME,LOCALITY FROM MINERALS ORDER BY MINID";
    }
    int ret;
    sqlite3_stmt *stmt;
    ret = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    if (ret!=SQLITE_OK) {
        wxLogMessage("error: %s", sqlite3_errmsg(db));
        return;
    }
    while ((ret=sqlite3_step(stmt))==SQLITE_ROW) {
        wxString name = wxString(sqlite3_column_text(stmt, 1), wxConvUTF8) + wxString(" [") + wxString(sqlite3_column_text(stmt, 0), wxConvUTF8) + wxString("]");
        wxString locality = wxString(sqlite3_column_text(stmt, 2), wxConvUTF8);
        if (str_tolower(name.ToStdString()).find(searchstr)!=std::string::npos && 
                locality.find(country)!=std::string::npos) {
            mineral_listbox->Append(name);
        }
    }
    if (ret!=SQLITE_DONE) {
        wxLogMessage("error: ", sqlite3_errmsg(db));
    }
    sqlite3_finalize(stmt);

    return;
}

void MainFrame::populate_country_filter() {
    mineral_country->Clear();
    mineral_country->Append("Any");
    std::string errmsg = "";
    std::vector<std::string> countries = db_get_country_list(db, &errmsg);
    for(const auto& value: countries) {
        mineral_country->Append(value);
    }
    return;
}

void MainFrame::update_gui() {
    populate_country_filter();
    populate_listbox();
    return;
}

void MainFrame::import_csv(wxCommandEvent& event) {
    /* Check if some db is already opened and warn the user */
    if (db) {
        wxMessageDialog *dial = new wxMessageDialog(NULL, "You have already an open database. By importing from a CSV file any duplicate mineral id will overwtie existing ones. Do you want to continue?", "Question", wxYES_NO|wxNO_DEFAULT|wxICON_QUESTION);
        if (dial->ShowModal()!=wxID_YES) {
            return;
        }
    } else {
        db_initialize();
    }

    /* Get the filename of the db to read */
    wxFileDialog openFileDialog(this, "Import CSV file", wxEmptyString, "minerals.csv",  "CSV files (*.csv)|*.csv", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal()==wxID_CANCEL) {
        return;
    }
    wxString fname = openFileDialog.GetPath();

    std::string errmsg;
    bool success = ::import_csv(db, fname.ToStdString(), &errmsg);
    if (!success) {
        wxLogMessage(wxString("Import failed! ") + errmsg);
    }
    update_gui();
}

void MainFrame::export_csv(wxCommandEvent& event) {
    if (!db) {
        wxLogMessage("Nothing to save!");
        return;
    }
    wxFileDialog saveFileDialog(this, "Export CSV file", wxEmptyString, "minerals.csv",  "CSV files (*.csv)|*.csv", wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
    if (saveFileDialog.ShowModal() == wxID_CANCEL) {
        return;
    }
    std::string errmsg = "";
    bool success = ::export_csv(db, saveFileDialog.GetPath().ToStdString(), &errmsg);
    if (!success) {
        wxLogMessage(wxString(errmsg));
    }
}

fs::path MainFrame::get_config_dirname() {
    fs::path configdir;
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    configdir = fs::path(std::getenv("APPDATA")) / "mineralapp";
    #else
    configdir = fs::path(std::getenv("HOME")) / ".mineralapp";
    #endif
    return configdir;
}

void MainFrame::read_config() {
    fs::path configdir = get_config_dirname();
    if (configdir.empty()) return;
    if (!fs::exists(configdir)) return;
    std::ifstream configfile;
    configfile.open(configdir/"config.txt");
    std::string last_open;
    std::string line;
    while (std::getline(configfile, line)) {
        std::istringstream is_line(line);
        std::string key;
        if (std::getline(is_line, key, '=')) {
            std::string value;
            if (std::getline(is_line, value)) {
                if (key.compare(std::string("last_open"))==0) {
                    last_open = value;
                }
            }
        }
    }
    configfile.close();
    if (!last_open.empty()) open_dbfile(last_open);
}


void MainFrame::write_config() {
    fs::path configdir = get_config_dirname();
    if (configdir.empty()) return;
    if (!fs::exists(configdir)) fs::create_directory(configdir);
    std::ofstream configfile;
    configfile.open(fs::path(configdir)/"config.txt");
    configfile << "last_open=" << db_file_path << std::endl;
    configfile.close();
}


