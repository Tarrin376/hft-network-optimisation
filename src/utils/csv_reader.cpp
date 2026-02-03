#include <string>
#include <iostream>

#include "utils/csv_reader.h"

CSVReader::CSVReader(const std::string& file_path) 
    : m_file{ file_path } {}

bool CSVReader::has_next() {
    bool read_success = !std::getline(m_file, m_line).fail();
    if (m_first_line) {
        read_success = !std::getline(m_file, m_line).fail();
        m_first_line = false;
    }

    return read_success;
}

const std::string& CSVReader::next() const {
    return m_line;
}