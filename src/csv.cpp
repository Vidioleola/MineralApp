
#include <iostream>
#include <sqlite3.h>
#include "parsecsv.hpp"
#include "addtodb.hpp"
#include "utils.h"

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
                std::cout << std::endl;
                return true;
            case aria::csv::FieldType::CSV_END:
                std::cout << std::endl;
                return false;
        }
    }
    return false;
}

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

bool db_csv_import(sqlite3 *db, std::string filename, std::string *errmsg) {
    if (!db_csv_import_check(filename, errmsg)) return false;
    if (!db_csv_import_core(db, filename, errmsg, true)) return false;
    if (!db_csv_import_core(db, filename, errmsg, false)) return false;
    return true;
}


bool db_csv_export(sqlite3 *db, std::string filename, std::string *errmsg) {

    std::ofstream csvfile;
    csvfile.open(filename);
    size_t data_header_size = data_header.size();

    /* Write CSV header */
    for (auto i=0; i<data_header_size-1; i++) {
        csvfile << data_header[i] << ",";
    }
    csvfile << data_header[data_header_size-1] << std::endl;

    /* Loop over all minid and write data */
    std::vector<int> minids = db_get_minid_list(db, 0, errmsg);
    for (auto minid : minids) {
        std::vector<std::string> data = db_get_data(db, minid, errmsg);
        for (int i=0; i<data_header_size; i++) {
            std::string s = db_get_field(data, data_header[i], false);
            csvfile << "\"" << str_escape(s, '"', '"') << "\"";
            if (i+1!=data_header_size) csvfile << ",";
        }
        csvfile << std::endl;
    }

    csvfile.close();
    return true;
}


