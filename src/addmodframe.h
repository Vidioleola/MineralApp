

class AddModFrame: public wxFrame {
    public:
        AddModFrame(MainFrame *parent, const wxString& title, sqlite3 *maindb, int uid);
    private:
        MainFrame *main_window;
        sqlite3 *db;
        int modifying;

        wxTextCtrl *entry_minid;
        wxTextCtrl *entry_name;
        wxTextCtrl *entry_locality;
        wxTextCtrl *entry_locid;
        wxTextCtrl *entry_size;
        wxTextCtrl *entry_weight;
        wxTextCtrl *entry_acquisition;
        wxTextCtrl *entry_collection;
        wxTextCtrl *entry_value;

        wxTextCtrl *entry_s1_species,  *entry_s2_species,  *entry_s3_species,  *entry_s4_species;
        wxTextCtrl *entry_s1_class,    *entry_s2_class,    *entry_s3_class,    *entry_s4_class;
        wxTextCtrl *entry_s1_chemf,    *entry_s2_chemf,    *entry_s3_chemf,    *entry_s4_chemf;
        wxTextCtrl *entry_s1_color,    *entry_s2_color,    *entry_s3_color,    *entry_s4_color;
        wxTextCtrl *entry_s1_flsw,     *entry_s2_flsw,     *entry_s3_flsw,     *entry_s4_flsw;
        wxTextCtrl *entry_s1_flmw,     *entry_s2_flmw,     *entry_s3_flmw,     *entry_s4_flmw;
        wxTextCtrl *entry_s1_fllw,     *entry_s2_fllw,     *entry_s3_fllw,     *entry_s4_fllw;
        wxTextCtrl *entry_s1_fl405,    *entry_s2_fl405,    *entry_s3_fl405,    *entry_s4_fl405;
        wxTextCtrl *entry_s1_phsw,     *entry_s2_phsw,     *entry_s3_phsw,     *entry_s4_phsw;
        wxTextCtrl *entry_s1_phmw,     *entry_s2_phmw,     *entry_s3_phmw,     *entry_s4_phmw;
        wxTextCtrl *entry_s1_phlw,     *entry_s2_phlw,     *entry_s3_phlw,     *entry_s4_phlw;
        wxTextCtrl *entry_s1_ph405,    *entry_s2_ph405,    *entry_s3_ph405,    *entry_s4_ph405;
        wxTextCtrl *entry_s1_tenebr,   *entry_s2_tenebr,   *entry_s3_tenebr,   *entry_s4_tenebr;
        wxTextCtrl *entry_radioact;

        wxTextCtrl *entry_comments;

        void PopulateForm(int uid);
        void OnSave(wxCommandEvent& event);
        void OnCancel(wxCommandEvent& event);
        wxDECLARE_EVENT_TABLE();
    };

enum {
    ID_AddMod_Save = wxID_HIGHEST + 1,
    ID_AddMod_Cancel,
};


