#ifndef CSV_PARSER_H
#define CSV_PARSER_H

#include <fstream>
#include <string>

/**
 * A lightweight class for sequential line-by-line reading of CSV files.
 */
class CSVReader {
public:
    explicit CSVReader(const std::string& file_path);

    /**
     * Moves the internal file stream to the next line and 
     * handles skipping the header row on the first call.
     * @return True if a new line was successfully loaded into the buffer 'm_line'.
     */
    bool has_next();

    /**
     * Provides access to the most recently read line of the CSV.
     * @return The string content of the buffer 'm_line'.
     */
    const std::string& next() const;

private:
    std::string m_line{};
    std::ifstream m_file{};
    bool m_first_line{ true };
};

#endif