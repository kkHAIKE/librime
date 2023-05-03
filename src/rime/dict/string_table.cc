//
// Copyright RIME Developers
// Distributed under the BSD License
//
// 2014-07-04 GONG Chen <chen.sst@gmail.com>
//

#include <rime/common.h>
#include <rime/dict/string_table.h>

namespace rime {

StringTable::StringTable(const char* ptr, size_t size) {
  trie_.map(ptr, size);
}

bool StringTable::HasKey(const string& key) {
  marisa::Agent agent;
  agent.set_query(key.c_str());
  return trie_.lookup(agent);
}

StringId StringTable::Lookup(const string& key) {
  marisa::Agent agent;
  agent.set_query(key.c_str());
  if(trie_.lookup(agent)) {
    return agent.key().id();
  }
  else {
    return kInvalidStringId;
  }
}

void StringTable::CommonPrefixMatch(const string& query,
                                    vector<StringId>* result) {
  marisa::Agent agent;
  agent.set_query(query.c_str());
  result->clear();
  while (trie_.common_prefix_search(agent)) {
    result->push_back(agent.key().id());
  }
}

void StringTable::Predict(const string& query,
                          vector<StringId>* result) {
  marisa::Agent agent;
  agent.set_query(query.c_str());
  result->clear();
  while (trie_.predictive_search(agent)) {
    result->push_back(agent.key().id());
  }
}

string StringTable::GetString(StringId string_id) {
  marisa::Agent agent;
  agent.set_query(string_id);
  try {
    trie_.reverse_lookup(agent);
  }
  catch (const marisa::Exception& /*ex*/) {
    LOG(ERROR) << "invalid id for string table: " << string_id;
    return string();
  }
  return string(agent.key().ptr(), agent.key().length());
}

size_t StringTable::NumKeys() const {
  return trie_.size();
}

size_t StringTable::BinarySize() const {
  return trie_.io_size();
}

void StringTableBuilder::Add(const string& key,
                             double weight,
                             StringId* reference) {
  keys_.push_back(key.c_str(), key.length(), (float)weight);
  references_.push_back(reference);
}

void StringTableBuilder::Clear() {
  trie_.clear();
  keys_.clear();
  references_.clear();
}

void StringTableBuilder::Build() {
  trie_.build(keys_);
  UpdateReferences();
}

void StringTableBuilder::UpdateReferences() {
  if (keys_.size() != references_.size()) {
    return;
  }
  marisa::Agent agent;
  for (size_t i = 0; i < keys_.size(); ++i) {
    if (references_[i]) {
      *references_[i] = keys_[i].id();
    }
  }
}

namespace detail {
  // https://stackoverflow.com/questions/1494182/setting-the-internal-buffer-used-by-a-standard-stream-pubsetbuf
  template <typename char_type>
  struct ostreambuf : public std::basic_streambuf<char_type, std::char_traits<char_type> >
  {
      ostreambuf(char_type* buffer, std::streamsize bufferLength)
      {
          // set the "put" pointer the start of the buffer and record it's length.
          this->setp(buffer, buffer + bufferLength);
      }
  };
}

void StringTableBuilder::Dump(char* ptr, size_t size) {
  if (size < BinarySize()) {
    LOG(ERROR) << "insufficient memory to dump string table.";
    return;
  }

  detail::ostreambuf<char> ostreamBuffer(ptr, size);
  std::ostream stream(&ostreamBuffer);
  stream << trie_;
}

}  // namespace rime
