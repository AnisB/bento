// bento includes
#include "bento_tools/statistics.h"

// system includes
#include <algorithm>

namespace bento
{
    void evaluate_avg_med_stddev(uint64_t* first, uint64_t* end, uint64_t numElements, uint64_t& average, uint64_t& median, uint64_t& standardDeviation)
    {
	    // Sort the timings
	    std::sort(first, end, std::greater<uint64_t>());

		// Pick the median
		median = first[numElements / 2];

	    // Compute the average timing
		average = 0;
	    for (int i = 0; i < numElements; ++i)
	    {
			average += first[i];
	    }
		average /= numElements;

	    // Compute the standard deviation
		standardDeviation = 0;
	    for (int i = 0; i < numElements; ++i)
	    {
			standardDeviation += (first[i] - average) * (first[i] - average);
	    }
		standardDeviation = (uint64_t)sqrt(standardDeviation / (double)numElements);
    }

    void evaluate_avg_med_stddev(float* first, float* end, uint64_t numElements, float& average, float& median, float& standardDeviation)
    {
	    // Sort the timings
	    std::sort(first, end, std::greater<float>());

		// Pick the median
		median = first[numElements / 2];

	    // Compute the average timing
		average = 0;
	    for (int i = 0; i < numElements; ++i)
	    {
			average += first[i];
	    }
		average /= numElements;

	    // Compute the standard deviation
		standardDeviation = 0;
	    for (int i = 0; i < numElements; ++i)
	    {
			standardDeviation += (first[i] - average) * (first[i] - average);
	    }
		standardDeviation = sqrtf(standardDeviation / (float)numElements);
	}
}