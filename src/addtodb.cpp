
#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <sqlite3.h> 


static std::vector<std::string> data_header = {
    "MINID", "NAME", "LOCALITY", "LOCID_MNDAT", "SIZE", "WEIGHT", "ACQUISITION", "COLLECTION", "VALUE",
    "S1_SPECIES", "S1_CLASS", "S1_CHEMF", "S1_COLOR", "S1_FLSW", "S1_FLMW", "S1_FLLW", "S1_FL405", "S1_PHSW", "S1_PHMW", "S1_PHLW", "S1_PH405", "S1_TENEBR",
    "S2_SPECIES", "S2_CLASS", "S2_CHEMF", "S2_COLOR", "S2_FLSW", "S2_FLMW", "S2_FLLW", "S2_FL405", "S2_PHSW", "S2_PHMW", "S2_PHLW", "S2_PH405", "S2_TENEBR",
    "S3_SPECIES", "S3_CLASS", "S3_CHEMF", "S3_COLOR", "S3_FLSW", "S3_FLMW", "S3_FLLW", "S3_FL405", "S3_PHSW", "S3_PHMW", "S3_PHLW", "S3_PH405", "S3_TENEBR",
    "S4_SPECIES", "S4_CLASS", "S4_CHEMF", "S4_COLOR", "S4_FLSW", "S4_FLMW", "S4_FLLW", "S4_FL405", "S4_PHSW", "S4_PHMW", "S4_PHLW", "S4_PH405", "S4_TENEBR",
    "RADIOACT", "COMMENTS"
};


int db_addmod_mineral(sqlite3 *db, std::vector<std::string> data, int minid_mod, std::string *errmsg) {

    /* Check data vector size */
    if (data.size()!=63) {
        *errmsg = std::string("Data should contain 63 items, but is has ") + std::to_string(data.size());
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
        for (int i=0; i<62; i++) { query += data_header[i] + "=?, "; }
        query += data_header[62] + "=? ";
        query += "WHERE MINID=?;";
    } else {
        query += "INSERT ";
        if (minid_mod==-2) { query += "OR REPLACE "; }
        query += "INTO MINERALS (";
        if (minid_new>0) { query += "MINID, "; }
        for (int i=1; i<62; i++) { query += data_header[i] + ", "; }
        query += data_header[62] + ") ";
        query += "VALUES (";
        for (int i=1; i<62; i++) { query += "?,"; }
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
    for (int i=0; i<62; i++) {
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

