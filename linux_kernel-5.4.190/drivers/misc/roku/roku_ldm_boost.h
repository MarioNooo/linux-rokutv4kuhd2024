#ifndef roku_boost_933af923cd4e39d6
#define roku_boost_933af923cd4e39d6
// contains function declarations used in RokuBoost.c
#ifdef VISUAL_STUDIO
#define roku_malloc(X) malloc(X)
#define roku_free(X)   free(X)
#define roku_printf(...) printf(__VA_ARGS__)
typedef int bool;
#define false 0
#define true  1
typedef unsigned long long uint64_t;
#else
#define roku_malloc(X) kmalloc(X, GFP_KERNEL)
#define roku_free(X)   kfree(X)
#define roku_printf(msg...)
#endif
//contains sytem, configuration and algo parameters

struct configregs {
    unsigned short vzones;
    unsigned short hzones;
    unsigned short zones;
    unsigned short GlobalDriveCurrent;
    uint64_t PSCurrentMax;
};

struct  algoregs {
    unsigned short MinDimmingKnee;
    unsigned short MaxBoostKnee;
    unsigned short DimmingSlopeGain;
    unsigned short BoostSlopeGain;
    unsigned long int PSCurrentLimit;
    unsigned short regMinCurrent;
    unsigned short BoostGain;
    unsigned char TempFilterDCEnable;
    unsigned short UpGain;
    unsigned short DownGain;
    unsigned char ApplyTfilter2PWM;
    unsigned char AdaptKneeSel;
    unsigned short MaxDimmingKnee;
    unsigned short MinBoostKnee;
    unsigned char KneeTFEnable;
    unsigned short KneeTFGain;
    unsigned char BoostSelect;
    unsigned char DimmingEnable;
    unsigned short PWM_Max;
    unsigned short PWM_Min;
    unsigned char PWM_MapEnable;
    unsigned short GlobalCurrent;
    unsigned short MaxCurrent;
    unsigned short FSGain;
    unsigned short FSThreshold;
    unsigned char LDReduceSel;
    unsigned short BoostMaxAZL;
    unsigned short BoostMinAZL;
    unsigned short DimmingMaxAZL;
    unsigned short DimmingMinAZL;
};

struct sysstateregs {
    unsigned char LDStrength;
    unsigned char sceneChange;
    unsigned short GlobalBacklight;
};

struct LDstats {
    unsigned short LDMax;
    unsigned short LDMin;
    unsigned short LDAZL;
};

struct KneeOut {
    unsigned short BoostKnee_low;
    unsigned short ReduceKnee_high;
    unsigned short BoostKnee_high;
    unsigned short ReduceKnee_low;
    unsigned long ReduceKneeLow_hist;
    unsigned long BoostKneeLow_hist;
    unsigned long BoostKneeHigh_hist;
    unsigned long ReduceKneeHigh_hist;
};

struct Knee_Last {
    unsigned long BoostKneelow_last;
    unsigned long ReduceKneehigh_last;
    unsigned long BoostKneehigh_last;
    unsigned long ReduceKneelow_last;
};

#define GlobaBacklight_Max 1023u
#define ParamReg_Max 1024u
bool RokuBoost_init(struct configregs* config);
void RokuBoost_exit(void);
void RokuBoost_set_algregs(struct algoregs* alg);
void RokuBoost_setMaxPower(uint64_t maxPower);
struct algoregs* RokuBoost_get_algregs(void);
void RokuBoost(unsigned short* LDVal_Input, unsigned short* PWM_GBL_Output, unsigned short *DriveCurrent_GBL_Output, struct sysstateregs* sysstate);
#endif