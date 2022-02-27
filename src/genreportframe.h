
class GenReportFrame: public wxFrame {
    public:
        GenReportFrame(wxFrame *parent, const wxString& title, sqlite3 *maindb, int uid, std::string db_file_path);
    private:
        sqlite3 *db;
        wxRadioButton *rb1a, *rb1b;
        wxRadioButton *rb2a, *rb2b;
        int selected_uid;
        std::string db_path;
        void OnSave(wxCommandEvent& event);
        void OnCancel(wxCommandEvent& event);
        wxDECLARE_EVENT_TABLE();
    };

enum {
    ID_GenReport_Save = wxID_HIGHEST + 1,
    ID_GenReport_Cancel,
};

