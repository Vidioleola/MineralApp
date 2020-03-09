
#include <vector>
#include <algorithm>
#include <regex>

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/richtext/richtextctrl.h>

#include <sqlite3.h> 

#if defined(__cplusplus) && __cplusplus >= 201703L && defined(__has_include)
  #if __has_include(<filesystem>)
    #define GHC_USE_STD_FS
  #endif
#endif
#if defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 101500
  #undef GHC_USE_STD_FS
#endif
#ifdef GHC_USE_STD_FS
  #include <filesystem>
  namespace fs = std::filesystem;
#else
  #include "filesystem.hpp"
  namespace fs = ghc::filesystem;
#endif

#include "mainframe.h"
#include "addmodframe.h"

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(ID_NewMineral,       MainFrame::OnNewMineral)
    EVT_MENU(ID_ModifyMineral,    MainFrame::OnModifyMineral)
    EVT_MENU(ID_DuplicateMineral, MainFrame::OnDuplicateMineral)
    EVT_MENU(ID_DeleteMineral,    MainFrame::OnDeleteMineral)
    EVT_MENU(wxID_OPEN,  MainFrame::OnOpen)
    EVT_MENU(wxID_SAVE,  MainFrame::OnSave)
    EVT_MENU(wxID_EXIT,  MainFrame::OnExit)
    EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
    EVT_LISTBOX(ID_SelectMineral, MainFrame::OnSelectMineral)
    EVT_TEXT_URL(wxID_ANY, MainFrame::OnURL)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
        : wxFrame(NULL, wxID_ANY, title, pos, size) {
    /* Initialize DB */
    db = NULL;
    //wxInitAllImageHandlers();
    wxImage::AddHandler(new wxPNGHandler);
    wxImage::AddHandler(new wxJPEGHandler);
    wxImage::AddHandler(new wxGIFHandler);
    wxImage::AddHandler(new wxTIFFHandler);
    /* Menu Bar */
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(wxID_OPEN);
    menuFile->Append(wxID_SAVE);
    //menuFile->Append(wxID_COPY);
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
    wxMenu *menuMineral = new wxMenu;
    menuMineral->Append(ID_NewMineral, "&Add", "Add a new mineral to the database");
    menuMineral->Append(ID_ModifyMineral, "&Modify", "Modify the selected mineral");
    menuMineral->Append(ID_DuplicateMineral, "&Duplicate", "Duplicate the selected mineral");
    menuMineral->Append(ID_DeleteMineral, "&Delete", "Delete the selected mineral");
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File" );
    menuBar->Append(menuMineral, "&Mineral" );
    menuBar->Append(menuHelp, "&Help" );
    SetMenuBar( menuBar );
    /* Status Bar */
    CreateStatusBar();
    SetStatusText("Welcome to MineralApp!" );
    /* ListBox */
    mineral_listbox = new wxListBox(this, ID_SelectMineral);
    /* viewbox */
    mineral_view = new wxRichTextCtrl(this, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY, wxDefaultValidator, wxTextCtrlNameStr);
    /* Horizontal sizer -- split listbox and viewbox */
    wxBoxSizer *hsizer = new wxBoxSizer(wxHORIZONTAL);
    hsizer->Add(mineral_listbox, 1, wxEXPAND | wxALL, 5);
    hsizer->Add(mineral_view, 5, wxEXPAND | wxALL, 5);
    hsizer->SetSizeHints(this);
    SetSizerAndFit(hsizer);
}

void MainFrame::OnOpen(wxCommandEvent& event) {
    int ret;
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
    wxString fname2 = openFileDialog.GetPath();
    const char *fname = static_cast<const char*>(fname2.c_str());
    /* Open the db from file */
    sqlite3 *db_tmp;
    ret = sqlite3_open(fname, &db_tmp);
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

    db_file_path = fs::path(fname);

    /* Populate the mineral listbox */
    populate_listbox();
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

void MainFrame::OnExit(wxCommandEvent& event) {
    Close(true);
}

void MainFrame::OnAbout(wxCommandEvent& event) {
    const char *msg =
        "MineralApp is a small and simple application to create a database of "
        "your minerals. You can add your mineral collection, storing any details "
        "you are interested in, helping you (hopefully...) to keep your mineral "
        "collection well organized!";
    wxMessageBox(msg, "MineralApp v" VERSION, wxOK | wxICON_INFORMATION);
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

void MainFrame::OnModifyMineral(wxCommandEvent& event) {
    if (!db) {
        db_initialize();
    }
    if (!db) {
        wxMessageBox("DB initialization failed! Sorry, try to close everything and retry...");
        return;
    }
    /* Get the id of the selected mineral */
    int selected = mineral_listbox->GetSelection();
    if (selected==wxNOT_FOUND) {
        wxLogMessage("Please, select a mineral from the left panel.");
        return;
    }
    wxString label = mineral_listbox->GetString(selected);
    int ndxi = label.rfind('[');
    int ndxf = label.rfind(']');
    int minid;
    int ret;
    ret = sscanf(label.substr(ndxi,ndxf).c_str(), "[%d]", &minid);
    if (ret!=1) {
        wxLogMessage("Please, select a mineral from the left panel.");
        return;
    }
    /* Open form */
    AddModFrame *frame = new AddModFrame(this, "Add new mineral", db, minid);
    frame->Show();
}

void MainFrame::OnDuplicateMineral(wxCommandEvent& event) {
    wxLogMessage("Duplicate mineral not implemented yet!");
}

void MainFrame::OnDeleteMineral(wxCommandEvent& event) {
    wxLogMessage("Delete mineral not implemented yet!");
}

void MainFrame::OnSelectMineral(wxCommandEvent& event) {
    int selected = mineral_listbox->GetSelection();
    if (selected==wxNOT_FOUND) {
        return;
    }
    wxString label = mineral_listbox->GetString(selected);
    int ndx = label.rfind('[');
    int minid;
    int ret;
    ret = sscanf(std::string(label.ToAscii()).c_str()+ndx, "[%d]", &minid);
    if (ret!=1) {
        wxLogMessage("Impossible to parse id :(");
        return;
    }
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
    //wxFont monospace = wxFont(wxDEFAULT, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    wxFont monospace = wxFont(wxFontInfo().FaceName("Andale Mono"));
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
    r->BeginBold(); r->WriteText("Value          : "); r->EndBold(); if (value.length()>0) r->WriteText(value); r->Newline();
    r->Newline();

    /* Table of species */
    ndx = 9;
    write_table_row(stmt, "Species        ", ndx); ndx+=1;
    write_table_row(stmt, "Class          ", ndx); ndx+=1;
    write_table_row(stmt, "Chem. Formula  ", ndx); ndx+=1;
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
    int l1 = strlen(s1);
    int l2 = strlen(s2);
    int l3 = strlen(s3);
    int l4 = strlen(s4);
    if (l1+l2+l3+l4>0) {
        r->BeginBold(); r->WriteText(name+": "); r->EndBold();
        r->WriteText(s1+std::string(guess_column_width(stmt, 0)-s1.length(), ' '));
        if (l2+l3+l4>0) r->WriteText(s2+std::string(guess_column_width(stmt, 1)-s2.length(), ' '));
        if (l3+l4>0) r->WriteText(s3+std::string(guess_column_width(stmt, 2)-s3.length(), ' '));
        if (l4>0) r->WriteText(s4+std::string(guess_column_width(stmt, 3)-s4.length(), ' '));
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
    int l1 = strlen(s1);
    int l2 = strlen(s2);
    int l3 = strlen(s3);
    int l4 = strlen(s4);
    if (l1+l2+l3+l4>0) {
        r->BeginBold(); r->WriteText("                 "); r->EndBold();
        r->BeginStyle(urlStyle); r->BeginURL(wxString("http://www.mindat.org/show.php?name=")+s1);r->WriteText("MINDAT");r->EndURL();r->EndStyle();
        r->WriteText(std::string(guess_column_width(stmt, 0)-6, ' '));
        if (l2+l3+l4>0) {
            r->BeginStyle(urlStyle);r->BeginURL(wxString("http://www.mindat.org/show.php?name=")+s2);r->WriteText("MINDAT");r->EndURL();r->EndStyle();
            r->WriteText(std::string(guess_column_width(stmt, 1)-6, ' '));
        }
        if (l3+l4>0) {
            r->BeginStyle(urlStyle);r->BeginURL(wxString("http://www.mindat.org/show.php?name=")+s3);r->WriteText("MINDAT");r->EndURL();r->EndStyle();
            r->WriteText(std::string(guess_column_width(stmt, 2)-6, ' '));
        }
        if (l4>0) {
            r->BeginStyle(urlStyle);r->BeginURL(wxString("http://www.mindat.org/show.php?name=")+s4);r->WriteText("MINDAT");r->EndURL();r->EndStyle();
            r->WriteText(std::string(guess_column_width(stmt, 3)-6, ' '));
        }
        r->Newline();
    }
}


void MainFrame::ReadData(std::string uid) {

    wxRichTextAttr urlStyle;
    urlStyle.SetTextColour(*wxBLUE);
    urlStyle.SetFontUnderlined(true);

    fs::path basepath = fs::path(db_file_path).remove_filename() / "data" / uid;
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
        std::string temppath = std::regex_replace(entry.path().string(), std::regex(" "), "%20");
        r->BeginStyle(urlStyle);r->BeginURL(wxString("file://")+wxString(temppath));r->WriteText(wxString("Open original"));r->EndURL();r->EndStyle();
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

void MainFrame::populate_listbox() {
    mineral_listbox->Clear();
    const char *query = "SELECT MINID,NAME FROM MINERALS";
    int ret;
    sqlite3_stmt *stmt;
    ret = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    if (ret!=SQLITE_OK) {
        wxLogMessage("error: %s", sqlite3_errmsg(db));
        return;
    }
    while ((ret=sqlite3_step(stmt))==SQLITE_ROW) {
        wxString name = wxString(sqlite3_column_text(stmt, 1), wxConvUTF8) + wxString(" [") + wxString(sqlite3_column_text(stmt, 0), wxConvUTF8) + wxString("]");
        mineral_listbox->Append(name);
    }
    if (ret!=SQLITE_DONE) {
        wxLogMessage("error: ", sqlite3_errmsg(db));
    }
    sqlite3_finalize(stmt);
    return;
}

