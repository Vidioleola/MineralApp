
#include <vector>
#include <string>
#include <iostream>
#include <sqlite3.h> 

bool db_addmod_mineral(sqlite3 *db, int minid_new, int minid_mod, std::vector<std::string> data, std::string *errmsg) {

    const char *query_insert = "INSERT OR REPLACE INTO MINERALS (MINID, NAME, LOCALITY, LOCID_MNDAT, SIZE, WEIGHT, ACQUISITION, COLLECTION, VALUE, S1_SPECIES, S1_CLASS, S1_CHEMF, S1_COLOR, S1_FLSW, S1_FLMW, S1_FLLW, S1_FL405, S1_PHSW, S1_PHMW, S1_PHLW, S1_PH405, S1_TENEBR, S2_SPECIES, S2_CLASS, S2_CHEMF, S2_COLOR, S2_FLSW, S2_FLMW, S2_FLLW, S2_FL405, S2_PHSW, S2_PHMW, S2_PHLW, S2_PH405, S2_TENEBR, S3_SPECIES, S3_CLASS, S3_CHEMF, S3_COLOR, S3_FLSW, S3_FLMW, S3_FLLW, S3_FL405, S3_PHSW, S3_PHMW, S3_PHLW, S3_PH405, S3_TENEBR, S4_SPECIES, S4_CLASS, S4_CHEMF, S4_COLOR, S4_FLSW, S4_FLMW, S4_FLLW, S4_FL405, S4_PHSW, S4_PHMW, S4_PHLW, S4_PH405, S4_TENEBR, RADIOACT, COMMENTS) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);";
    const char *query_insert_autoid = "INSERT OR REPLACE INTO MINERALS (NAME, LOCALITY, LOCID_MNDAT, SIZE, WEIGHT, ACQUISITION, COLLECTION, VALUE, S1_SPECIES, S1_CLASS, S1_CHEMF, S1_COLOR, S1_FLSW, S1_FLMW, S1_FLLW, S1_FL405, S1_PHSW, S1_PHMW, S1_PHLW, S1_PH405, S1_TENEBR, S2_SPECIES, S2_CLASS, S2_CHEMF, S2_COLOR, S2_FLSW, S2_FLMW, S2_FLLW, S2_FL405, S2_PHSW, S2_PHMW, S2_PHLW, S2_PH405, S2_TENEBR, S3_SPECIES, S3_CLASS, S3_CHEMF, S3_COLOR, S3_FLSW, S3_FLMW, S3_FLLW, S3_FL405, S3_PHSW, S3_PHMW, S3_PHLW, S3_PH405, S3_TENEBR, S4_SPECIES, S4_CLASS, S4_CHEMF, S4_COLOR, S4_FLSW, S4_FLMW, S4_FLLW, S4_FL405, S4_PHSW, S4_PHMW, S4_PHLW, S4_PH405, S4_TENEBR, RADIOACT, COMMENTS) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);";
   const char *query_update = "UPDATE MINERALS SET MINID=?, NAME=?, LOCALITY=?, LOCID_MNDAT=?, SIZE=?, WEIGHT=?, ACQUISITION=?, COLLECTION=?, VALUE=?, S1_SPECIES=?, S1_CLASS=?, S1_CHEMF=?, S1_COLOR=?, S1_FLSW=?, S1_FLMW=?, S1_FLLW=?, S1_FL405=?, S1_PHSW=?, S1_PHMW=?, S1_PHLW=?, S1_PH405=?, S1_TENEBR=?, S2_SPECIES=?, S2_CLASS=?, S2_CHEMF=?, S2_COLOR=?, S2_FLSW=?, S2_FLMW=?, S2_FLLW=?, S2_FL405=?, S2_PHSW=?, S2_PHMW=?, S2_PHLW=?, S2_PH405=?, S2_TENEBR=?, S3_SPECIES=?, S3_CLASS=?, S3_CHEMF=?, S3_COLOR=?, S3_FLSW=?, S3_FLMW=?, S3_FLLW=?, S3_FL405=?, S3_PHSW=?, S3_PHMW=?, S3_PHLW=?, S3_PH405=?, S3_TENEBR=?, S4_SPECIES=?, S4_CLASS=?, S4_CHEMF=?, S4_COLOR=?, S4_FLSW=?, S4_FLMW=?, S4_FLLW=?, S4_FL405=?, S4_PHSW=?, S4_PHMW=?, S4_PHLW=?, S4_PH405=?, S4_TENEBR=?, RADIOACT=?, COMMENTS=? WHERE MINID=?;";

    /* Check data vector size */
    if (data.size()!=62) {
        *errmsg = std::string("Data should contain 62 items, but is has ") + std::to_string(data.size());
        return false;
    }

    /* Setup the db connection and choose the query to use: with specified minid, or auto, or updte */
    int ret;
    sqlite3_stmt *stmt;
    int ndx = 1;
    if (minid_mod<0) {
        if (minid_new<0) {
            sqlite3_prepare_v2(db, query_insert_autoid, strlen(query_insert_autoid), &stmt, NULL);
        } else {
            sqlite3_prepare_v2(db, query_insert, strlen(query_insert), &stmt, NULL);
            sqlite3_bind_int(stmt, ndx, minid_new); ndx+=1;
        }
    } else {
        sqlite3_prepare_v2(db, query_update, strlen(query_update), &stmt, NULL);
        if (minid_new<0) {
            minid_new = minid_mod;
        }
        sqlite3_bind_int(stmt, ndx, minid_new); ndx+=1;
    }

    /* Bind all inputs... */
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
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

