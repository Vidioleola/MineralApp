
class MainFrame: public wxFrame {
    public:
        MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
        void populate_listbox();
        void draw_mineral_view(int uid);
    private:
        wxListBox *mineral_listbox;
        wxRichTextCtrl *mineral_view;
        void OnNewMineral(wxCommandEvent& event);
        void OnModifyMineral(wxCommandEvent& event);
        void OnDuplicateMineral(wxCommandEvent& event);
        void OnDeleteMineral(wxCommandEvent& event);
        void OnSelectMineral(wxCommandEvent& event);
        void OnOpen(wxCommandEvent& event);
        void OnSave(wxCommandEvent& event);
        void OnExit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnURL(wxTextUrlEvent& event);
        sqlite3 *db;
        void db_initialize();
        void write_table_row(sqlite3_stmt *stmt, wxString name, int ndx);
        void write_link_row(sqlite3_stmt *stmt);
        void ReadData(std::string uid);
        int get_minid_from_listbox();
        std::string db_file_path;
        wxDECLARE_EVENT_TABLE();
    };

enum {
    ID_NewMineral = wxID_HIGHEST + 1,
    ID_ModifyMineral,
    ID_DuplicateMineral,
    ID_DeleteMineral,
    ID_SelectMineral,
};

