#pragma once
#include "ev/ev.h"
#include "measure/measure.h"
#include <stdbool.h>
#include <stdint.h>

namespace retention {

template<measure::measure_type MEASURE_UNITS, class BUFFERGATE, class SHRINKGATE, int NUMBER_OF_EV, int PER_EV, int RETENTION_SETS, int SHRINK_FACTOR, int INTERLEAVE, bool used_double_cache_lines[32]>
class RetentionParamaters {
    constexpr RetentionParamaters() {
        int page_offset_idx = 0;
        for (int i = 0; i < 32; i++) {
            if (used_double_cache_lines[i])
                page_offsets[page_offset_idx++] = i * DOUBLE_CACHE_LINE_SIZE;
        }
        amount_of_used_double_cache_lines = page_offset_idx;

        total_retention_entries = amount_of_used_double_cache_lines * NUMBER_OF_EV * PER_EV;
        total_measure_entries = total_retention_entries / SHRINK_FACTOR;
    }

    uint32_t page_offsets[32];
    int amount_of_used_double_cache_lines;
    int total_retention_entries;
    int total_measure_entries;

    using MeasureClass = measure::AmplificationMeasure<MEASURE_UNITS>;
    static constexpr BUFFERGATE buffer;
    static constexpr SHRINKGATE shrink;
    static constexpr int number_of_ev = NUMBER_OF_EV;
    static constexpr int per_ev = PER_EV;
    static constexpr int retention_sets = RETENTION_SETS;
    static constexpr int shrink_factor = SHRINK_FACTOR;
    static constexpr int interleave = INTERLEAVE;
};

template<class paramaters>
class RetentionScheme {
public:
    RetentionScheme(amplification::Amplification *amp[2]) {
        for (int i = 0; i < 2; i++)
            m[i] = paramaters::MeasureClass(*amp[i]);
    }

    virtual void initialize(uint64_t &trash);
    uintptr_t get_entry(int index);
    bool measure(int index, uint64_t &trash);

    void retain(uint64_t &trash);

private:
    uintptr_t get_page(int page_index);
    typename paramaters::MeasureClass m[2];
    ev::EvictionSet **m_eviction_sets;
    int m_current_measure_index;
};

}