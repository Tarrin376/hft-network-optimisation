#ifndef CSV_PARSER_H
#define CSV_PARSER_H

#include <fstream>
#include <string>

class CSVReader {
public:
    explicit CSVReader(const std::string& file_path);
    bool has_next();
    const std::string& next() const;

private:
    std::string m_line{};
    std::ifstream m_file{};
    bool m_first_line{ true };
};

#endif