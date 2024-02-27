#include <string_view>
#include <locale.h>
#include <stdlib.h>
#include <string>
#include <vector>

namespace u8tbl
{
enum class TableType
{
    /**
     * ASCII table
     *
     * +------------+-------------+
     * | @@hostname | @@server_id |
     * +------------+-------------+
     * | node-001   | 2           |
     * +------------+-------------+
     */
    ASCII = 0,

    /**
     * Plain Unicode table
     *
     * ┌────────────┬─────────────┐
     * │ @@hostname │ @@server_id │
     * ├────────────┼─────────────┤
     * │ node-002   │ 3           │
     * └────────────┴─────────────┘
     */
    UNICODE = 1,

    /**
     * Fancy Unicode table
     *
     * 🮣────────────🮦─────────────🮢
     * 🮤 @@hostname │ @@server_id 🮥
     * 🮥────────────┼─────────────🮤
     * 🮤 node-003   │ 4           🮥
     * 🮡────────────🮧─────────────🮠
     */
    FANCY = 2,

    /**
     * No table, simple padded output:
     *
     * @@hostname  @@server_id
     * node-003    4
     */
    NONE = 3,

    /**
     * Tab separated output
     */
    TSV = 4,

    /**
     * Comma separated output
     */
    CSV = 5,
};

/**
 * Converts a 2D-array into a pretty-printed table
 *
 * @tparam Container a container of containers that stores values that are convertible to std::string_view
 *
 * @param container Container of rows to print
 * @param type      Table type
 *
 * @return Pretty-printed table
 */
template<typename Container>
std::string create(Container&& container, TableType type = TableType::UNICODE);

namespace _impl
{
struct TableFormat
{
    std::string_view TOP_LEFT;
    std::string_view TOP_HORIZONTAL;
    std::string_view TOP_MIDDLE;
    std::string_view TOP_RIGHT;

    std::string_view LEFT_MIDDLE;
    std::string_view LEFT_VERTICAL;

    std::string_view MIDDLE_VERTICAL;
    std::string_view MIDDLE_HORIZONTAL;
    std::string_view MIDDLE_MIDDLE;

    std::string_view RIGHT_MIDDLE;
    std::string_view RIGHT_VERTICAL;

    std::string_view BOTTOM_LEFT;
    std::string_view BOTTOM_HORIZONTAL;
    std::string_view BOTTOM_MIDDLE;
    std::string_view BOTTOM_RIGHT;

    std::string_view NEWLINE;
    std::string_view PADDING;
};

static constexpr TableFormat ascii_format {
    "+",    // TOP_LEFT
    "-",    // TOP_HORIZONTAL
    "+",    // TOP_MIDDLE
    "+",    // TOP_RIGHT

    "+",    // LEFT_MIDDLE
    "|",    // LEFT_VERTICAL

    "|",    // MIDDLE_VERTICAL
    "-",    // MIDDLE_HORIZONTAL
    "+",    // MIDDLE_MIDDLE

    "+",    // RIGHT_MIDDLE
    "|",    // RIGHT_VERTICAL

    "+",    // BOTTOM_LEFT
    "-",    // BOTTOM_HORIZONTAL
    "+",    // BOTTOM_MIDDLE
    "+",    // BOTTOM_RIGHT

    "\n",   // NEWLINE
    " ",    // PADDING
};

static constexpr TableFormat unicode_format {
    "┌",    // TOP_LEFT
    "─",    // TOP_HORIZONTAL
    "┬",    // TOP_MIDDLE
    "┐",    // TOP_RIGHT

    "├",    // LEFT_MIDDLE
    "│",    // LEFT_VERTICAL

    "│",    // MIDDLE_VERTICAL
    "─",    // MIDDLE_HORIZONTAL
    "┼",    // MIDDLE_MIDDLE

    "┤",    // RIGHT_MIDDLE
    "│",    // RIGHT_VERTICAL

    "└",    // BOTTOM_LEFT
    "─",    // BOTTOM_HORIZONTAL
    "┴",    // BOTTOM_MIDDLE
    "┘",    // BOTTOM_RIGHT

    "\n",   // NEWLINE
    " ",    // PADDING
};

static constexpr TableFormat fancy_format {
    "🮣",    // TOP_LEFT
    "─",    // TOP_HORIZONTAL
    "🮦",    // TOP_MIDDLE
    "🮢",    // TOP_RIGHT

    "🮥",    // LEFT_MIDDLE
    "🮤",    // LEFT_VERTICAL

    "│",    // MIDDLE_VERTICAL
    "─",    // MIDDLE_HORIZONTAL
    "┼",    // MIDDLE_MIDDLE

    "🮤",    // RIGHT_MIDDLE
    "🮥",    // RIGHT_VERTICAL

    "🮡",    // BOTTOM_LEFT
    "─",    // BOTTOM_HORIZONTAL
    "🮧",    // BOTTOM_MIDDLE
    "🮠",    // BOTTOM_RIGHT

    "\n",   // NEWLINE
    " ",    // PADDING
};

static constexpr TableFormat no_format {
    "",     // TOP_LEFT
    "",     // TOP_HORIZONTAL
    "",     // TOP_MIDDLE
    "",     // TOP_RIGHT

    "",     // LEFT_MIDDLE
    "",     // LEFT_VERTICAL

    "",     // MIDDLE_VERTICAL
    "",     // MIDDLE_HORIZONTAL
    "",     // MIDDLE_MIDDLE

    "",     // RIGHT_MIDDLE
    "",     // RIGHT_VERTICAL

    "",     // BOTTOM_LEFT
    "",     // BOTTOM_HORIZONTAL
    "",     // BOTTOM_MIDDLE
    "",     // BOTTOM_RIGHT

    "",     // NEWLINE
    " ",    // PADDING
};

static constexpr TableFormat tsv_format {
    "",     // TOP_LEFT
    "",     // TOP_HORIZONTAL
    "",     // TOP_MIDDLE
    "",     // TOP_RIGHT

    "",     // LEFT_MIDDLE
    "",     // LEFT_VERTICAL

    "\t",   // MIDDLE_VERTICAL
    "",     // MIDDLE_HORIZONTAL
    "",     // MIDDLE_MIDDLE

    "",     // RIGHT_MIDDLE
    "",     // RIGHT_VERTICAL

    "",     // BOTTOM_LEFT
    "",     // BOTTOM_HORIZONTAL
    "",     // BOTTOM_MIDDLE
    "",     // BOTTOM_RIGHT

    "",     // NEWLINE
    "",     // PADDING
};

static constexpr TableFormat csv_format {
    "",     // TOP_LEFT
    "",     // TOP_HORIZONTAL
    "",     // TOP_MIDDLE
    "",     // TOP_RIGHT

    "",     // LEFT_MIDDLE
    "",     // LEFT_VERTICAL

    ",",    // MIDDLE_VERTICAL
    "",     // MIDDLE_HORIZONTAL
    "",     // MIDDLE_MIDDLE

    "",     // RIGHT_MIDDLE
    "",     // RIGHT_VERTICAL

    "",     // BOTTOM_LEFT
    "",     // BOTTOM_HORIZONTAL
    "",     // BOTTOM_MIDDLE
    "",     // BOTTOM_RIGHT

    "",     // NEWLINE
    "",     // PADDING
};

static constexpr TableFormat formats[] = {
    ascii_format,
    unicode_format,
    fancy_format,
    no_format,
    tsv_format,
    csv_format,
};

#ifdef TABLE_FORMAT_FROM_ENV
TableType type_from_env(TableType type)
{
    using namespace std::string_view_literals;
    if (const char* tt = getenv("TABLE_FORMAT"))
    {
        if (tt == "ASCII"sv)
        {
            return TableType::ASCII;
        }
        else if (tt == "UNICODE"sv)
        {
            return TableType::UNICODE;
        }
        else if (tt == "FANCY"sv)
        {
            return TableType::FANCY;
        }
        else if (tt == "NONE"sv)
        {
            return TableType::NONE;
        }
        else if (tt == "TSV"sv)
        {
            return TableType::TSV;
        }
        else if (tt == "CSV"sv)
        {
            return TableType::CSV;
        }
    }

    return type;
}
#endif

// Simple class that counts the display width of UTF-8 strings
// Note: Assumes that the en_US.UTF-8 locale is installed
class UTF8Converter
{
public:
    UTF8Converter()
        : m_loc(newlocale(LC_ALL_MASK, "en_US.UTF-8", 0))
    {
        if (valid())
        {
            uselocale(m_loc);
        }
    }

    ~UTF8Converter()
    {
        if (m_loc != (locale_t)0)
        {
            freelocale(m_loc);
            uselocale(LC_GLOBAL_LOCALE);
        }
    }

    int width(const std::string& str)
    {
        if (valid())
        {
            m_buffer.resize(std::max(m_buffer.size(), mbstowcs(nullptr, str.c_str(), 0) + 1));
            size_t rc = mbstowcs(&m_buffer[0], str.c_str(), m_buffer.size());

            if (rc != (size_t)-1)
            {
                return wcswidth(m_buffer.data(), rc);
            }
        }

        return str.size();
    }

private:

    bool valid() const
    {
        return m_loc != (locale_t)0;
    }

    locale_t     m_loc;
    std::wstring m_buffer;
};
}

template<typename Container>
std::string create(Container&& container, TableType type)
{
#ifdef TABLE_FORMAT_FROM_ENV
    type = _impl::type_from_env(type);
#endif

    _impl::UTF8Converter utf8;
   std::vector<int> cell_lengths;

    for (const auto& row : container)
    {
        cell_lengths.resize(std::max(cell_lengths.size(), row.size()));

        for (size_t i = 0; i < row.size(); i++)
        {
            cell_lengths[i] = std::max(cell_lengths[i], utf8.width(row[i]));
        }
    }

    const auto& f = _impl::formats[(int)type];
    std::string str;

    // Header
    str += f.TOP_LEFT;

    for (size_t i = 0; i < cell_lengths.size(); i++)
    {
        if (i != 0)
        {
            str += f.TOP_MIDDLE;
        }

        for (int s = 0; s < cell_lengths[i] + 2; s++)
        {
            str += f.TOP_HORIZONTAL;
        }
    }

    str += f.TOP_RIGHT;
    str += f.NEWLINE;

    for (size_t r = 0; r < container.size(); r++)
    {
        const auto& row = container[r];

        if (r != 0)
        {
            // Row separator
            str += f.LEFT_MIDDLE;

            for (size_t i = 0; i < cell_lengths.size(); i++)
            {
                if (i != 0)
                {
                    str += f.MIDDLE_MIDDLE;
                }

                for (int s = 0; s < cell_lengths[i] + 2; s++)
                {
                    str += f.MIDDLE_HORIZONTAL;
                }
            }

            str += f.RIGHT_MIDDLE;
            str += f.NEWLINE;
        }

        // Data row
        str += f.LEFT_VERTICAL;
        size_t end = std::max(row.size(), cell_lengths.size());

        for (size_t i = 0; i < end; i++)
        {
            if (i != 0)
            {
                str += f.MIDDLE_VERTICAL;
            }

            const std::string& val = row.size() > i ? row[i] : std::string();
            str += f.PADDING;
            str += val;
            str += f.PADDING;

            int w = cell_lengths[i] - utf8.width(val);

            for (int i = 0; i < w; i++ )
            {
                str += f.PADDING;
            }
        }

        str += f.RIGHT_VERTICAL;
        // Data rows always end in a newline regardless of the table type
        str += "\n";
    }

    // Footer
    str += f.BOTTOM_LEFT;

    for (size_t i = 0; i < cell_lengths.size(); i++)
    {
        if (i != 0)
        {
            str += f.BOTTOM_MIDDLE;
        }

        for (int s = 0; s < cell_lengths[i] + 2; s++)
        {
            str += f.BOTTOM_HORIZONTAL;
        }
    }

    str += f.BOTTOM_RIGHT;
    str += f.NEWLINE;

    return str;
}
}
