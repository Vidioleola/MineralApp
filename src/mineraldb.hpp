
#ifndef MINERALDB
#define MINERALDB

#include <vector>
#include <string>
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


static std::vector<std::string> data_header = {
    "MINID", "NAME", "LOCALITY", "LOCID_MNDAT", "SIZE", "WEIGHT", "ACQUISITION", "COLLECTION", "VALUE",
    "S1_SPECIES", "S1_CLASS", "S1_CHEMF", "S1_COLOR", "S1_FLSW", "S1_FLMW", "S1_FLLW", "S1_FL405", "S1_PHSW", "S1_PHMW", "S1_PHLW", "S1_PH405", "S1_TENEBR",
    "S2_SPECIES", "S2_CLASS", "S2_CHEMF", "S2_COLOR", "S2_FLSW", "S2_FLMW", "S2_FLLW", "S2_FL405", "S2_PHSW", "S2_PHMW", "S2_PHLW", "S2_PH405", "S2_TENEBR",
    "S3_SPECIES", "S3_CLASS", "S3_CHEMF", "S3_COLOR", "S3_FLSW", "S3_FLMW", "S3_FLLW", "S3_FL405", "S3_PHSW", "S3_PHMW", "S3_PHLW", "S3_PH405", "S3_TENEBR",
    "S4_SPECIES", "S4_CLASS", "S4_CHEMF", "S4_COLOR", "S4_FLSW", "S4_FLMW", "S4_FLLW", "S4_FL405", "S4_PHSW", "S4_PHMW", "S4_PHLW", "S4_PH405", "S4_TENEBR",
    "RADIOACT", "COMMENTS"
};

void db_initialize(sqlite3 **db, std::string *errmsg);
void db_open(sqlite3 **db, std::string fname, std::string *errmsg);
void db_save(sqlite3 *db, std::string fname, std::string *errmsg);
void db_close(sqlite3 *db, std::string *errmsg);

void db_delete_mineral(sqlite3 *db, int minid, std::string *errmsg);
void db_duplicate_mineral(sqlite3 *db, int minid, std::string *errmsg);

int db_addmod_mineral(sqlite3 *db, std::vector<std::string> data, int minid_mod, std::string *errmsg);
std::vector<std::string> db_get_data(sqlite3 *db, int minid, std::string *errmsg);
std::string db_get_field(std::vector<std::string> data, std::string field, bool remove_no=true);

int db_get_field_index(std::string field);
std::vector<int> db_get_minid_list(sqlite3 *db, int orderby, std::string *errmsg);
std::vector<std::string> db_get_country_list(sqlite3 *db, std::string *errmsg);
std::vector<std::string> db_get_species_list(sqlite3 *db, std::string *errmsg);
std::vector<std::string> db_search_minerals(sqlite3 *db, std::string sname, std::string sminid, std::string country, std::string species, 
        std::string orderby, std::string *errmsg);
std::vector<fs::path> db_get_datafile_list(std::string db_file_path, std::string minid);

void db_generate_report(sqlite3* db, std::string fname, std::string db_path, bool fulldb, bool incdata, bool html, int selected_uid, std::string *errmsg);

bool db_csv_import(sqlite3 *db, std::string filename, std::string *errmsg);
bool db_csv_export(sqlite3 *db, std::string filename, std::string *errmsg);

std::string url_encode(const std::string &value);
std::string str_escape(const std::string &value, char to_escape, char escape_with);
std::string str_tolower(std::string s);
std::string strip_unicode(std::string &str);

#endif

