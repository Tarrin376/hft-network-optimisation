#ifndef CSV_WRITER_H
#define CSV_WRITER_H

#include <functional>
#include <string_view>
#include <string>
#include <array>
#include <fstream>
#include <sstream>
#include <cstdint>

/**
 * A generic class for writing objects into a CSV file.
 * 
 * This class provides a type-safe way to transform structured data into 
 * comma-separated rows using a user-defined formatter function. It handles 
 * header initialisation and file streaming automatically.
 * 
 * @tparam T The type of the data object to be serialised.
 * @tparam N The number of columns in the resulting CSV.
 */
template <typename T, std::size_t N>
class CSVWriter {
public:
    using Row = std::array<std::string, N>;

    // A function object that maps an instance of T to a CSV Row.
    using Formatter = std::function<const Row(const T&)>;

    /**
     * @param file_name The name of the file (stored in the default 'data_files' directory).
     * @param csv_headers The column names to write as the first row.
     * @param formatter The logic used to convert objects into string arrays.
     */
    CSVWriter(const std::string& file_name, const Row& csv_headers, const Formatter formatter) : 
        m_writer{ m_prefix + file_name },
        m_csv_headers{ csv_headers },
        m_formatter{ formatter } {}

    /**
     * Serialises a single data object and writes it to a CSV file.
     * @param data The object to be formatted and written.
     */
    void write(const T& data) {
        if (!m_formatter) {
            return;
        }

        if (m_is_first_line) {
            m_writer << to_csv_string(m_csv_headers);
            m_is_first_line = false;
        }

        m_writer << to_csv_string(m_formatter(data));
    }

    /**
     * Converts an array of strings into a single comma-delimited line.
     * @param row The array of column values.
     * @return A formatted CSV string.
     */
    const std::string to_csv_string(const Row& row) const {
        std::stringstream ss{};

        for (std::size_t i = 0; i < N; i++) {
            ss << row[i];
            if (i < N - 1) {
                ss << ',';
            }
        }

        ss << '\n';
        return ss.str();
    }

private:
    const std::string m_prefix{ "../data_files/" };
    const Row m_csv_headers{};
    const Formatter m_formatter{};

    std::ofstream m_writer{};
    bool m_is_first_line{ true };
};

#endif