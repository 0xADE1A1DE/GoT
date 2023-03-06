#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <cstdint>
#include <string>
#include <cmath>
#include <unordered_map>
#include <deque>
#include <queue>

#include <unistd.h>
#include <signal.h>

#include <thread>
#include <atomic>
#include <chrono>
#include <mutex>

#include <smmintrin.h>
#include <immintrin.h>

using namespace std;

atomic<bool> sigint_caught;

void sigint_handler(int s)
{
  sigint_caught = true;
  return;
}

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
static vector<s_chunks> chunkVector;

static unordered_map<int64_t, vector<double>> majorityMap;

static vector<atomic<bool>> *threadsRunSignal_p;
static vector<atomic<uint64_t>> *threadsStartIndex_p;
static vector<atomic<uint64_t>> *threadsEndIndex_p;

static atomic<int64_t> max_matches;
static atomic<int64_t> max_matches_start_index;
static atomic<int64_t> max_matches_chunk_index;

static atomic<int64_t> whole_start;
static atomic<int64_t> whole_end;







#define TARGET_MATCH 27

void find_best_match(uint64_t thread_id)
{
  while (true)
  {
    
    while (true)
    {
      
      if ((*threadsRunSignal_p)[thread_id])
      {
        
        break;
      }
      if (sigint_caught)
        return;
      
    }
    int64_t local_max_matches = 0;
    int64_t local_max_matches_start_index = 0;
    int64_t local_max_matches_chunk_index = 0;

    
    const uint64_t thread_start_index = (*threadsStartIndex_p)[thread_id];
    const uint64_t thread_end_index = (*threadsEndIndex_p)[thread_id];
    

#define VECTOR_SIZE 32
    const uint64_t vectorSize = (((whole_end - whole_start + 1) / VECTOR_SIZE) + 1) * VECTOR_SIZE;
    string currentMajorityVector;
    currentMajorityVector.resize(vectorSize);

    fill(currentMajorityVector.begin(), currentMajorityVector.end(), 'A');
    uint64_t vec_ctr = 0;
    for (int64_t i = whole_start; i < whole_end; i++, vec_ctr++)
    {
      const char currentSequence = (majorityMap[i][1] > majorityMap[i][0] ? 'm' : 'S');
      
      
      
      
      
      currentMajorityVector[vec_ctr] = currentSequence;
    }

    

    const uint64_t numberOfVectors = vectorSize / VECTOR_SIZE;
    vector<__m256i> currentMajorityVectors(numberOfVectors);
    for (int i = 0; i < numberOfVectors; i++)
    {
      currentMajorityVectors[i] = _mm256_loadu_si256((__m256i_u *)((uintptr_t)currentMajorityVector.data() +
                                                                   i * VECTOR_SIZE));
    }

    for (intptr_t i = thread_start_index; i < thread_end_index; i++)
    {
      
      const s_chunks chunk = chunkVector[i];
      

      
      
      
      string currentKeyvector;
      currentKeyvector.resize(2 * vectorSize + chunk.key.length());
      fill(currentKeyvector.begin(), currentKeyvector.end(), 'B');

      for (int64_t key_index = 0, vector_index = vectorSize; key_index < chunk.key.length(); key_index++,
                   vector_index++)
      {
        currentKeyvector[vector_index] = chunk.key[key_index];
      }

      for (int64_t start_index = whole_start, key_offset = 0;
           start_index < whole_end;
           start_index++, key_offset--)
      {
        
        int64_t matches = 0;

        ;
        for (int i = 0; i < numberOfVectors; i++)
        {
          
          __m256i currentKeyVectors = _mm256_loadu_si256((__m256i_u *)((uintptr_t)currentKeyvector.data() + vectorSize +
                                                                       i * VECTOR_SIZE + key_offset));
          __m256i results = _mm256_cmpeq_epi8(currentMajorityVectors[i],
                                              currentKeyVectors);

          
          matches += _mm_popcnt_u64(_mm256_extract_epi64(results, 0));
          matches += _mm_popcnt_u64(_mm256_extract_epi64(results, 1));
          matches += _mm_popcnt_u64(_mm256_extract_epi64(results, 2));
          matches += _mm_popcnt_u64(_mm256_extract_epi64(results, 3));
        }

        matches >>= 3;

        

        if (matches > local_max_matches)
        {
          local_max_matches = matches;
          local_max_matches_start_index = start_index;
          local_max_matches_chunk_index = i;
        }
        
        
        
      }
    }
    

    if (local_max_matches > max_matches)
    {
      max_matches = local_max_matches;
      max_matches_start_index = local_max_matches_start_index;
      max_matches_chunk_index = local_max_matches_chunk_index;
    }

    

    
    
    (*threadsRunSignal_p)[thread_id] = false;
    
  }
}

int main(int argc, char **argv)
{
  sigint_caught = false;

  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = sigint_handler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;

  sigaction(SIGINT, &sigIntHandler, NULL);

  ifstream inputIfs(argv[1]);

  whole_start = 0;
  whole_end = 0;

  
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
  uint64_t maxCount = 0;
  string maxSeq;
  
  
  if (argc < 2) {
    for (const s_chunks& chunk : chunkVectorOriginal) {
      stringCountMap[chunk.key]++;
    }
    
    
    for (const pair<string, uint64_t>& p : stringCountMap) {
      if (p.second > maxCount && p.first.length() >= 28 && p.first.find_last_of('.') == p.first.npos) {
        maxCount = p.second;
        maxSeq = p.first;
      }
    }
  cout << "First Sequence = " << maxSeq << " count: " << maxCount << endl;
  } else {
    maxSeq = string(argv[2]);
  }


  const s_chunks first_chunk(0,0,0, maxSeq);

  cout << "first sequence: " << first_chunk.key << endl;

  for (int index = 0; index < first_chunk.key.length(); index++)
  {
    if (majorityMap.find(index) == majorityMap.end())
    {
      majorityMap[index].push_back(0);
      majorityMap[index].push_back(0);
    }
    if (first_chunk.key[index] == 'S')
      majorityMap[index][0]++;
    else
      majorityMap[index][1]++;
  }
  whole_end = first_chunk.key.length();

  

  

  const uint32_t nThreads = thread::hardware_concurrency();
  

  cout << "#Threads = " << nThreads << endl;

  vector<atomic<bool>> threadsRunSignal(nThreads);
  threadsRunSignal_p = &threadsRunSignal;

  fill(threadsRunSignal.begin(), threadsRunSignal.end(),
       false);

  vector<atomic<uint64_t>> threadsStartIndex(nThreads);
  vector<atomic<uint64_t>> threadsEndIndex(nThreads);

  threadsStartIndex_p = &threadsStartIndex;
  threadsEndIndex_p = &threadsEndIndex;

  
  

  vector<thread> threadPool;

  for (uint64_t thread_id = 0; thread_id < nThreads; thread_id++)
  {
    threadPool.push_back(thread(find_best_match, thread_id));
  }

  while (!sigint_caught)
  {
    chunkVector = chunkVectorOriginal;
    uint64_t belowTargetMatch = 0;
    while (!chunkVector.empty() && !sigint_caught)
    {
      
      
      
      max_matches = 0;
      max_matches_chunk_index = 0;
      max_matches_start_index = 0;

      cout << "remaining: " << chunkVector.size() << " current status: " << endl;

      
      
      
      
      
      

      
      const uint64_t chunkSize = chunkVector.size() / nThreads;
      
      const uint64_t remainder = ((chunkSize > 0) ? chunkVector.size() % chunkSize : chunkVector.size());
      for (uint32_t thread_id = 0; thread_id < nThreads; thread_id++)
      {

        threadsStartIndex[thread_id] = chunkSize * thread_id;
        threadsEndIndex[thread_id] = chunkSize * (thread_id + 1) + ((thread_id == nThreads - 1) ? remainder : 0);
        threadsEndIndex[thread_id] = chunkSize * (thread_id + 1);
      }
      for (uint32_t thread_id = 0; thread_id < nThreads; thread_id++)
      {
        threadsRunSignal[thread_id] = true;
      }
      

      while (true)
      {
        bool allDone = true;
        for (uint32_t thread_id = 0; thread_id < nThreads; thread_id++)
        {
          
          if (threadsRunSignal[thread_id])
            allDone = false;
          
        }
        if (allDone)
          break;
      }

      

      
      

      const s_chunks &max_matches_chunk =
          chunkVector[max_matches_chunk_index];

{
      if (max_matches < TARGET_MATCH)
      {
         belowTargetMatch++;
      }         
      else
      {
        belowTargetMatch = 0;
      }
        const int64_t old_start = whole_start;
        const int64_t old_end = whole_end;

        whole_start = min((int64_t)whole_start, (int64_t)max_matches_start_index);
        whole_end = max((int64_t)whole_end, int64_t(max_matches_start_index + (int64_t)max_matches_chunk.key.length()));

        for (int space_idx = 0; space_idx < (old_start - whole_start); space_idx++)
          cout << " ";
        
        for (int64_t i = old_start; i < old_end; i++)
        {
          const vector<double> &vec = majorityMap[i];
          cout << (vec[0] >= vec[1] ? 'S' : 'm');
        }
        cout << endl;

        for (int space_idx = whole_start; space_idx < max_matches_start_index; space_idx++)
          cout << " ";

        cout << max_matches_chunk.key << endl;

        cout << "> Added " << max_matches_chunk.key << " starting at " << max_matches_start_index << " for " << max_matches << " matches"
             << "- start = " << whole_start << " - end = "
             << whole_end << endl;
             
        const int64_t weight = pow(2, (int64_t)max_matches);
        for (int64_t index = 0, map_index = max_matches_start_index;
             index < (int64_t)max_matches_chunk.key.length(); index++, map_index++)
        {
          if (majorityMap.find(map_index) == majorityMap.end())
          {
            majorityMap[map_index].push_back(0.0);
            majorityMap[map_index].push_back(0.0);
          }
          
          
          
          
          
          
          
          
          
          
          
          
          
          
          
          
          double newWeight;

          
          if (max_matches_chunk.key[index] == 'S') {
            majorityMap[map_index][0] += max_matches;
          }
          else if (max_matches_chunk.key[index] == 'm') { 
            majorityMap[map_index][1] += max_matches;
          }
        }
        chunkVector.erase(chunkVector.begin() + max_matches_chunk_index);
      
}
        
      
      
      
      
      
      
      
      
      
      
      
      
      
    }
    
    
    
    
    
  
  }

  for (uint64_t i = 0; i < threadPool.size(); i++)
  {
    threadPool[i].join();
  }
  threadPool.clear();

  uint64_t majorityCountTotal = 0;

  
  for (int64_t i = whole_start; i < whole_end; i++)
  {
    const vector<double> &vec = majorityMap[i];
    if (vec[0] > vec[1])
    {
      majorityCountTotal += vec[0];
    }
    else
    {
      majorityCountTotal += vec[1];
    }
  }
  const uint64_t majorityAverage = majorityCountTotal / majorityMap.size();

  
  
  
  
  
  

  for (int64_t i = whole_start; i < whole_end; i++)
  
  {
    const vector<double> &vec = majorityMap[i];
    const double sConfidence = vec[0] / (double)majorityAverage;
    const double mConfidence = vec[1] / (double)majorityAverage;

    
    if (sConfidence + mConfidence < 0.4)
      cout << "X";

    
    else if (abs(sConfidence - mConfidence) < 0.3)
      cout << "A";
    else
      cout << (vec[0] >= vec[1] ? 'S' : 'm');
  }
  cout << endl;
}