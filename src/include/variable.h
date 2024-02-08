// Imported from lpem project (https://github.com/loudove/lpem) 
// under the MIT license
// Αuthor: Loukas D. Peristeras (l.peristeras@inn.demokritos.gr)

#ifndef VARIABLE_H
#define VARIABLE_H

namespace Math
{

template <typename T = double>
struct SimpleStatistics
{
  T mean;
  T std;
  T min;
  T max;
};

/// A class representing a simple variable.
template <typename T = double>
class Variable
{
public:
  /// Constructor.
  Variable();

  /// Constructor with initial value.
  Variable(const T &value);

  /// Destructor.
  ~Variable();

  /// Set the value of the variable to its new \p value.
  template <typename T1>
  Variable<T> &operator=(T1 value);

  /// Set the value of the variable to its new \p value with the given \p multiplicity.
  template <typename T1>
  void set(const T1 &value, const int &multiplicity = 1);

  /// Set the value to zero and also reset the statistics.
  void reset();

  /// Update the value and the statistics of \p this Variable with
  /// the given \p v variable.
  void join(Variable<T> &v);

  /// Get the current value of the variable.
  const T &value() const;
  const T &operator()() const;

  /// Get the number of values set for the variable.
  const int &N() const { return (m_N); }

  /// Get the cumulative sum of the variable values.
  const T &cumulative() const { return (m_cumulative); }

  //c Get the cumulative sum of the variable square values.
  const T &sqcumulative() const { return (m_sqcumulative); }

  /// Get the mean value of the variable.
  double mean() const;

  /// Get the variance of the variable.
  double std() const;

  /// Get min value set.
  const T &min() const { return (m_min); }

  /// Get max value set.
  const T &max() const { return (m_max); }

private:
  T m_value;        ///< Current value.
  int m_N;          ///< Number of values set.
  T m_cumulative;   ///< Sum of values set.
  T m_sqcumulative; ///< Sum of values square set.
  T m_min;          ///< Minimum value set.
  T m_max;          ///< Maximum value set.
  void _update();   /// <After a new value has been set perform updates.
};

} // namespace Math

#include "variable.inl"

#endif // VARIABLE_H
