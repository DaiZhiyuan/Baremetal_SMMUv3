#ifndef __SMMU_TEST_H__
#define __SMMU_TEST_H__

#include <stdint.h>

enum smmu_test_mode {
    SMMU_TEST_MODE_S1 = 1,
    SMMU_TEST_MODE_S2 = 2,
    SMMU_TEST_MODE_S1S2 = 3,
};

void smmu_test_run(enum smmu_test_mode mode);

#endif /* __SMMU_TEST_H__ */
