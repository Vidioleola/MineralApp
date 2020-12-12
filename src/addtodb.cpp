
#include "addtodb.hpp"

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
    query += "( S1_SPECIES = \""+species+"\" or S2_SPECIES = \""+species+"\" or S3_SPECIES = \""+species+"\" or S4_SPECIES = \""+species+"\") ";
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
        for (int i=0; i<data_header_size-1; i++) { query += data_header[i] + "=?, "; }
        query += data_header[data_header_size-1] + "=? ";
        query += "WHERE MINID=?;";
    } else {
        query += "INSERT ";
        if (minid_mod==-2) { query += "OR REPLACE "; }
        query += "INTO MINERALS (";
        if (minid_new>0) { query += "MINID, "; }
        for (int i=1; i<data_header_size-1; i++) { query += data_header[i] + ", "; }
        query += data_header[data_header_size-1] + ") ";
        query += "VALUES (";
        for (int i=1; i<data_header_size-1; i++) { query += "?,"; }
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
    for (int i=0; i<data_header_size-1; i++) {
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

