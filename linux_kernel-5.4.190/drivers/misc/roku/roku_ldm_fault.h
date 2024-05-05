#ifndef roku_ldm_fault_3efeabfa4f11c292
#define roku_ldm_fault_3efeabfa4f11c292

#define MAX_FAULTS_AT_ONCE 16
#define FAULT_DESC_LEN 7

struct fault_reason {
    uint8_t size;
    struct {
        char desc[FAULT_DESC_LEN];
        uint8_t dev_id;
        uint32_t ch_mask;
    } records[MAX_FAULTS_AT_ONCE];
};


#endif // roku_ldm_fault_3efeabfa4f11c292
