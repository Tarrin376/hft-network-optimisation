#include <string>

#include "csv_reader.h"

CSVReader::CSVReader(const std::string& file_path) 
    : m_file{ file_path } {}

bool CSVReader::has_next() {
    return !std::getline(m_file, m_line).fail();
}

const std::string& CSVReader::next() const {
    return m_line;
}