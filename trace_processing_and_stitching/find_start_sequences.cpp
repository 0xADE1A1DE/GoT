#include <string>
#include <iostream>
#include <algorithm>
#include <map>
#include <vector>
#include <cstdint>
#include <sstream>
#include <fstream>

using namespace std;

struct s_chunks
{
  uint64_t stringLength;
  uint64_t matchLength;
  uint64_t matchPos;
  string key;

  s_chunks() {}
  s_chunks(uint64_t stringLength, uint64_t matchLength,
           uint64_t matchPos, string key) : stringLength(stringLength),
                                            matchLength(matchLength), matchPos(matchPos), key(key){};
};

static vector<s_chunks> chunkVectorOriginal;

int main(int argc, char** argv) {
  ifstream inputIfs(argv[1]);
  
  while (!inputIfs.eof())
  {
    string line;
    getline(inputIfs, line);

    if (line == "")
      break;

    if (line.front() == '>')
      continue;

    
    

    stringstream sstream(line);

    uint64_t not_read;
    uint64_t stringLength;
    uint64_t matchLength;
    uint64_t matchPos;
    string key;

    sstream >> not_read;
    sstream >> stringLength;
    sstream >> matchLength;
    sstream >> matchPos;
    sstream >> key;

    
    int64_t maxContinuousS = 0;
    int64_t currentMax = 0;
    for (const char c : key)
    {
      if (c == 'S')
        currentMax++;
      else
        currentMax = 0;

      maxContinuousS = max(currentMax, maxContinuousS);
    }

    if (maxContinuousS >= 12)
      continue;

    chunkVectorOriginal.emplace_back(stringLength,
                                     matchLength,
                                     matchPos,
                                     key);
  }
  
  map<string, uint64_t> stringCountMap;
  map<uint64_t, vector<string>> frequencyStringMap;
  
  
    for (const s_chunks& chunk : chunkVectorOriginal) {
      stringCountMap[chunk.key]++;
    }
    
    for (const pair<string, uint64_t>& p : stringCountMap) {
      frequencyStringMap[p.second].push_back(p.first);
    }
    
    
    vector<pair<uint64_t, vector<string>>> vec(frequencyStringMap.begin(),
                                               frequencyStringMap.end());
    sort(vec.begin(), vec.end(),
         [](const pair<uint64_t, vector<string>>& p1,
            const pair<uint64_t, vector<string>>& p2) {
              return p1.first > p2.first;
            });
    
    for (const pair<uint64_t, vector<string>>& p : vec) {
      cout << "### " << p.first << " Matches ###" << endl;
      for (const string& s : p.second) {
        cout << s << endl;
      }
    }
  
}