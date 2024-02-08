// Imported from lpem project (https://github.com/loudove/lpem) 
// under the MIT license
// Αuthor: Loukas D. Peristeras (l.peristeras@inn.demokritos.gr)

#include <cstring>
#include <iostream>
#include <iterator>
#include <numeric>
#include <limits>

#include "histogram.h"
#include "error.h"

using namespace std;

namespace Math
{

template <typename T> inline
Histogram<T>::Histogram(const string &name) : m_name(name),
                                              m_type(UNINITIALIZED),
                                              m_min(static_cast<T>(0)),
                                              m_max(static_cast<T>(0)),
                                              m_ref(static_cast<T>(0)),
                                              m_d(static_cast<T>(0)),
                                              m_nbin(0),
                                              m_offset(static_cast<T>(0.0)),
                                              m_initialized(false),
                                              m_variable(),
                                              m_under(),
                                              m_over()
{
}

template <typename T> inline
Histogram<T>::~Histogram() {}

template <typename T> inline
void Histogram<T>::init(const T &min, const T &max, const int &nbin)
{
    ASSERT(min < max && nbin > 0, "Histogram<T>::init, check arguments");

    m_type = FIXED;
    m_min = m_ref = min;
    m_max = max;
    m_nbin = nbin;
    T length = static_cast<T>(max - min);
    m_d = length / static_cast<T>(m_nbin);
    m_bin.resize(m_nbin, static_cast<T>(0));

    m_variable.reset();

    m_initialized = true;
}

template <typename T> inline
void Histogram<T>::init(const T &min, const T &max, const double &d)
{
    ASSERT(min < max && d > static_cast<T>(0.0), "Histogram<T>::init, check arguments");

    m_type = FIXED;
    m_min = m_ref = min;
    m_max = max;
    T length = static_cast<T>(max - min);
    m_nbin = static_cast<int>(floor(length / d));
    m_d = length / static_cast<double>(m_nbin);
    m_bin.resize(m_nbin, static_cast<T>(0));

    m_variable.reset();

    m_initialized = true;
}

template <typename T> inline
void Histogram<T>::init(const double &d, const T &value, const bool &addit)
{
    ASSERT(d > static_cast<T>(0.0), "Histogram<T>::init, check arguments");

    m_type = FREE;
    m_map.clear();
    m_d = d;
    int iref = static_cast<int>(floor(static_cast<T>(value) / d));
    // int iref = static_cast<int>( static_cast<T>(ref) / d);
    m_ref = static_cast<T>(iref * d);

    m_variable.reset();

    m_initialized = true;

    if (addit)
        add(value);
}

template <typename T> inline
void Histogram<T>::setOffset(const double &offset)
{
    if (m_offset >= 0.0 && m_offset <= 1.0)
        m_offset = offset;
}

template <typename T> inline
void Histogram<T>::add(const T &value, const int &multiplicity)
{
    ASSERT(m_initialized, "Histogram<T>::add, use of non initialized histogram");

    int n = static_cast<int>(floor(static_cast<T>(value - m_ref) / m_d));

    addAt(n, value, multiplicity);
}

template <typename T> inline
void Histogram<T>::addAt(const int &n, const T &value, const int &multiplicity)
{
    ASSERT(m_initialized, "Histogram<T>::addAt, use of non initialized histogram");

    if (m_type == FREE)
    {
        m_map[n] += static_cast<double>(multiplicity);
        m_variable.set(value, multiplicity);
    }
    else
    {
        // Drop the values out of bounds.
        if (n < 0)
            m_under.set(value, multiplicity);
        else if (n >= m_nbin)
            m_over.set(value, multiplicity);
        else
        {
            m_bin[n]++;
            m_variable.set(value, multiplicity);
        }
    }
}

template <typename T> inline
bool Histogram<T>::compatible(const Histogram<T> h1, const Histogram<T> h2)
{
    return (
        h1.initialized() && h2.initialized() &&      // both should be initialized
        h1.type() == h2.type() &&                    // should have the same type
        h1.binLength() == h2.binLength() &&          // the same bin length
        h1.referenceValue() == h2.referenceValue()  // and the same reference value.
        ); 
}

template <typename T> inline
void Histogram<T>::join( Histogram &h)
{
    ASSERT(compatible(*this, h), "Histogram<T>::join, non compatible histograms");

    if (m_type == FREE)
    {
        for (auto const &it : h.m_map)
            m_map[it.first] += it.second;
    }
    else
    {
        m_bin += h.m_bin;
        m_under.join(h.m_under);
        m_over.join(h.m_over);
    }
    m_variable.join(h.m_variable);
}

template <typename T> inline
double Histogram<T>::mean()
{
    ASSERT(m_initialized, "Histogram<T>::mean, use of non initialized histogram");

    if (numberOfValues() == 0) return 0.0;

    double mean(0.0);
    double halfd = m_offset * m_d;

    if (m_type == FREE)
    {
        // sum = std::accumulate( m_map.cbegin(), m_map.cend(), 0.0,
        //     [](const int &c, const maptype::value_type &n ) { return (c+n.second);})
        // for (auto const &it : m_map)
        //     sum += it.second;
        for (auto const &it : m_map)
            mean += (m_min + it.first * m_d + halfd) * it.second;
    }
    else
    {
        // sum = m_bin.sum();
        for (int i = 0; i < m_nbin; i++)
            mean += (m_min + i * m_d + halfd) * m_bin[i];
    }
    // the normalization factor is 1/sum/m_d, but in mean value calculation we
    // should also multiply with m_d. therefore we just skip it in both places
    // and here we only divide with sum
    double factor = 1.0 / m_variable.N();
    return (mean * factor);
}

template <typename T> inline
double Histogram<T>::std()
{
    ASSERT(m_initialized, "Histogram<T>::std, use of non initialized histogram");

    double std(0.0);

    double v, t;
    double halfd = m_offset * m_d;
    double mean_ = mean();

    if (m_type == FREE)
    {
        for (auto const &it : m_map)
        {
            v = m_min + it.first * m_d + halfd;
            t = v - mean_;
            std += t * t * it.second;
        }
    }
    else
    {
        for (int i = 0; i < m_nbin; i++)
        {
            v = m_min + i * m_d + halfd;
            t = v - mean_;
            std += t * t * m_bin[i];
        }
    }
    // the normalization factor is 1/sum/m_d, but in mean value calculation we
    // should also multiply with m_d. therefore we just skip it in both places
    // and here we only divide with sum
    double factor = 1.0 / m_variable.N();
    return (sqrt(std * factor));
}

template <typename T> inline
void Histogram<T>::data(std::valarray<T> &x, std::valarray<T> &y, const bool &normalize, const bool &complete)
{
    ASSERT(m_initialized, "Histogram<T>::data, use of non initialized histogram");

    size_t n = numberOfBins(complete);
    x.resize(n, static_cast<T>(0));
    y.resize(n, 0.0);

    if (n == 0) return;

    double halfd = m_offset * m_d;
    double factor = normalize ? 1.0 / m_d / m_variable.N() : 1.0;

    if (m_type == FREE)
    {
        auto minbin = m_map.cbegin()->first;
        auto maxbin = m_map.crbegin()->first + 1;
        for (auto it = m_map.cbegin(); it != m_map.cend(); it++)
        {
            auto i = complete ? it->first - minbin : std::distance(m_map.cbegin(), it);
            x[i] = m_ref + static_cast<double>(it->first) * m_d + halfd;
            y[i] = it->second;
        }
        if (complete)
            for (int i = minbin; i < maxbin; i++) // update x values
                x[i - minbin] = m_ref + static_cast<double>(i) * m_d + halfd;
    }
    else
    {
        for (int i = 0; i < m_nbin; i++)
            x[i] = m_ref + static_cast<double>(i) * m_d + halfd;
        y = m_bin;
    }
    y *= factor;
}

template <typename T> inline
void Histogram<T>::write(FILE *file, const bool &normalize)
{
    ASSERT(m_initialized, "Histogram<T>::write, use of non initialized histogram");

    if (numberOfValues() == 0) return;

    // header
    fprintf(file, "# %s mean = %#.8g, std = %#.8g, norm = %s\n", m_name.data(), mean(), std(), normalize ? "true" : "false");

    double factor = normalize ? 1.0 / m_d / m_variable.N() : 1.0;
    double halfd = m_offset * m_d;
    if (m_type == FREE)
    {
        auto const &minbin = m_map.cbegin()->first;
        auto const &maxbin = m_map.crbegin()->first + 1;
        map<int, double>::const_iterator it;
        map<int, double>::const_iterator end = m_map.end();
        for (int i = minbin; i < maxbin; i++)
        {
            it = m_map.find(i);
            double value = (it != end) ? (*it).second * factor : 0.0;
            fprintf(file, "%#.8g %#.8g\n", m_ref + i * m_d + halfd, value);
        }
    }
    else
    {
        for (int i = 0; i < m_nbin; i++)
            fprintf(file, "%#.8g %#.8g\n", m_ref + i * m_d + halfd, m_bin[i] * factor);
    }
}

template <typename T> inline
void Histogram<T>::write(std::vector<std::string> &lines, const bool &normalize)
{
    ASSERT(m_initialized, "Histogram<T>::write, use of non initialized histogram");

    if (numberOfValues() == 0) return;

    int i;
    char buffer[100];
    std::string current;
    double factor = normalize ? 1.0 / m_d / m_variable.N() : 1.0 ;
    double halfd = m_offset * m_d;
    if (m_type == FREE)
    {
        auto const &minbin = m_map.cbegin()->first;
        auto const &maxbin = m_map.crbegin()->first + 1;
        map<int, double>::const_iterator it;
        map<int, double>::const_iterator end = m_map.end();
        for (i = minbin; i < maxbin; i++)
        {
            int j = i - minbin;
            it = m_map.find(i);
            double value = (it != end) ? (*it).second * factor : 0.0;
            sprintf(buffer, "%#.8g  %#.8g", m_ref + i * m_d + halfd, value);
            current = string(buffer);
            lines[j].append(current);
        }
        for (int j = maxbin - minbin; j < lines.size(); j++)
            lines[j].append("                                ");
    }
    else
    {
        for (i = 0; i < m_nbin; i++)
        {
            sprintf(buffer, "%#.8g  %#.8g", m_ref + i * m_d + halfd, m_bin[i] * factor);
            current = string(buffer);
            lines[i].append(current);
        }
        for (i = m_nbin; i < lines.size(); i++)
            lines[i].append("                                ");
    }
}

template <typename T> inline
void Histogram<T>::reset()
{
    if (m_type == FREE)
    {
        m_map.clear();
    }
    else
    {
        m_bin = static_cast<T>(0);
    }
    m_variable.reset();
}

template <typename T> inline
size_t Histogram<T>::numberOfBins(const int &complete) const
{
    size_t n(0);
    if (m_initialized)
    {
        if (m_type == FREE)
        {
            n = m_map.size();
            if (complete && n != 0)
            {
                auto b = binRange();
                n = b.second - b.first;
            }
        }
        else
            n = m_nbin;
    }
    return (n);
}

template <typename T> inline
std::pair<int, int> Histogram<T>::binRange() const
{
    std::pair<int, int> p(0, 0);
    if (m_initialized)
    {
        if (m_type == FIXED)
        {
            p.first = 0;
            p.second = m_nbin;
        }
        else
        {
            if (m_map.size() != 0)
            {
                p.first = m_map.cbegin()->first;
                p.second = m_map.crbegin()->first + 1;
            }
        }
    }
    return (p);
}

template <typename T> inline
std::pair<T, T> Histogram<T>::binValueRange() const
{
    std::pair<T, T> p(static_cast<T>(0), static_cast<T>(0));
    if (m_initialized)
    {
        if (m_type == FIXED)
        {
            p.first = m_min;
            p.second = m_max;
        }
        else
        {
            if (m_map.size() != 0)
            {
                p.first = m_ref + m_map.cbegin()->first * m_d;
                p.second = m_ref + (m_map.crbegin()->first + 1.0) * m_d;
            }
        }
    }
    return (p);
}

template <typename T> inline
std::pair<T, T> Histogram<T>::valueRange() const
{
    return (std::pair<T, T>(m_variable.min(), m_variable.max()));
}

} // namespace Math
