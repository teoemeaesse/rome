#include <gtest/gtest.h>

#include "rm/debug/exception.hpp"
#include "rm/debug/metrics.hpp"

using namespace rome;
using namespace rome::core;

TEST(Metrics, RegisterThread_CurrentThread_OK) {
    Metrics& metrics = Metrics::getInstance();
    if (metrics.isRegistered()) metrics.unregisterThread();

    metrics.registerThread("MetricsTest");

    EXPECT_TRUE(metrics.isRegistered());
    EXPECT_EQ(metrics.getThreadAlias(), "MetricsTest");
    EXPECT_FALSE(metrics.isMemoryTracking());

    metrics.unregisterThread();
    EXPECT_FALSE(metrics.isRegistered());
}

TEST(Metrics, RegisterAllocation_WhenRegistered_TracksCounters) {
    Metrics& metrics = Metrics::getInstance();
    if (metrics.isRegistered()) metrics.unregisterThread();
    metrics.registerThread("MetricsAllocationTest");

    int value = 0;
    metrics.registerAllocation(&value, 16);

    EXPECT_EQ(metrics.getCurrentBytes(), 16);
    EXPECT_EQ(metrics.getPeakBytes(), 16);
    EXPECT_EQ(metrics.getTotalBytes(), 16);
    EXPECT_EQ(metrics.getTotalAllocations(), 1);
    EXPECT_EQ(metrics.getMissingDeallocations(), 1);

    metrics.registerDeallocation(&value);

    EXPECT_EQ(metrics.getCurrentBytes(), 0);
    EXPECT_EQ(metrics.getPeakBytes(), 16);
    EXPECT_EQ(metrics.getTotalBytes(), 16);
    EXPECT_EQ(metrics.getTotalAllocations(), 1);
    EXPECT_EQ(metrics.getMissingDeallocations(), 0);

    metrics.unregisterThread();
}

TEST(Metrics, Getters_WhenUnregistered_Throw) {
    Metrics& metrics = Metrics::getInstance();
    if (metrics.isRegistered()) metrics.unregisterThread();

    EXPECT_THROW(metrics.getCurrentBytes(), Exception);
    EXPECT_THROW(metrics.getThreadAlias(), Exception);
    EXPECT_THROW(metrics.isMemoryTracking(), Exception);
}
