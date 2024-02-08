// Imported from lpem project (https://github.com/loudove/lpem) 
// under the MIT license
// Αuthor: Loukas D. Peristeras (l.peristeras@inn.demokritos.gr)

#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <string>
#include <utility>
#include <valarray>
#include <vector>
#include <map>
#include <utility>

#include "variable.h"

namespace Math
{

// LDP TODO consider using template type for the data and the indexing

/// Implements a 1D histogram. Two types of histograms are supported:
/// - \p FIXED The histogram has a defined range and all the added values that do no belong
///       to this range are droped.
/// - \p FREE The histogram has a variable range ajusted automaticaly during the addition of
///       a new value.
template <typename T = double>
class Histogram
{
public:
    /// Enumerate Histogram types.
    enum Type
    {
        FIXED,
        FREE,
        UNINITIALIZED
    };

    /// Construct a Histogram object.
    Histogram(const std::string &name = "");

    /// Destroy a Histogram object.
    ~Histogram();

    /// Return true if histograms \p h1 and \p h2  are compatible.
    static bool compatible(Histogram<T> h1, Histogram<T> h2);

    /// Initialize a FIXED</em> type histogram by setting the objervation range
    /// to [ \p min, \p max ] and the number of binds to \p nbin.
    void init(const T &min, const T &max, const int &nbin);

    /// Initialize a \p FIXED type histogram by setting the objervation range
    /// to [ \p min, \p max ] and the length of each bin is \p d.
    void init(const T &min, const T &max, const double &d);

    /// Initialize a \p FREE type histogram by setting the length of the bins
    /// to \p d, and the reference (zero) \p value for the indexing.
    void init(const double &d, const T &value, const bool &addit = false);

    /// Set the \p offset for the bins.
    void setOffset(const double &offset);

    /// Add the \p value with the given  \p multiplicity to the histogram.
    void add(const T &value, const int &multiplicity = 1);

    /// Add the \p value with the given \p multiplicity at the \p n histogram bin.
    void addAt(const int &n, const T &value, const int &multiplicity = 1);

    /// Add the given histogram \p h to this histogram.
    void join( Histogram &h);

    /// Get the mean value.
    double mean();

    /// Get the std value.
    double std();

    /// Reset the histogram by seting to zero all the statistics.
    void reset();

    /// Return a copy of the histogram data ( \p x, \p y values). If \p complete 
    /// is true then the data of the FREE histogram will be converted to a data of the
    /// REGULAR counterpart.
    void data(std::valarray<T> &x, std::valarray<T> &y, const bool &normalize, const bool &complete = true);

    /// Return the total number of values added.
    const int &numberOfValues() const { return (m_variable.N()); }

    /// Return the name of the histogram.
    const std::string &name() const { return (m_name); }

    /// Return the type of the histogram.
    const Type &type() const { return (m_type); }

    /// Return the bin length.
    const T &binLength() const { return (m_d); }

    /// Retrun the reference value.
    const T &referenceValue() const { return (m_ref); }

    /// Return the underlying variable.
    const Variable<T> &variable() const { return (m_variable); }

    /// Return the offset for bin.
    const double &offset() const { return (m_offset); }

    /// Return true if the histogram is initialized correctly and it is ready for use.
    const bool &initialized() const { return (m_initialized); }

    /// Return the number of bins. For histograms of tyep FREE if \p complete is
    /// true the missing bins will be encountered.
    size_t numberOfBins(const int &complete) const;

    /// Return a std::pair with the minimum (fist) and maximum (second) indexes of the histogram's bins.
    std::pair<int,int> binRange() const;

    /// Return a std::pair with the minimum (fist) and maximum (second) values of the histogram's bins.
    std::pair<T,T> binValueRange() const;

    /// Return a std::pair with the minimum (fist) and maximum (second) recorded values.
    std::pair<T,T> valueRange() const;

    /// Write the histogram data in a file with optional normalization performed (default).
    void write(FILE *file, const bool &normalize);

    /// Write the histogram data in a string array lines with optional normalization performed (default).
    void write(std::vector<std::string> &lines, const bool &normalize);

protected:

    Type m_type;            ///< Histogram Type.
    T m_ref;                ///< Reference (base) value for FREE histograms.
    T m_d;                  ///< Length of bin.

    T m_min;                ///< Minimum limit of objervation values for FIXED histograms.
    T m_max;                ///< Maximum limit of objervation values for FIXED histograms.
    int m_nbin;             ///< Number of bins.
    std::valarray<T> m_bin; ///< Fixed Histogram data.

    typedef std::map<int, double> maptype;
    std::map<int, T> m_map; ///< Free Histogram data.

    T m_offset;             ///< The bin offset used in writing.
    bool m_initialized;     ///< Flag indicating the correct initialization.
    std::string m_name;     ///< Histogram name.

    Variable<T> m_variable; ///< Histogram underlying variable.
    Variable<T> m_under;    ///< underflow values
    Variable<T> m_over;     ///< overflow valules
};

} // namespace Math

#include "histogram.inl"

#endif
