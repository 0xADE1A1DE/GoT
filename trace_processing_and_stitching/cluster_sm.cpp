#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <cstdint>
#include <string>

using namespace std;

struct s_chunks
{
  uint64_t stringLength;
  uint64_t matchLength;
  uint64_t matchPos;
  string key;

  s_chunks(uint64_t stringLength, uint64_t matchLength,
           uint64_t matchPos, string key) : stringLength(stringLength),
                                            matchLength(matchLength), matchPos(matchPos), key(key){};
};

int minDis(string s1, string s2, int n, int m, vector<vector<int>> &dp){
         
  
  
           
  if(n==0)    return m;
         
  if(m==0)    return n;
         
           
  
  
         
  if(dp[n][m]!=-1)    return dp[n][m];
           
           
  
  
         
  if(s1[n-1]==s2[m-1]){          
    if(dp[n-1][m-1]==-1){              
      return dp[n][m] = minDis(s1, s2, n-1, m-1, dp);          
    }       
    else
      return dp[n][m] = dp[n-1][m-1];  
  }
   
         
  
           
  
         
  else{          
    int m1, m2, m3;        
     
    if(dp[n-1][m]!=-1){   
      m1 = dp[n-1][m];     
    }          
    else{  
      m1 = minDis(s1, s2, n-1, m, dp);     
    }           
             
    if(dp[n][m-1]!=-1){               
      m2 = dp[n][m-1];           
    }           
    else{   
      m2 = minDis(s1, s2, n, m-1, dp);     
    }                                  
    
    if(dp[n-1][m-1]!=-1){   
      m3 = dp[n-1][m-1];     
    }  
    else{  
      m3 = minDis(s1, s2, n-1, m-1, dp);      
    }    
    return dp[n][m] = 1+min(m1, min(m2, m3));       
  }
   
}
 


uint64_t editDistance(const string s1, const string s2,
                      uint64_t s1Index, uint64_t s2Index) {

  vector<vector<int>> dp (s1.length() + 1, vector<int>(s2.length() + 1, - 1));
  
  return minDis(s1, s2, s1.length(), s2.length(), dp);
  
}

int main(int argc, char **argv)
{
  uint64_t keyLength = 457;

  cerr << "keyLength = " << keyLength << endl;

  ifstream inputIfs(argv[1]);

  vector<s_chunks> chunkVector;

  while (!inputIfs.eof())
  {
    string line;
    getline(inputIfs, line);

    if (line == "")
      break;

    if (line.front() == '>')
      continue;
      
    if (line.find_first_of('.') != line.npos)
      continue;
    

    stringstream sstream(line);

    uint64_t unused;
    uint64_t stringLength;
    uint64_t matchLength;
    uint64_t matchPos;
    string key;

    sstream >> unused;
    sstream >> stringLength;
    sstream >> matchLength;
    sstream >> matchPos;
    sstream >> key;
    
    if (key.length() < 25)
      continue;

    chunkVector.emplace_back(stringLength,
                             matchLength,
                             matchPos,
                             key);
  }
  
  map<uint64_t, vector<string>> clusterMap;
  
  uint64_t clusterCount = 0;
  
#define DISTANCE_THRESHOLD 4
  while (!chunkVector.empty()) {
    cerr << chunkVector.size() << endl;
    const string refString = chunkVector.front().key;
    chunkVector.erase(chunkVector.begin());
    
    clusterMap[clusterCount].push_back(refString);
    
    for (uint64_t i = 0; i < chunkVector.size(); i++) {
      const uint64_t distance = editDistance(refString, chunkVector[i].key,
                                             refString.length() - 1, chunkVector[i].key.length() - 1);
      
      
      
      if (distance <= DISTANCE_THRESHOLD) {
        clusterMap[clusterCount].push_back(chunkVector[i].key);
        chunkVector.erase(chunkVector.begin() + i);
        i--;
      }
      
    }
    clusterCount++;
  }
  
  vector<uint64_t> clusterLength;
  for (const pair<uint64_t, vector<string>>& p : clusterMap) {
    clusterLength.push_back(p.second.size());
  }
  
  sort(clusterLength.begin(), clusterLength.end(),
       [&](const uint64_t len1, const uint64_t len2) {
         return len1 < len2;
       });
  
  const uint64_t targetClusterLength = clusterLength[clusterLength.size() * 0.85];
  
  cerr << "0.85 cluster length = " << targetClusterLength << endl;
  
  uint64_t ctr = 0;
for (const pair<uint64_t, vector<string>>& p : clusterMap) {
  if (p.second.size() >= targetClusterLength) {
  
  for (const string& s : p.second) {
    cout << 0 << " " << 0 << " " << 0 << " " << ctr << " " << s << endl;
  }
  ctr++;
  }

}




























  
  
}
