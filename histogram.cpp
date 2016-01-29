#include "histogram.h"
#include <cstdlib>

template <class T>
void histogram<T>::add(const T& inst)
{
  freqtable[inst]++;
}

template <class T>
void histogram<T>::compile()
{
  distribution.clear();
  
  int max = 0;
  for (auto& it : freqtable)
  {
    max += it.second;
    distribution.emplace(max, it.first);
  }
  
  freqtable.clear();
}

template <class T>
const T& histogram<T>::next() const
{
  int max = distribution.rbegin()->first;
  int r = rand() % max;
  
  return distribution.upper_bound(r)->second;
}

template class histogram <std::string>;
