#ifndef HISTOGRAM_H_24094D97
#define HISTOGRAM_H_24094D97

#include <map>
#include <string>
#include <random>
#include <iostream>

template <class T>
class histogram {
public:

  void add(const T& inst)
  {
    freqtable_[inst]++;
  }

  void compile()
  {
    distribution_.clear();

    int max = 0;
    for (auto& it : freqtable_)
    {
      max += it.second;
      distribution_.emplace(max, it.first);
    }

    freqtable_.clear();
  }

  const T& next(std::mt19937& rng) const
  {
    int max = distribution_.rbegin()->first;
    std::uniform_int_distribution<int> randDist(0, max - 1);
    int r = randDist(rng);

    return distribution_.upper_bound(r)->second;
  }

  void print() const
  {
    for (auto& freqpair : freqtable_)
    {
      std::cout << freqpair.first << ": " << freqpair.second << std::endl;
    }
  }

private:

  std::map<T, int> freqtable_;
  std::map<int, T> distribution_;
};

#endif /* end of include guard: HISTOGRAM_H_24094D97 */
