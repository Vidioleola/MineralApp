
#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sqlite3.h> 
#include "base64.h"
#include "image.h"

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


int db_addmod_mineral(sqlite3 *db, std::vector<std::string> data, int minid_mod, std::string *errmsg);
std::vector<std::string> db_get_country_list(sqlite3 *db, std::string *errmsg);
std::vector<std::string> db_get_species_list(sqlite3 *db, std::string *errmsg);
std::vector<fs::path> db_get_datafile_list(std::string db_file_path, std::string minid);
void db_generate_report(sqlite3* db, std::string fname, std::string db_path, bool fulldb, bool incdata, bool html, int selected_uid, std::string *errmsg);

