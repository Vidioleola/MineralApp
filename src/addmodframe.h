

class AddModFrame: public wxFrame {
    public:
        AddModFrame(MainFrame *parent, const wxString& title, sqlite3 *maindb, int uid);
    private:
        MainFrame *main_window;
        sqlite3 *db;
        int modifying;

        wxTextCtrl *entry_catnum;
        wxTextCtrl *entry_minid;
        wxTextCtrl *entry_name;
        wxTextCtrl *entry_locality;
        wxTextCtrl *entry_locid;
        wxTextCtrl *entry_length;
        wxTextCtrl *entry_width;
        wxTextCtrl *entry_height;
        wxComboBox *entry_size_units;
        wxComboBox *entry_size_category;
        wxTextCtrl *entry_weight;
        wxComboBox *entry_weight_units;
        wxComboBox *entry_acquisition_day;
        wxComboBox *entry_acquisition_month;
        wxTextCtrl *entry_acquisition_year;
        wxTextCtrl *entry_acquisition_source;
        wxComboBox *entry_deaccessioned_day;
        wxComboBox *entry_deaccessioned_month;
        wxTextCtrl *entry_deaccessioned_year;
        wxTextCtrl *entry_deaccessioned_to;
        wxTextCtrl *entry_collection;
        wxTextCtrl *entry_value;
        wxTextCtrl *entry_price;
        wxCheckBox *entry_selfcollected;

        wxTextCtrl *entry_s1_species,  *entry_s2_species,  *entry_s3_species,  *entry_s4_species;
        wxTextCtrl *entry_s1_variety,  *entry_s2_variety,  *entry_s3_variety,  *entry_s4_variety;
        wxTextCtrl *entry_s1_class,    *entry_s2_class,    *entry_s3_class,    *entry_s4_class;
        wxTextCtrl *entry_s1_chemf,    *entry_s2_chemf,    *entry_s3_chemf,    *entry_s4_chemf;
        wxTextCtrl *entry_s1_color,    *entry_s2_color,    *entry_s3_color,    *entry_s4_color;
        wxComboBox *entry_s1_transp,   *entry_s2_transp,   *entry_s3_transp,   *entry_s4_transp;
        wxTextCtrl *entry_s1_habit,    *entry_s2_habit,    *entry_s3_habit,    *entry_s4_habit;
        wxTextCtrl *entry_s1_flsw,     *entry_s2_flsw,     *entry_s3_flsw,     *entry_s4_flsw;
        wxTextCtrl *entry_s1_flmw,     *entry_s2_flmw,     *entry_s3_flmw,     *entry_s4_flmw;
        wxTextCtrl *entry_s1_fllw,     *entry_s2_fllw,     *entry_s3_fllw,     *entry_s4_fllw;
        wxTextCtrl *entry_s1_fl405,    *entry_s2_fl405,    *entry_s3_fl405,    *entry_s4_fl405;
        wxTextCtrl *entry_s1_phsw,     *entry_s2_phsw,     *entry_s3_phsw,     *entry_s4_phsw;
        wxTextCtrl *entry_s1_phmw,     *entry_s2_phmw,     *entry_s3_phmw,     *entry_s4_phmw;
        wxTextCtrl *entry_s1_phlw,     *entry_s2_phlw,     *entry_s3_phlw,     *entry_s4_phlw;
        wxTextCtrl *entry_s1_ph405,    *entry_s2_ph405,    *entry_s3_ph405,    *entry_s4_ph405;
        wxTextCtrl *entry_s1_tenebr,   *entry_s2_tenebr,   *entry_s3_tenebr,   *entry_s4_tenebr;
        wxTextCtrl *entry_s1_tribo,    *entry_s2_tribo,    *entry_s3_tribo,    *entry_s4_tribo;
        wxTextCtrl *entry_radioact;

        wxTextCtrl *entry_description;
        wxTextCtrl *entry_notes;
        wxTextCtrl *entry_owners;

        void PopulateForm(int uid);
        void OnSave(wxCommandEvent& event);
        void OnCancel(wxCommandEvent& event);
        wxDECLARE_EVENT_TABLE();
    };

enum {
    ID_AddMod_Save = wxID_HIGHEST + 1,
    ID_AddMod_Cancel,
};


