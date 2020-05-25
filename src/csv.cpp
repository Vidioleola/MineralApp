
#include <iostream>
#include <sqlite3.h>
#include "parsecsv.hpp"
#include "addtodb.hpp"


static std::vector<std::string> data_header = {
    "MINID", "NAME", "LOCALITY", "LOCID_MNDAT", "SIZE", "WEIGHT", "ACQUISITION", "COLLECTION", "VALUE",
    "S1_SPECIES", "S1_CLASS", "S1_CHEMF", "S1_COLOR", "S1_FLSW", "S1_FLMW", "S1_FLLW", "S1_FL405", "S1_PHSW", "S1_PHMW", "S1_PHLW", "S1_PH405", "S1_TENEBR",
    "S2_SPECIES", "S2_CLASS", "S2_CHEMF", "S2_COLOR", "S2_FLSW", "S2_FLMW", "S2_FLLW", "S2_FL405", "S2_PHSW", "S2_PHMW", "S2_PHLW", "S2_PH405", "S2_TENEBR",
    "S3_SPECIES", "S3_CLASS", "S3_CHEMF", "S3_COLOR", "S3_FLSW", "S3_FLMW", "S3_FLLW", "S3_FL405", "S3_PHSW", "S3_PHMW", "S3_PHLW", "S3_PH405", "S3_TENEBR",
    "S4_SPECIES", "S4_CLASS", "S4_CHEMF", "S4_COLOR", "S4_FLSW", "S4_FLMW", "S4_FLLW", "S4_FL405", "S4_PHSW", "S4_PHMW", "S4_PHLW", "S4_PH405", "S4_TENEBR",
    "RADIOACT", "COMMENTS"
};

bool import_csv_check(std::string filename, std::string *errmsg) {
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

bool import_csv(sqlite3 *db, std::string filename, std::string *errmsg) {
    if (!import_csv_check(filename, errmsg)) {
        return false;
    }
    int ret, minid;
    bool success;
    std::ifstream f(filename);
    aria::csv::CsvParser parser(f);
    std::vector<std::string> data;
    int rowndx = 0;
    for (auto& row : parser) {
        for (auto& field : row) {
            data.push_back(field);
        }
        if (rowndx>0) {
            ret = sscanf(data[0].c_str(), "%d", &minid);
            if (ret!=1) minid = -1;
            data.erase(data.begin());
            success = db_addmod_mineral(db, minid, -1, data, errmsg);
            if (!success) {
                *errmsg = *errmsg + " While reading minid " + std::to_string(minid);
                return false;
            }
        }
        data.clear();
        rowndx++;
    }
    return true;
}

