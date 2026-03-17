#ifndef CSV_WRITER_H
#define CSV_WRITER_H

#include <functional>
#include <string_view>
#include <string>
#include <array>
#include <fstream>
#include <sstream>
#include <cstdint>

template <typename T, std::size_t N>
class CSVWriter {
public:
    using Row = std::array<std::string, N>;
    using Formatter = std::function<const Row(const T&)>;

    CSVWriter(const std::string& file_name, const Row& csv_headers, const Formatter formatter) : 
        m_writer{ m_prefix + file_name },
        m_csv_headers{ csv_headers },
        m_formatter{ formatter } {}

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