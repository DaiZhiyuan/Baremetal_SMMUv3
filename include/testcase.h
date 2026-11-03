#ifndef __TESTCASE_H__
#define __TESTCASE_H__

extern void dump_smmu_idr0(void);
extern void dump_smmu_idr1(void);
extern void dump_smmu_idr2(void);
extern void dump_smmu_idr3(void);
extern void dump_smmu_idr4(void);
extern void dump_smmu_idr5(void);
extern void dump_smmu_idr6(void);
extern void dump_smmu_iidr(void);
extern void dump_smmu_aidr(void);
extern void check_offset(void);
extern void test_s1_translation(void);
extern void test_s2_translation(void);
extern void test_s1s2_translation(void);

#endif /* __TESTCASE_H__ */
