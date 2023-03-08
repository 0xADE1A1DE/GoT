#include "retention/retention.h"
#include "ev/assisted_ev.h"

namespace retention {

template<class paramaters>
void RetentionScheme<paramaters>::initialize(uint64_t &trash) {
    ev::AssistedEvictionSetManager ev_manager;
    m_eviction_sets = ev_manager.find_eviction_sets((paramaters::retention_sets * paramaters::per_ev + LLC_CACHE_ASSOCIATIVITY - 1) / LLC_CACHE_ASSOCIATIVITY, paramaters::number_of_ev);
}

template<class paramaters>
uintptr_t RetentionScheme<paramaters>::get_page(int page_index) {
    // We assume 0 < page_index < number_of_ev * per_ev;

    int ev_index = page_index % paramaters::interleave + page_index / (paramaters::interleave * paramaters::per_ev);
    int in_ev_index = (page_index / paramaters::interleave) % paramaters::per_ev;
    // in_ev_index += TODO: take retention set index into consideration.
    return m_eviction_sets[ev_index][in_ev_index / LLC_CACHE_ASSOCIATIVITY].arr[in_ev_index % LLC_CACHE_ASSOCIATIVITY];
}

template<class paramaters>
uintptr_t RetentionScheme<paramaters>::get_entry(int index) {
    int page_index = index % (paramaters::number_of_ev * paramaters::per_ev);
    int page_offset_index = index / (paramaters::number_of_ev * paramaters::per_ev);
    int page_offset = paramaters::page_offsets[page_offset_index];
    return get_page(index) + page_offset;
}

template<class paramaters>
bool RetentionScheme<paramaters>::measure(int index, uint64_t &trash) {
}

template<class paramaters>
void RetentionScheme<paramaters>::retain(uint64_t &trash) {
}

}