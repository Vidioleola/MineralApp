
#include "mineraldb.hpp"

#include <cstring>
#include <cctype>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <algorithm>
#include "base64.h"
#include "parsecsv.hpp"
#include "image.h"


/* Initialize an empty db */
void db_initialize(sqlite3 **db, std::string *errmsg) {
    int ret;
    char *errmsg_c;
    ret = sqlite3_open(":memory:", db);
    if (ret!=SQLITE_OK) {
        *errmsg += "Can't open database: ";
        *errmsg += sqlite3_errmsg(*db);
        return;
    }
    const char *query_minerals_create = "CREATE TABLE MINERALS (MINID INTEGER PRIMARY KEY, NAME TEXT NOT NULL, LOCALITY TEXT, LOCID_MNDAT TEXT, SIZE TEXT, WEIGHT TEXT, ACQUISITION TEXT, COLLECTION TEXT, VALUE TEXT, S1_SPECIES TEXT, S1_CLASS TEXT, S1_CHEMF TEXT, S1_COLOR TEXT, S1_FLSW TEXT, S1_FLMW TEXT, S1_FLLW TEXT, S1_FL405 TEXT, S1_PHSW TEXT, S1_PHMW TEXT, S1_PHLW TEXT, S1_PH405 TEXT, S1_TENEBR TEXT, S2_SPECIES TEXT, S2_CLASS TEXT, S2_CHEMF TEXT, S2_COLOR TEXT, S2_FLSW TEXT, S2_FLMW TEXT, S2_FLLW TEXT, S2_FL405 TEXT, S2_PHSW TEXT, S2_PHMW TEXT, S2_PHLW TEXT, S2_PH405 TEXT, S2_TENEBR TEXT, S3_SPECIES TEXT, S3_CLASS TEXT, S3_CHEMF TEXT, S3_COLOR TEXT, S3_FLSW TEXT, S3_FLMW TEXT, S3_FLLW TEXT, S3_FL405 TEXT, S3_PHSW TEXT, S3_PHMW TEXT, S3_PHLW TEXT, S3_PH405 TEXT, S3_TENEBR TEXT, S4_SPECIES TEXT, S4_CLASS TEXT, S4_CHEMF TEXT, S4_COLOR TEXT, S4_FLSW TEXT, S4_FLMW TEXT, S4_FLLW TEXT, S4_FL405 TEXT, S4_PHSW TEXT, S4_PHMW TEXT, S4_PHLW TEXT, S4_PH405 TEXT, S4_TENEBR TEXT, RADIOACT TEXT, COMMENTS TEXT );";
    ret = sqlite3_exec(*db, query_minerals_create, NULL, 0, &errmsg_c);
    if (ret!=SQLITE_OK) {
        *errmsg += errmsg_c;
        sqlite3_free(errmsg);
        sqlite3_close(*db);
        *db=NULL;
        return;
    }
    const char *query_settings_create = "CREATE TABLE SETTINGS (VERSION_MAJOR INT, VERSION_MINOR INT)";
    ret = sqlite3_exec(*db, query_settings_create, NULL, 0, &errmsg_c);
    if (ret!=SQLITE_OK) {
        *errmsg += errmsg_c;
        sqlite3_free(errmsg);
        sqlite3_close(*db);
        *db=NULL;
        return;
    }
    const char *query_set_version = "INSERT INTO SETTINGS (VERSION_MAJOR, VERSION_MINOR) VALUES (" VERSION_MAJOR ", " VERSION_MINOR ");";
    ret = sqlite3_exec(*db, query_set_version, NULL, 0, &errmsg_c);
    if (ret!=SQLITE_OK) {
        *errmsg += errmsg_c;
        sqlite3_free(errmsg);
        sqlite3_close(*db);
        *db=NULL;
        return;
    }
    return;
}


/* Open db from file */
void db_open(sqlite3 **db, std::string fname, std::string *errmsg) {

    int ret;

    /* Open the db from file */
    sqlite3 *db_tmp;
    ret = sqlite3_open(fname.c_str(), &db_tmp);
    if (ret!=SQLITE_OK) {
        *errmsg += "Failed to open file for reading!";
        return;
    }
    /* Create a new db in memory */
    ret = sqlite3_open(":memory:", db);
    if (ret!=SQLITE_OK) {
        *errmsg += "Can't create memory database: ";
        *errmsg += sqlite3_errmsg(*db);
        return;
    }
    /* Make the copy */
    sqlite3_backup *bkp;
    bkp = sqlite3_backup_init(*db, "main", db_tmp, "main");
    if (!bkp) {
        *errmsg += "Failed to backup file!";
        return;
    }
    sqlite3_backup_step(bkp, -1);
    sqlite3_backup_finish(bkp);
    sqlite3_close(db_tmp);

    return;
}


/* Save db to file */
void db_save(sqlite3 *db, std::string fname, std::string *errmsg) {
    sqlite3 *db_tmp;
    int ret = sqlite3_open(fname.c_str(), &db_tmp);
    if (ret!=SQLITE_OK) {
        *errmsg += "Failed to open file for writing!";
        return;
    }
    sqlite3_backup *bkp;
    bkp = sqlite3_backup_init(db_tmp, "main", db, "main");
    if (!bkp) {
        *errmsg += "Failed to initialize database saving! ";
        *errmsg += sqlite3_errmsg(db_tmp);
        return;
    }
    ret = sqlite3_backup_step(bkp, -1);
    if (ret != SQLITE_DONE) {
        *errmsg += "Failed to save all data to file (1)";
    }
    ret = sqlite3_backup_finish(bkp);
    if (ret != SQLITE_OK) {
        *errmsg += "Failed to save all data to file (2)";
    }
    sqlite3_close(db_tmp);
}


/* Close database */
void db_close(sqlite3 *db, std::string *errmsg) {
    int ret = sqlite3_close(db);
    if (ret!=SQLITE_OK) {
        *errmsg += "Cannot close current db. Sorry!";
    }
}


/* Delete a mineral from the database */
void db_delete_mineral(sqlite3 *db, int minid, std::string *errmsg) {
    std::string query = "DELETE FROM MINERALS WHERE minid=" + std::to_string(minid);
    char *errmsg_c;
    int ret = sqlite3_exec(db, query.c_str(), NULL, 0, &errmsg_c);
    if (ret!=SQLITE_OK) {
        *errmsg += "Delete failed! ";
        *errmsg += errmsg_c;
        sqlite3_free(errmsg_c);
    }
}


/* Duplicate the specified minid */
void db_duplicate_mineral(sqlite3 *db, int minid, std::string *errmsg) {
    std::string  query = " \
        CREATE TEMPORARY TABLE temp_table as SELECT * FROM  MINERALS WHERE minid=" + std::to_string(minid) + "; \
        UPDATE temp_table SET MINID=NULL; \
        INSERT INTO MINERALS SELECT * FROM temp_table; \
        DROP TABLE temp_table; \
    ";
    int ret;
    char *errmsg_c;
    ret = sqlite3_exec(db, query.c_str(), NULL, 0, &errmsg_c);
    if (ret!=SQLITE_OK) {
        *errmsg += errmsg_c;
        sqlite3_free(errmsg);
    }
    return;
}


/* Get the index of a given field from the data_header vector */
int db_get_field_index(std::string field) {
    auto it = find(data_header.begin(), data_header.end(), field);
    if (it != data_header.end()) return distance(data_header.begin(), it);
    else return -1;
}

/* Get the field value from the data vector */
std::string db_get_field(std::vector<std::string> data, std::string field, bool remove_no) {
    int ndx = db_get_field_index(field);
    if (ndx>=0) {
        std::string tmp = data[ndx];
        if (remove_no && tmp=="No") return std::string();
        else return tmp;
    }
    else return "ERROR!";
}

/* Get all data relative to a minid from the db */
std::vector<std::string> db_get_data(sqlite3 *db, int minid, std::string *errmsg) {
    std::vector<std::string> data;
    if (minid<0) {
        *errmsg += "MINID is negative. It should be positive...";
        return data;
    }
    std::string query = "SELECT ";
    size_t data_header_size = data_header.size();
    for (size_t i=0; i<data_header_size; i++) {
        query += data_header[i];
        if (i<data_header_size-1) query += ", ";
    }
    query += " FROM MINERALS WHERE MINID=?";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, minid);
    int ret=sqlite3_step(stmt);
    if (ret!=SQLITE_ROW) {
        *errmsg += "Impossible to find ID in database :(";
        sqlite3_finalize(stmt);
        return data;
    }
    for (size_t i=0; i<data_header.size(); i++) {
        const unsigned char *uc = sqlite3_column_text(stmt, i);
        std::string tmp = "";
        if (uc!=NULL) tmp=(const char*)uc;
        data.push_back(tmp);
    }
    sqlite3_finalize(stmt);
    return data;
}

/* Get the list of minid */
std::vector<int> db_get_minid_list(sqlite3 *db, int orderby, std::string *errmsg) {
    std::vector<int> minids;
    const char *query;
    if (orderby==1) {
        query = "SELECT MINID FROM MINERALS ORDER BY NAME";
    } else {
        query = "SELECT MINID FROM MINERALS ORDER BY MINID";
    }
    int ret;
    sqlite3_stmt *stmt;
    ret = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    if (ret!=SQLITE_OK) {
        *errmsg += std::string("Error: ") + sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        return minids;
    }
    while ((ret=sqlite3_step(stmt))==SQLITE_ROW) {
        const unsigned char *uc = sqlite3_column_text(stmt, 0);
        std::string tmp = "";
        if (uc!=NULL) tmp=(const char*)uc;
        int minid = -1;
        int ret = sscanf(tmp.c_str(), "%d", &minid);
        if (ret!=1) minid = -1;
        minids.push_back(minid);
    }
    if (ret!=SQLITE_DONE) {
        *errmsg += std::string("Error: ") + sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        return minids;
    }
    sqlite3_finalize(stmt);
    return minids;
}


/* Search the database for minerals that match the given criteria. Return a formated list of strings */
std::vector<std::string> db_search_minerals(sqlite3 *db, std::string sname, std::string sminid, std::string country, std::string species, 
        std::string orderby, std::string *errmsg) {

    std::string query = "SELECT MINID, NAME FROM MINERALS ";
    query += "WHERE LOCALITY LIKE \"%" + country + "\" AND ";
    query += "( S1_SPECIES LIKE \"%"+species+"%\" or S2_SPECIES LIKE \"%"+species+"%\" or S3_SPECIES LIKE \"%"+species+"%\" or S4_SPECIES LIKE \"%"+species+"%\" ) ";
    query += "AND ( NAME LIKE \"%"+sname+"%\" OR MINID LIKE \"%"+sminid+"%\" ) ";
    query += "ORDER BY " + orderby;

    std::vector<std::string> results;
    int ret;
    sqlite3_stmt *stmt;
    ret = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);
    if (ret!=SQLITE_OK) {
        *errmsg += std::string(sqlite3_errmsg(db));
        return results;
    }
    const unsigned char *uc;
    std::string minid, name;
    while ((ret=sqlite3_step(stmt))==SQLITE_ROW) {
        uc = sqlite3_column_text(stmt, 0);
        if (uc!=NULL) minid=(const char*)uc; else minid="";
        uc = sqlite3_column_text(stmt, 1);
        if (uc!=NULL) name=(const char*)uc; else name="";
        results.push_back(name + " [" + minid + "]");
    }
    if (ret!=SQLITE_DONE) {
        *errmsg += std::string(sqlite3_errmsg(db));
    }
    sqlite3_finalize(stmt);
    return results;
}


/* Get a sorted list of files in the data directory for a given specimen */
std::vector<fs::path> db_get_datafile_list(std::string db_file_path, std::string minid) {
    std::vector<fs::path> files;
    /* Get basepath */
    fs::path basepath;
    basepath = fs::path(db_file_path).remove_filename() / "data";
    if (!fs::is_directory(basepath)) return files;
    basepath = fs::path(db_file_path).remove_filename() / "data" / minid;
    if (!fs::is_directory(basepath)) {
        std::string prefix = minid + " ";
        basepath = fs::path();
        for (const auto & entry : fs::directory_iterator(fs::path(db_file_path).remove_filename() / "data")) {
            if (entry.is_directory() && strncmp(entry.path().filename().c_str(), prefix.c_str(), prefix.size())==0) {
                basepath = entry.path();
                break;
            }
        }
        if (basepath.empty()) return files;
    }
    /* Get list of files */
    for (auto& entry: fs::directory_iterator(basepath)) {
        if (entry.path().filename().string().at(0) == '.') continue;
        files.push_back(entry.path());
    }
    std::sort(files.begin(), files.end());
    return files;
}

/* Insert a new mineral or modify an existing one */
int db_addmod_mineral(sqlite3 *db, std::vector<std::string> data, int minid_mod, std::string *errmsg) {

    /* Check data vector size */
    size_t data_header_size = data_header.size();
    if (data.size()!=data_header_size) {
        *errmsg = std::string("Data should contain ") + std::to_string(data_header_size) + " items, but is has " + std::to_string(data.size());
        return -2;
    }

    /* Get the minid from data[0] */
    int ret;
    int minid_new;
    ret = sscanf(data[0].c_str(), "%d", &minid_new);
    if (ret!=1) minid_new = -1;
    data.erase(data.begin());

    /* Create SQL query */
    std::string query = "";
    if (minid_mod>0) {
        query += "UPDATE MINERALS SET ";
        for (size_t i=0; i<data_header_size-1; i++) { query += data_header[i] + "=?, "; }
        query += data_header[data_header_size-1] + "=? ";
        query += "WHERE MINID=?;";
    } else {
        query += "INSERT ";
        if (minid_mod==-2) { query += "OR REPLACE "; }
        query += "INTO MINERALS (";
        if (minid_new>0) { query += "MINID, "; }
        for (size_t i=1; i<data_header_size-1; i++) { query += data_header[i] + ", "; }
        query += data_header[data_header_size-1] + ") ";
        query += "VALUES (";
        for (size_t i=1; i<data_header_size-1; i++) { query += "?,"; }
        if (minid_new>0) { query += "?,"; }
        query += "?);";
    }

    /* Setup the db connection */
    sqlite3_stmt *stmt;
    int ndx = 1;
    sqlite3_prepare_v2(db, query.c_str(), strlen(query.c_str()), &stmt, NULL);
    if (minid_mod<0) {
        if (minid_new>0) {
            sqlite3_bind_int(stmt, ndx, minid_new); ndx+=1;
        }
    } else {
        if (minid_new<0) {
            minid_new = minid_mod;
        }
        sqlite3_bind_int(stmt, ndx, minid_new); ndx+=1;
    }

    /* Bind all inputs */
    for (size_t i=0; i<data_header_size-1; i++) {
        sqlite3_bind_text(stmt, ndx, data[i].c_str(), -1, SQLITE_TRANSIENT); ndx+=1;
    }
    if (minid_mod>=0) {
        sqlite3_bind_int(stmt, ndx, minid_mod); ndx+=1;
    }

    /* Run the query */
    ret = sqlite3_step(stmt);
    if (ret!=SQLITE_DONE) {
        *errmsg = std::string("Internal error: ") + std::string(sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -2;
    }

    sqlite3_finalize(stmt);
    return minid_new;
}

/* Get a list of unique countries in the db */
std::vector<std::string> db_get_country_list(sqlite3 *db, std::string *errmsg) {

    std::vector<std::string> loclst;
    const char *query = "SELECT LOCALITY FROM MINERALS";
    int ret;
    sqlite3_stmt *stmt;
    ret = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    if (ret!=SQLITE_OK) {
        *errmsg += std::string(sqlite3_errmsg(db));
        return loclst;
    }
    std::string loc = "";
    std::string country = "";
    while ((ret=sqlite3_step(stmt))==SQLITE_ROW) {
        const unsigned char *uc = sqlite3_column_text(stmt, 0);
        loc = "";
        if (uc!=NULL) loc = (const char*)uc;
        if (loc.length()==0) continue;
        size_t pos = loc.rfind(", ");
        if (pos==std::string::npos) {
            country = loc;
        } else {
            country = loc.substr(pos+2, std::string::npos);
        }
        if (std::find(loclst.begin(), loclst.end(), country) == loclst.end()) {
            loclst.push_back(country);
        }
    }
    if (ret!=SQLITE_DONE) {
        *errmsg += std::string(sqlite3_errmsg(db));
    }
    sqlite3_finalize(stmt);
    std::sort(loclst.begin(), loclst.end());
    return loclst;
}

/* Get a list of unique species in the db */
std::vector<std::string> db_get_species_list(sqlite3 *db, std::string *errmsg) {

    std::vector<std::string> specieslst;
    const char *query = "SELECT S1_SPECIES,S2_SPECIES,S3_SPECIES,S4_SPECIES FROM MINERALS";
    int ret;
    sqlite3_stmt *stmt;
    ret = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    if (ret!=SQLITE_OK) {
        *errmsg += std::string(sqlite3_errmsg(db));
        return specieslst;
    }
    std::string species = "";
    while ((ret=sqlite3_step(stmt))==SQLITE_ROW) {
        for (int i=0; i<4; i++) {
            const unsigned char *uc = sqlite3_column_text(stmt, i);
            species = "";
            if (uc!=NULL) species = (const char*)uc;
            if (species.length()==0) continue;
            if (std::find(specieslst.begin(), specieslst.end(), species) == specieslst.end()) {
                specieslst.push_back(species);
            }
        }
    }
    if (ret!=SQLITE_DONE) {
        *errmsg += std::string(sqlite3_errmsg(db));
    }
    sqlite3_finalize(stmt);
    std::sort(specieslst.begin(), specieslst.end());
    return specieslst;
}

/* Generate an html-formatted report for one specimen */
static std::string db_generate_html_report_minid(std::vector<std::string> data) {

    std::string tmp;
    std::string html;

    html += "<h2>" + db_get_field(data, "NAME") + "</h2>\n";
    html += "<table style=\"table-layout: fixed; width: 100%;\">\n";
    html += "<colgroup>";
    html += "<col style=\"width:20%\">";
    html += "<col style=\"width:20%\">";
    html += "<col style=\"width:20%\">";
    html += "<col style=\"width:20%\">";
    html += "<col style=\"width:20%\">";
    html += "</colgroup>\n";
    html += "<tbody>\n";
    html += "<tr><td><b>Unique ID:</b></td><td colspan=\"4\">" + db_get_field(data, "MINID") + "</td></tr>\n";
    tmp = db_get_field(data, "LOCALITY");
    if (tmp.size()>0) {
        html += "<tr><td><b>Locality:</b></td><td colspan=\"4\">" + db_get_field(data, "LOCALITY");
        tmp = db_get_field(data, "LOCID_MNDAT");
        if (tmp.size()>0) {
            html += " <a href=\"https://www.mindat.org/loc-" + tmp + ".html\" target=\"_blank\">";
            html += "(mindat: " + tmp +")";
        }
        html += "</td></tr>\n";
    }
    tmp = db_get_field(data, "SIZE");
    if (tmp.size()>0) {
        html += "<tr><td><b>Size:</b></td><td colspan=\"4\">" + tmp + "</td></tr>\n";
    }
    tmp = db_get_field(data, "WEIGHT");
    if (tmp.size()>0) {
        html += "<tr><td><b>Weight:</b></td><td colspan=\"4\">" + tmp + "</td></tr>\n";
    }
    tmp = db_get_field(data, "ACQUISITION");
    if (tmp.size()>0) {
        html += "<tr><td><b>Acquisition:</b></td><td colspan=\"4\">" + tmp + "</td></tr>\n";
    }
    tmp = db_get_field(data, "COLLECTION");
    if (tmp.size()>0) {
        html += "<tr><td><b>Collection:</b></td><td colspan=\"4\">" + tmp + "</td></tr>\n";
    }
    tmp = db_get_field(data, "VALUE");
    if (tmp.size()>0) {
        html += "<tr><td><b>Value:</b></td><td colspan=\"4\">" + tmp + "</td></tr>\n";
    }

    /* Species */
    tmp = db_get_field(data, "S1_SPECIES") + db_get_field(data, "S2_SPECIES") + db_get_field(data, "S3_SPECIES") + db_get_field(data, "S4_SPECIES");
    if (tmp.size()>0) {
        html += "<tr><td><b>Species:</b></td><td>" + db_get_field(data, "S1_SPECIES") + "</td><td>" + db_get_field(data, "S2_SPECIES") + "</td><td>" + db_get_field(data, "S3_SPECIES") + "</td><td>" + db_get_field(data, "S4_SPECIES") + "</td></tr>\n";
    }
    tmp = db_get_field(data, "S1_CLASS") + db_get_field(data, "S2_CLASS") + db_get_field(data, "S3_CLASS") + db_get_field(data, "S4_CLASS");
    if (tmp.size()>0) {
        html += "<tr><td><b>Class:</b></td><td>" + db_get_field(data, "S1_CLASS") + "</td><td>" + db_get_field(data, "S2_CLASS") + "</td><td>" + db_get_field(data, "S3_CLASS") + "</td><td>" + db_get_field(data, "S4_CLASS") + "</td></tr>\n";
    }
    tmp = db_get_field(data, "S1_CHEMF") + db_get_field(data, "S2_CHEMF") + db_get_field(data, "S3_CHEMF") + db_get_field(data, "S4_CHEMF");
    if (tmp.size()>0) {
        html += "<tr><td><b>Chemical Formula:</b></td><td>" + db_get_field(data, "S1_CHEMF") + "</td><td>" + db_get_field(data, "S2_CHEMF") + "</td><td>" + db_get_field(data, "S3_CHEMF") + "</td><td>" + db_get_field(data, "S4_CHEMF") + "</td></tr>\n";
    }
    tmp = db_get_field(data, "S1_COLOR") + db_get_field(data, "S2_COLOR") + db_get_field(data, "S3_COLOR") + db_get_field(data, "S4_COLOR");
    if (tmp.size()>0) {
        html += "<tr><td><b>Color:</b></td><td>" + db_get_field(data, "S1_COLOR") + "</td><td>" + db_get_field(data, "S2_COLOR") + "</td><td>" + db_get_field(data, "S3_COLOR") + "</td><td>" + db_get_field(data, "S4_COLOR") + "</td></tr>\n";
    }

    /* Fluorescence */
    tmp = db_get_field(data, "S1_FLSW") + db_get_field(data, "S2_FLSW") + db_get_field(data, "S3_FLSW") + db_get_field(data, "S4_FLSW");
    if (tmp.size()>0) {
        html += "<tr><td><b>Fluorescence SW:</b></td><td>" + db_get_field(data, "S1_FLSW") + "</td><td>" + db_get_field(data, "S2_FLSW") + "</td><td>" + db_get_field(data, "S3_FLSW") + "</td><td>" + db_get_field(data, "S4_FLSW") + "</td></tr>\n";
    }
    tmp = db_get_field(data, "S1_FLMW") + db_get_field(data, "S2_FLMW") + db_get_field(data, "S3_FLMW") + db_get_field(data, "S4_FLMW");
    if (tmp.size()>0) {
        html += "<tr><td><b>Fluorescence MW:</b></td><td>" + db_get_field(data, "S1_FLMW") + "</td><td>" + db_get_field(data, "S2_FLMW") + "</td><td>" + db_get_field(data, "S3_FLMW") + "</td><td>" + db_get_field(data, "S4_FLMW") + "</td></tr>\n";
    }
    tmp = db_get_field(data, "S1_FLLW") + db_get_field(data, "S2_FLLW") + db_get_field(data, "S3_FLLW") + db_get_field(data, "S4_FLLW");
    if (tmp.size()>0) {
        html += "<tr><td><b>Fluorescence LW:</b></td><td>" + db_get_field(data, "S1_FLLW") + "</td><td>" + db_get_field(data, "S2_FLLW") + "</td><td>" + db_get_field(data, "S3_FLLW") + "</td><td>" + db_get_field(data, "S4_FLLW") + "</td></tr>\n";
    }
    tmp = db_get_field(data, "S1_FL405") + db_get_field(data, "S2_FL405") + db_get_field(data, "S3_FL405") + db_get_field(data, "S4_FL405");
    if (tmp.size()>0) {
        html += "<tr><td><b>Fluorescence 405nm:</b></td><td>" + db_get_field(data, "S1_FL405") + "</td><td>" + db_get_field(data, "S2_FL405") + "</td><td>" + db_get_field(data, "S3_FL405") + "</td><td>" + db_get_field(data, "S4_FL405") + "</td></tr>\n";
    }

    /* Phosphorescence */
    tmp = db_get_field(data, "S1_PHSW") + db_get_field(data, "S2_PHSW") + db_get_field(data, "S3_PHSW") + db_get_field(data, "S4_PHSW");
    if (tmp.size()>0) {
        html += "<tr><td><b>Phosphorescence SW:</b></td><td>" + db_get_field(data, "S1_PHSW") + "</td><td>" + db_get_field(data, "S2_PHSW") + "</td><td>" + db_get_field(data, "S3_PHSW") + "</td><td>" + db_get_field(data, "S4_PHSW") + "</td></tr>\n";
    }
    tmp = db_get_field(data, "S1_PHMW") + db_get_field(data, "S2_PHMW") + db_get_field(data, "S3_PHMW") + db_get_field(data, "S4_PHMW");
    if (tmp.size()>0) {
        html += "<tr><td><b>Phosphorescence MW:</b></td><td>" + db_get_field(data, "S1_PHMW") + "</td><td>" + db_get_field(data, "S2_PHMW") + "</td><td>" + db_get_field(data, "S3_PHMW") + "</td><td>" + db_get_field(data, "S4_PHMW") + "</td></tr>\n";
    }
    tmp = db_get_field(data, "S1_PHLW") + db_get_field(data, "S2_PHLW") + db_get_field(data, "S3_PHLW") + db_get_field(data, "S4_PHLW");
    if (tmp.size()>0) {
        html += "<tr><td><b>Phosphorescence LW:</b></td><td>" + db_get_field(data, "S1_PHLW") + "</td><td>" + db_get_field(data, "S2_PHLW") + "</td><td>" + db_get_field(data, "S3_PHLW") + "</td><td>" + db_get_field(data, "S4_PHLW") + "</td></tr>\n";
    }
    tmp = db_get_field(data, "S1_PH405") + db_get_field(data, "S2_PH405") + db_get_field(data, "S3_PH405") + db_get_field(data, "S4_PH405");
    if (tmp.size()>0) {
        html += "<tr><td><b>Phosphorescence 405nm:</b></td><td>" + db_get_field(data, "S1_PH405") + "</td><td>" + db_get_field(data, "S2_PH405") + "</td><td>" + db_get_field(data, "S3_PH405") + "</td><td>" + db_get_field(data, "S4_PH405") + "</td></tr>\n";
    }

    /* Other properties */
    tmp = db_get_field(data, "S1_TENEBR") + db_get_field(data, "S2_TENEBR") + db_get_field(data, "S3_TENEBR") + db_get_field(data, "S4_TENEBR");
    if (tmp.size()>0) {
        html += "<tr><td><b>Tenebrescence:</b></td><td>" + db_get_field(data, "S1_TENEBR") + "</td><td>" + db_get_field(data, "S2_TENEBR") + "</td><td>" + db_get_field(data, "S3_TENEBR") + "</td><td>" + db_get_field(data, "S4_TENEBR") + "</td></tr>\n";
    }

    /* Radioactivity and Comments */
    tmp = db_get_field(data, "RADIOACT");
    if (tmp.size()>0) {
        html += "<tr><td><b>Radioactive:</b></td><td colspan=\"4\">" + tmp + "</td></tr>\n";
    }
    tmp = db_get_field(data, "COMMENTS");
    if (tmp.size()>0) {
        html += "<tr><td><b>Notes:</b></td><td colspan=\"4\">" + tmp + "</td></tr>\n";
    }
    html += "</tbody>\n";
    html += "</table>\n";

    return html;
}

/* Generate html report for the images */
static std::string db_generate_html_report_minid_images(std::string db_file_path, std::string minid, bool mht) {
    std::vector<fs::path> files = db_get_datafile_list(db_file_path, minid);
    std::vector<std::string> img_formats = { ".png", ".jpg", ".jpeg", ".gif", ".tiff", ".tif" };
    std::vector<fs::path> images;
    for (const auto & path : files) {
        std::string ext = path.extension().string();
        if (std::find(img_formats.begin(), img_formats.end(), ext) != img_formats.end()) {
            images.push_back(path);
        }
    }
    if (images.size()==0) return "";
    std::string html;
    html += "<table style=\"table-layout: fixed; width: 100%;\">\n";
    html += "<colgroup>";
    html += "<col style=\"width:33%\">";
    html += "<col style=\"width:33%\">";
    html += "<col style=\"width:33%\">";
    html += "</colgroup>\n";
    html += "<tbody>\n";
    int num=0;
    html += "<tr>\n";
    for (const auto & path : images) {
        if (mht) {
            dz2::Image img(path.string());
            int w = 150;
            int h = w*img.height()/img.width();
            html += "<td><img src=\"mindb.fld/" + minid + "-" + path.filename().string() + "\" width='" + std::to_string(w) + "' height='" + std::to_string(h) + "'></td>\n";
        } else {
            html += "<td><img src=\"" + path.string() + "\" width=\"100%\"></td>\n";
        }
        num = (num+1)%3;
        if (num==0) {
            html += "</tr>\n<tr>\n";
        }
    }
    html += "</tr>\n</tbody>\n</table>\n";
    return html;
}


/* Generate an html-formatted report for the whole database */
static std::string db_generate_html_report(sqlite3 *db, std::vector<int> minids, std::string db_file_path, bool mht, bool incdata, std::string *errmsg) {
    std::vector<std::string> data;
    std::string html;
    html += "<html xmlns:o='urn:schemas-microsoft-com:office:office' xmlns:w='urn:schemas-microsoft-com:office:word' xmlns='http://www.w3.org/TR/REC-html40'>\n";
    html += "<head><title>Mineral Database -- generated by MineralApp</title>\n";
    html += "<style><!-- \n";
    html += "@page WordSection1 {size:8.5in 11.0in;margin:1.0in 1.0in 1.0in 1.0in;mso-header-margin:.5in;mso-footer-margin:.5in;mso-paper-source:0;}";
    html += "div.WordSection1 {page:WordSection1;}\n";
    html += "</style>\n";
    html += "</head>\n";
    html += "<body>\n";
    html += "<div class=WordSection1>\n";
    for (const auto& minid: minids) {
        data = db_get_data(db, minid, errmsg);
        if (errmsg->size()>0) return std::string();
        html += db_generate_html_report_minid(data);
        if (incdata) html += db_generate_html_report_minid_images(db_file_path, std::to_string(minid), mht);
        html += "<br clear=all style='mso-special-character:line-break;page-break-before:always'>\n";
    }
    html += "</div>\n";
    html += "</body>\n";
    html += "</html>\n";
    return html;
}

/* Generate report -- main function */
void db_generate_report(sqlite3* db, std::string db_file_path, std::string fname, bool fulldb, bool incdata, bool html, int selected_uid, std::string *errmsg) {

    /* Prepare list of minids */
    std::vector<int> minids;
    if (fulldb) {
        minids = db_get_minid_list(db, 0, errmsg);
        if (errmsg->size()>0) return;
    } else if (selected_uid>=0) {
        minids.push_back(selected_uid);
    } else {
        *errmsg += "Sorry, no mineral was selected.";
        return;
    }

    /* Generate html report */
    std::string htmlreport = db_generate_html_report(db, minids, db_file_path, !html, incdata, errmsg);
    if (errmsg->size()>0) return;

    /* Create html */
    if (html) {
        std::ofstream fp(fname);
        fp << htmlreport;
        fp.close();
    }

    /* Create mht */
    else {
        std::ofstream fp(fname);
        std::string boundary = "----=_NextPart_ZROIIZO.ZCZYUACXV.ZARTUI";
        std::string header = "MIME-Version: 1.0\nContent-Type: multipart/related; boundary=\""+boundary+"\"\n\n";
        fp << header;
        fp << "--" << boundary << "\n";
        fp << "Content-Location: file:///C:/mindb.htm\n";
        fp << "Content-Transfer-Encoding: base64\n";
        fp << "Content-Type: text/html\n\n";
        fp << base64_encode_mime(htmlreport);
        fp << "\n\n";
        if (incdata) {
            std::vector<std::string> img_formats = { ".png", ".jpg", ".jpeg" };
            for (const auto& minid: minids) {
                std::string uid = std::to_string(minid);
                std::vector<fs::path> files = db_get_datafile_list(db_file_path, uid);
                for (const auto & path : files) {
                    std::string ext = path.extension().string();
                    if (std::find(img_formats.begin(), img_formats.end(), ext) != img_formats.end()) {
                        dz2::Image img(path.string());
                        img.resize(800);
                        img.set_jpeg_quality(90);
                        fp << "--" << boundary << "\n";
                        fp << "Content-Location: file:///C:/mindb.fld/"+uid+"-"+path.filename().string() + "\n";
                        fp << "Content-Transfer-Encoding: base64\n";
                        fp << "Content-Type: image/" + ext.substr(1) + "\n\n";
                        fp << base64_encode_mime(img.write_to_string("jpg"));
                        fp << "\n\n";
                    }
                }
            }
        }
        fp << "--" << boundary << "--\n";
        fp.close();
    }
    return;
}



/*****************************************************************************
    CSV Import/Export
*****************************************************************************/

/* CSV import -- Utility function: Check that the header file is correct */
static bool db_csv_import_check(std::string filename, std::string *errmsg) {
    std::ifstream f(filename);
    aria::csv::CsvParser parser(f);
    int index = 0;
    for (;;) {
        auto field = parser.next_field();
        switch (field.type) {
            case aria::csv::FieldType::DATA:
                if (*field.data==data_header[index]) {
                    index++;
                } else {
                    *errmsg = std::string("Column ") + std::to_string(index+1) + " should be " + data_header[index] + " but I got " + *field.data;
                    return false;
                }
                break;
            case aria::csv::FieldType::ROW_END:
                return true;
            case aria::csv::FieldType::CSV_END:
                return false;
        }
    }
    return false;
}

/* CSV import -- Utility function: Read all data from a CSV file */
static bool db_csv_import_core(sqlite3 *db, std::string filename, std::string *errmsg, bool skip_miss_id) {
    int success_id;
    int ret;
    int minid;
    std::ifstream f(filename);
    aria::csv::CsvParser parser(f);
    std::vector<std::string> data;
    int rowndx = 0;
    for (auto& row : parser) {
        rowndx++;
        data.clear();
        for (auto& field : row) {
            data.push_back(field);
        }
        if (rowndx>1) {
            ret = sscanf(data[0].c_str(), "%d", &minid);
            if (ret!=1) minid = -1;
            if (skip_miss_id) {
                if (minid<0) continue;
            } else {
                if (minid>0) continue;
            }
            success_id = db_addmod_mineral(db, data, -2, errmsg);
            if (success_id<-1) {
                *errmsg = *errmsg + " While reading minid " + data[0] + " " + data[1];
                return false;
            }
        }
    }
    return true;
}

/* CSV import -- Main driver */
bool db_csv_import(sqlite3 *db, std::string filename, std::string *errmsg) {
    if (!db_csv_import_check(filename, errmsg)) return false;
    if (!db_csv_import_core(db, filename, errmsg, true)) return false;
    if (!db_csv_import_core(db, filename, errmsg, false)) return false;
    return true;
}

/* CSV export */
bool db_csv_export(sqlite3 *db, std::string filename, std::string *errmsg) {

    std::ofstream csvfile;
    csvfile.open(filename);
    size_t data_header_size = data_header.size();

    /* Write CSV header */
    for (size_t i=0; i<data_header_size-1; i++) {
        csvfile << data_header[i] << ",";
    }
    csvfile << data_header[data_header_size-1] << std::endl;

    /* Loop over all minid and write data */
    std::vector<int> minids = db_get_minid_list(db, 0, errmsg);
    for (auto minid : minids) {
        std::vector<std::string> data = db_get_data(db, minid, errmsg);
        for (size_t i=0; i<data_header_size; i++) {
            std::string s = db_get_field(data, data_header[i], false);
            csvfile << "\"" << str_escape(s, '"', '"') << "\"";
            if (i+1!=data_header_size) csvfile << ",";
        }
        csvfile << std::endl;
    }

    csvfile.close();
    return true;
}


/*****************************************************************************
    General utilities
*****************************************************************************/

/* Convert a string to a valid URL */
std::string url_encode(const std::string &value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;
    for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
        std::string::value_type c = (*i);
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~' || c == '/') {
            escaped << c;
            continue;
        }
        escaped << std::uppercase;
        escaped << '%' << std::setw(2) << int((unsigned char) c);
        escaped << std::nouppercase;
    }
    return escaped.str();
}

/* Escape all occurences of to_escape with escape_with. */
std::string str_escape(const std::string &value, char to_escape, char escape_with) {
    std::ostringstream escaped;
    escaped.fill('0');
    for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
        std::string::value_type c = (*i);
        if (c==to_escape) escaped << escape_with;
        escaped << c;
    }
    return escaped.str();
}

/* Convert a string to lowercase. */
std::string str_tolower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
    return s;
}

/* Remove all unicode symbols from a string. */
static bool invalid_char(char c) {
    return c<0;
}
std::string strip_unicode(std::string &str) {
    str.erase(remove_if(str.begin(),str.end(), invalid_char), str.end());
    return str;
}

