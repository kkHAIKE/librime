#ifndef RIME_UTIL_H_
#define RIME_UTIL_H_

#include <vector>
#include <string>

namespace rime {

template<typename StringVec, typename String>
void Split(StringVec& result, const String& s,
                                      char delim) {
  if (s.empty()) {
    result.emplace_back();
    return;
  }
  size_t pos = 0;
  while (pos < s.length()) {
    auto next = s.find(delim, pos);
    if (next == std::string::npos) {
      result.emplace_back(s.begin() + pos, s.end());
      break;
    }
    result.emplace_back(s.begin() + pos, s.begin() + next);
    pos = next + 1;
  }
}

template <typename StringVec>
std::string Join(const StringVec& parts,
                        const std::string_view& delim) {
  std::string result;
  bool flag{};
  for (const auto& part : parts) {
    if (flag) {
      result.append(delim);
    } else {
      flag = true;
    }
    result.append(part);
  }
  return result;
}

inline void Trim(std::string& s) {
  auto first = std::find_if_not(s.begin(), s.end(), isspace);
  auto last = std::find_if_not(s.rbegin(), s.rend(), isspace);
  if (first <= last.base()) {
    s.erase(last.base(), s.end());
    s.erase(s.begin(), first);
  } else {
    s.clear();
  }
}

class ScopeExit {
public:
  template<typename F>
  ScopeExit(F&& f) : f_(std::forward<F>(f)) {}
  ~ScopeExit() { f_(); }
private:
  std::function<void()> f_;
};

}

#endif
