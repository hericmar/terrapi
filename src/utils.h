#include <array>

namespace Terra
{
/// Get element index within in iterable container.
/// \tparam T
/// \param array
/// \param value
/// \return Index of element, -1 otherwise.
template <typename T, std::size_t Size>
constexpr int GetElementIndex(const std::array<T, Size>& array, const T& value)
{
    int i = 0;

    for (auto it = array.begin(); it != array.end(); ++it)
    {
        if constexpr (std::is_same<const char*, T>::value)
        {
            if (strcmp(*it, value) == 0) { return i; }
        }

        ++i;
    }

    return -1;
}
} // namespace Terra
