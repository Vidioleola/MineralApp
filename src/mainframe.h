
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


class MainFrame: public wxFrame {
    public:
        MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
        void populate_listbox();
        void draw_mineral_view(int uid);
    private:
        wxMenu *menuFile;
        wxMenu *menuMineral;
        wxMenu *menuHelp;
        wxMenuBar *menuBar;
        wxListBox *mineral_listbox;
        wxTextCtrl *mineral_search;
        wxRadioBox *mineral_orderby;
        wxRichTextCtrl *mineral_view;
        void OnNewMineral(wxCommandEvent& event);
        void OnModifyMineral(wxCommandEvent& event);
        void OnDuplicateMineral(wxCommandEvent& event);
        void OnDeleteMineral(wxCommandEvent& event);
        void OnSelectMineral(wxCommandEvent& event);
        void OnOpen(wxCommandEvent& event);
        void OnSave(wxCommandEvent& event);
        void OnClose(wxCommandEvent& event);
        void import_csv(wxCommandEvent& event);
        void export_csv(wxCommandEvent& event);
        void OnExit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnHelp(wxCommandEvent& event);
        void OnURL(wxTextUrlEvent& event);
        void populate_listbox_evt(wxCommandEvent& event);
        sqlite3 *db;
        void db_initialize();
        void open_dbfile(std::string);
        void write_table_row(sqlite3_stmt *stmt, wxString name, int ndx);
        void write_table_row_chemf(sqlite3_stmt *stmt, wxString name, int ndx);
        void write_link_row(sqlite3_stmt *stmt);
        void ReadData(std::string uid);
        int get_minid_from_listbox();
        std::string db_file_path;
        fs::path get_config_dirname();
        void read_config();
        void write_config();
        wxDECLARE_EVENT_TABLE();
    };

enum {
    ID_NewMineral = wxID_HIGHEST + 1,
    ID_ModifyMineral,
    ID_DuplicateMineral,
    ID_DeleteMineral,
    ID_SelectMineral,
    ID_SearchMineral,
    ID_OrderByMineral,
    ID_ExportCSV,
    ID_ImportCSV,
    ID_HIDEVALUE,
};

