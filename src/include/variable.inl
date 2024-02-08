// Imported from lpem project (https://github.com/loudove/lpem) 
// under the MIT license
// Αuthor: Loukas D. Peristeras (l.peristeras@inn.demokritos.gr)

#include <iostream>
#include <math.h>
#include <limits>

#include "variable.h"

namespace Math
{

template <typename T> inline
Variable<T>::Variable()
{
  m_value = static_cast<T>(0.0);
  m_N = 0;
  m_cumulative = static_cast<T>(0.0);
  m_sqcumulative = static_cast<T>(0.0);
  m_min = std::numeric_limits<T>::max();
  m_max = std::numeric_limits<T>::min();
  // std::cout << "variable was initialized to zero..." << std::endl;

}

template <typename T> inline
Variable<T>::Variable(const T &value)
{
  m_value = value;
  m_N = 1;
  m_cumulative = m_value;
  m_sqcumulative = m_value * m_value;
  _update();
  // std::cout << "variable was initialized ..." << std::endl;
}

template <typename T> inline
Variable<T>::~Variable()
{
  // std::cout << "variable was destroyed ..." << std::endl;
}

template <typename T>
template <typename T1> inline
Variable<T> &Variable<T>::operator=(T1 value)
{
  set((T)value);
  return (*this);
}

template <typename T>
template <typename T1> inline
void Variable<T>::set(const T1 &value, const int &multiplicity)
{
  m_value = static_cast<T>(value);
  m_N += multiplicity;
  T _v = multiplicity * m_value;
  m_cumulative += _v;
  m_sqcumulative += _v * _v;
  _update();
}

template <typename T> inline
void Variable<T>::reset()
{
  *this = Variable<T>();
}

template <typename T> inline
void Variable<T>::join( Variable<T> &v)
{
  m_value += v.m_value;
  m_N += v.m_N;
  m_cumulative += v.m_cumulative;
  m_sqcumulative += v.m_sqcumulative;
  m_min = std::min(m_min,v.m_min);
  m_max = std::max(m_max,v.m_max);
}

template <typename T> inline
const T &Variable<T>::value() const
{
  return (m_value);
}

template <typename T> inline
const T &Variable<T>::operator()() const
{
  return (m_value);
}

template <typename T>
double Variable<T>::mean() const
{
  double m = 0.0;
  if (m_N != 0)
    m = static_cast<double>(m_cumulative) / static_cast<double>(m_N);
  return (m);
}

template <typename T> inline
double Variable<T>::std() const
{
  double std = 0.0;
  if (m_N > 1)
  {
    double s = static_cast<double>(m_sqcumulative) / static_cast<double>(m_N - 1);
    double m = static_cast<double>(m_cumulative) / static_cast<double>(m_N);
    double m_ = static_cast<double>(m_cumulative) / static_cast<double>(m_N - 1);
    std = s - m * m_;
    // We use abs in case where variable is constant and std could be -0.
    std = sqrt(fabs(std));
  }
  return (std);
}

template <typename T> inline
void Variable<T>::_update()
{
    if (m_value < m_min)
      m_min = m_value;
    if (m_value > m_max)
      m_max = m_value;
}

} // namespace Math
