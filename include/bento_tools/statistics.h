#pragma once

// bento includes
#include "bento_base/platform.h"

namespace bento
{
    // Function to evaluate the classic statistics values
    void evaluate_avg_med_stddev(uint64_t* first, uint64_t* end, uint64_t numElements, uint64_t& average, uint64_t& median, uint64_t& standardDeviation);
    void evaluate_avg_med_stddev(float* first, float* end, uint64_t numElements, float& average, float& median, float& standardDeviation);
}