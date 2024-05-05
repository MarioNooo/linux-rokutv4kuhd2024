#ifdef VISUAL_STUDIO
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<inttypes.h>
#else
#include <linux/types.h>
#include <linux/slab.h>
#endif
#include"roku_ldm_boost.h"

#define UINT16_MAX 65535

// define the values for the algo, system and config parameters
static struct algoregs a1 = {
    .MinDimmingKnee = 00,  //16 bit value
    .MaxBoostKnee = 53000,  //16 bit value
    .DimmingSlopeGain = 1, //10bit value 
    .BoostSlopeGain = 1, //10bit value 
    .PSCurrentLimit = 922, //10bit value 
    .regMinCurrent = 8192,    //16bit value
    .BoostGain = 1024,                    //10bit value
    .TempFilterDCEnable = 0,
    .UpGain = 500,                    //10bit value
    .DownGain = 500,                //10bit value,
    .ApplyTfilter2PWM = 0,
    .AdaptKneeSel = 4,            //0 = registers, 1, shift knees by Min/Max, 2= use min, max, and shifted knees, 3 = use min, max, registers, 4 - modify knees based on AZL.
    .MaxDimmingKnee = 1000,
    .MinBoostKnee = 25000,
    .KneeTFEnable = 0,
    .KneeTFGain = 870,  //10bit value
    .BoostSelect = 1,
    .DimmingEnable = 1,
    .PWM_Max = 65535,
    .PWM_Min = 0,
    .PWM_MapEnable = 1,
    .GlobalCurrent = 42598,
    .MaxCurrent = 65535,
    .FSGain = 800,
    .FSThreshold = 63000,
    .LDReduceSel = 0,  //0 = spread over zones, 1  = white zones, 2 = non-reduced zones
    .BoostMaxAZL = 65535,
    .BoostMinAZL = 1000,
    .DimmingMinAZL = 0,
    .DimmingMaxAZL = 54000
};

static struct configregs c1 = {
    .vzones = 6,
    .hzones = 8,
    .zones = 48,
    .GlobalDriveCurrent = 42598, //16 bit value
    .PSCurrentMax = 2144692, 
};

static struct sysstateregs s1 = {
    .LDStrength = 3,
    .sceneChange = 0,
    .GlobalBacklight = GlobaBacklight_Max,
};



static uint64_t * PWM_GBL_lastframe_ = NULL;
static uint64_t* DriveCurrent_GBL_lastframe_ = NULL;
static unsigned short* PWM_GBL_ = NULL;
static unsigned short* DriveCurrent_Tfiltered_ = NULL;
static unsigned short* DriveCurrent_Reduced_ = NULL;

static unsigned char* zone_type_ = NULL;
static unsigned short* PWM_Mapped_ = NULL;
static unsigned short* DriveCurrent_GReduced_ = NULL;
static unsigned short* DriveCurrent_boosted_ = NULL;
static unsigned short* DriveCurrent_GBL_ = NULL;
static uint64_t* DriveCurrent_tmp_ = NULL;
static bool initialized_ = false;
static unsigned long  BoostKneelow_last = 0;
static unsigned long  ReduceKneelow_last = 0;
static unsigned long  BoostKneehigh_last = 0;
static unsigned long  ReduceKneehigh_last = 0;

static uint64_t* PWM_GBL_temp_ = NULL;
static uint64_t* DriveCurrent_GBL_temp_ = NULL;




//void AdaptKnees(unsigned short LDMax, unsigned short LDMin, unsigned short LDAZL, unsigned long ReduceKnee_last, unsigned long BoostKnee_last, unsigned short* BoostKnee_low, unsigned short* ReduceKnee_high, unsigned short* BoostKnee_high, unsigned short *ReduceKnee_low, unsigned long* ReduceKnee_hist, unsigned long* BoostKnee_hist);
void AdaptKnees(struct LDstats LD_stats, unsigned long ReduceKneelow_last, unsigned long ReduceKneehigh_last, unsigned long BoostKneelow_last, unsigned long BoostKneehigh_last, struct KneeOut *Knee_outputs);
void BoostReduceCount(unsigned short* LDValues, unsigned short* black_zones, unsigned short* white_zones, unsigned short ReduceKnee, unsigned short BoostKnee, uint64_t* FSSum, unsigned char* FullScreenFlag, unsigned char* zone_type);
void ReduceDriveCurrent(unsigned short* LDValues, unsigned short ReduceKnee_high, unsigned short ReduceKnee_low, unsigned short* DriveCurrentReduced);
void PowerCalc(unsigned short* LDValues, unsigned short* DriveCurrent, unsigned char * OverPower, uint64_t* overage, uint64_t* spare_power, uint64_t* power_used);
void GlobalDriveCurrentReduce(unsigned short* DriveCurrent, unsigned char reduction_type, unsigned char* zone_type, unsigned short white_zones, unsigned short black_zones, uint64_t overage, unsigned short* DriveCurrent_Reduced);
void ZoneCenter(unsigned char* zone_type, unsigned char centerValue, unsigned char* zone_type_out);
void LDValuestats(unsigned short* LDVal, struct LDstats *LDs_out);

void TemporalFilter(uint64_t* PWM_GBL_lastframe, uint64_t* DriveCurrent_GBL_lastframe, unsigned short* PWM_GBL, unsigned short* DriveCurrent_GBL, uint64_t* DriveCurrent_Tfiltered, uint64_t* PWM_Tfiltered);
void PWM_Mapping(unsigned short* LEDValue_in, unsigned short ReduceKnee, unsigned short BoostKnee, unsigned short* PWM_out);
void BoostDriveCurrent2(unsigned char* zone_type, unsigned short white_zones, unsigned short BoostKnee_low, unsigned short BoostKnee_high, unsigned short* PWM_in, unsigned short* DriveCurrent_in, uint64_t spare_power, uint64_t FSSum, unsigned char FullScreenFlag, unsigned short* DriveCurrent_boosted);


bool RokuBoost_init(struct configregs * config)
{   
    if (config != NULL) {
        memcpy((void*)&c1, (void*)config, (unsigned int)sizeof(struct configregs));

        // clean up previous init
        RokuBoost_exit();

        c1.zones = c1.hzones * c1.vzones;
        unsigned int uchar_size = c1.zones * sizeof(unsigned char);
        unsigned int ushort_size = c1.zones * sizeof(unsigned short);

        zone_type_ = (unsigned char*)roku_malloc(uchar_size);
        if (zone_type_ == NULL) {
            roku_printf("zone_type_ memory allocation failed");
            return false;
        } else {
            memset((void*)zone_type_, 0, uchar_size);
        }


        PWM_GBL_lastframe_ = (uint64_t*)roku_malloc(c1.zones * sizeof(uint64_t));
        if (PWM_GBL_lastframe_ == NULL) {
            roku_printf("PWM_GBL_lastframe_ memory allocation failed");
            return false;
        } else {
            memset((void*)PWM_GBL_lastframe_, 0, c1.zones * sizeof(uint64_t));
        }

        DriveCurrent_GBL_lastframe_ = (uint64_t*)roku_malloc(c1.zones * sizeof(uint64_t));
        if (DriveCurrent_GBL_lastframe_ == NULL) {
            roku_printf("DriveCurrent_GBL_lastframe_ memory allocation failed");
            return false;
        } else {
            memset((void*)DriveCurrent_GBL_lastframe_, 0, c1.zones * sizeof(uint64_t));
        }

        PWM_GBL_ = (unsigned short*)roku_malloc(ushort_size);
        if (PWM_GBL_ == NULL) {
            roku_printf("PWM_GBL_ memory allocation failed");
            return false;
        } else {
            memset((void*)PWM_GBL_, 0, ushort_size);
        }

        DriveCurrent_Tfiltered_ = (unsigned short*)roku_malloc(ushort_size);
        if (DriveCurrent_Tfiltered_ == NULL) {
            roku_printf("DriveCurrent_Tfiltered_ memory allocation failed");
            return false;
        } else {
            memset((void*)DriveCurrent_Tfiltered_, 0, ushort_size);
        }

        DriveCurrent_Reduced_ = (unsigned short*)roku_malloc(ushort_size);
        if (DriveCurrent_Reduced_ == NULL) {
            roku_printf("DriveCurrent_Reduced_ memory allocation failed");
            return false;
        } else {
            memset((void*)DriveCurrent_Reduced_, 0, ushort_size);
        }

        PWM_Mapped_ = (unsigned short*)roku_malloc(ushort_size);
        if (PWM_Mapped_ == NULL) {
            roku_printf("PWM_Mapped_ memory allocation failed");
            return false;
        }
        else {
            memset((void*)PWM_Mapped_, 0, ushort_size);
        }

        DriveCurrent_GReduced_ = (unsigned short*)roku_malloc(ushort_size);
        if (DriveCurrent_GReduced_ == NULL) {
            roku_printf("DriveCurrent_GReduced_ memory allocation failed");
            return false;
        } else {
            memset((void*)DriveCurrent_GReduced_, 0, ushort_size);
        }

        DriveCurrent_boosted_ = (unsigned short*)roku_malloc(ushort_size);
        if (DriveCurrent_boosted_ == NULL) {
            roku_printf("DriveCurrent_boosted_ memory allocation failed");
            return false;
        } else {
            memset((void*)DriveCurrent_boosted_, 0, ushort_size);
        }

        DriveCurrent_GBL_ = (unsigned short*)roku_malloc(ushort_size);
        if (DriveCurrent_GBL_ == NULL) {
            roku_printf("DriveCurrent_GBL_ memory allocation failed");
            return false;
        } else {
            memset((void*)DriveCurrent_GBL_, 0, ushort_size);
        }

        DriveCurrent_tmp_ = (uint64_t*)roku_malloc(c1.zones * sizeof(uint64_t));
        if (DriveCurrent_tmp_ == NULL) {
            roku_printf("DriveCurrent_tmp_ memory allocation failed");
            return false;
        }
        else {
            memset((void*)DriveCurrent_tmp_, 0, c1.zones * sizeof(uint64_t));
        }

        DriveCurrent_GBL_temp_ = (uint64_t*)roku_malloc(c1.zones * sizeof(uint64_t));
        if (DriveCurrent_GBL_temp_ == NULL) {
            roku_printf("DriveCurrent_GBL_tmp_ memory allocation failed");
            return false;
        }
        else {
            memset((void*)DriveCurrent_GBL_temp_, 0, c1.zones * sizeof(uint64_t));
        }

        PWM_GBL_temp_ = (uint64_t*)roku_malloc(c1.zones * sizeof(uint64_t));
        if (PWM_GBL_temp_ == NULL) {
            roku_printf("PWM_GBL_temp_ memory allocation failed");
            return false;
        }
        else {
            memset((void*)PWM_GBL_temp_, 0, c1.zones * sizeof(uint64_t));
        }

        initialized_ = true;
        return true;
    }
    return false;
}

void RokuBoost_exit(void)
{
    if (PWM_GBL_lastframe_) {
        roku_free(PWM_GBL_lastframe_);
        PWM_GBL_lastframe_ = NULL;
    }
    if (DriveCurrent_GBL_lastframe_) {
        roku_free(DriveCurrent_GBL_lastframe_);
        DriveCurrent_GBL_lastframe_ = NULL;
    }
    if (PWM_GBL_) {
        roku_free(PWM_GBL_);
        PWM_GBL_ = NULL;
    }
    if (DriveCurrent_Tfiltered_) {
        roku_free(DriveCurrent_Tfiltered_);
        DriveCurrent_Tfiltered_ = NULL;
    }
    if (DriveCurrent_Reduced_) {
        roku_free(DriveCurrent_Reduced_);
        DriveCurrent_Reduced_ = NULL;
    }
    if (zone_type_) {
        roku_free(zone_type_);
        zone_type_ = NULL;
    }

    if (PWM_Mapped_) {
        roku_free(PWM_Mapped_);
        PWM_Mapped_ = NULL;
    }

    if (DriveCurrent_GReduced_) {
        roku_free(DriveCurrent_GReduced_);
        DriveCurrent_GReduced_ = NULL;
    }

    if (DriveCurrent_boosted_) {
        roku_free(DriveCurrent_boosted_);
        DriveCurrent_boosted_ = NULL;
    }

    if (DriveCurrent_GBL_) {
        roku_free(DriveCurrent_GBL_);
        DriveCurrent_GBL_ = NULL;
    }

    if (DriveCurrent_GBL_temp_) {
        roku_free(DriveCurrent_GBL_temp_);
        DriveCurrent_GBL_temp_ = NULL;
    }

    if (PWM_GBL_temp_) {
        roku_free(PWM_GBL_temp_);
        PWM_GBL_temp_ = NULL;
    }

    if (DriveCurrent_tmp_) {
        roku_free(DriveCurrent_tmp_);
        DriveCurrent_tmp_ = NULL;
    }

    initialized_ = false;
}

struct algoregs* RokuBoost_get_algregs(void) {
    return &a1;
}

void RokuBoost_set_algregs(struct algoregs* alg)
{
    if (alg != NULL) {
        memcpy((void*)&a1, (void*)alg, (unsigned int)sizeof(struct algoregs));
    }
    // take care of out of bounds values
    a1.PSCurrentLimit = (a1.PSCurrentLimit > ParamReg_Max) ? ParamReg_Max : a1.PSCurrentLimit;
    a1.UpGain = (a1.UpGain > ParamReg_Max) ? ParamReg_Max : a1.UpGain;
    a1.DownGain = (a1.DownGain > ParamReg_Max) ? ParamReg_Max : a1.DownGain;
    a1.FSGain = (a1.FSGain > ParamReg_Max) ? ParamReg_Max : a1.FSGain;
    a1.BoostGain = (a1.BoostGain > ParamReg_Max) ? ParamReg_Max : a1.BoostGain;
    a1.BoostSlopeGain = (a1.BoostSlopeGain > ParamReg_Max) ? ParamReg_Max : a1.BoostSlopeGain;
    a1.KneeTFGain = (a1.KneeTFGain > ParamReg_Max) ? ParamReg_Max : a1.KneeTFGain;
    a1.DimmingSlopeGain = (a1.DimmingSlopeGain > ParamReg_Max) ? ParamReg_Max : a1.DimmingSlopeGain;
    a1.MaxCurrent = (a1.GlobalCurrent >= a1.MaxCurrent) ? a1.GlobalCurrent + 1 : a1.MaxCurrent;
    a1.regMinCurrent = (a1.GlobalCurrent <= a1.regMinCurrent) ? a1.GlobalCurrent - 1 : a1.regMinCurrent;
}


void RokuBoost(unsigned short* LDVal_Input, unsigned short* PWM_GBL_Output, unsigned short* DriveCurrent_GBL_Output, struct sysstateregs* sysstate) {

    int i, j,k;
    unsigned char reduction_type;
    unsigned short black_zones;
    unsigned short white_zones;
    unsigned char OverPower, OverPower1, FullScreenFlag;
    uint64_t PWM_temp, DC_temp, FSSum;
    uint64_t Power_used, Power_used1;
    uint64_t Overage, Overage1;
    uint64_t Spare_Power, Spare_Power1;

    struct LDstats LD_stats;
    struct KneeOut Knee_outputs;
   


    if (!initialized_) return;

    if (sysstate != NULL) {
        memcpy((void*)&s1, (void*)sysstate, (unsigned int)sizeof(struct sysstateregs));
    }

    //algorithm starts here 

    //get statistics about LD Value input to guide performance
    LDValuestats(LDVal_Input, &LD_stats);
    roku_printf("LDMax = %i, LDMin = %i, LDAZL = %i \n", LD_stats.LDMax, LD_stats.LDMin, LD_stats.LDAZL);



    //modify boost and reduce knees based on LD Value statistics
//    AdaptKnees(LDMax, LDMin, LDAZL, ReduceKnee_last, BoostKnee_last, &BoostKnee_low, &ReduceKnee_high, &BoostKnee_high, &ReduceKnee_low, &ReduceKnee_hist, &BoostKnee_hist);
    AdaptKnees(LD_stats, ReduceKneelow_last, ReduceKneehigh_last, BoostKneelow_last, BoostKneehigh_last, &Knee_outputs);
    roku_printf("ReduceKnee_low = %i, ReduceKnee_high = %i,BoostKnee_low = %i, BoostKnee_high = %i, ReduceKneelow_hist =%i, ReduceKneehigh_hist = %i BoostKneelow_hist = %i, BoostKneehigh_hist = %i \n", Knee_outputs.ReduceKnee_low, Knee_outputs.ReduceKnee_high, Knee_outputs.BoostKnee_low, Knee_outputs.BoostKnee_high, Knee_outputs.ReduceKneeLow_hist, Knee_outputs.ReduceKneeHigh_hist, Knee_outputs.BoostKneeLow_hist, Knee_outputs.BoostKneeHigh_hist);
 
    //assign nominal current and reduced current for all zones
    ReduceDriveCurrent(LDVal_Input, Knee_outputs.ReduceKnee_high, Knee_outputs.ReduceKnee_low, DriveCurrent_Reduced_);
#ifdef VISUAL_STUDIO
    for (j = 0; j < c1.zones; j++) {
        roku_printf("PWM = %i, DC = %i \n", LDVal_Input[j], DriveCurrent_Reduced_[j]);
    }
   
#endif   
    //determine amount of zones to be darkened, or boosted, and assign a tag to the zone
    //1 = reduced, 0 = no change, 2 = boosted
    BoostReduceCount(LDVal_Input, &black_zones, &white_zones, Knee_outputs.ReduceKnee_high, Knee_outputs.BoostKnee_low, &FSSum, &FullScreenFlag, zone_type_);
#ifdef VISUAL_STUDIO
    for (j = 0; j < c1.vzones; j++) {
        for (i = 0; i < c1.hzones; i++) {
            k = j * c1.hzones + i;
            roku_printf("%i, ", zone_type_[k]);
        }
        roku_printf("\n");
    }
#endif
    
    roku_printf("black zones = %i, white zones = %i, FSSum = %"PRIu64", fullScreenFlag = % i \n", black_zones, white_zones, FSSum,FullScreenFlag);

    // remap the PWM values to PWM_Max and PWM_Min based on the Knee values
    PWM_Mapping(LDVal_Input, Knee_outputs.ReduceKnee_high, Knee_outputs.BoostKnee_low, PWM_Mapped_);
#ifdef VISUAL_STUDIO
    /*    for (j = 0; j < c1.zones; j++) {
            roku_printf("LDValue_Input = %i, PWM_Mapped_ = %i \n", LDVal_Input[j], PWM_Mapped_[j]);
        }
    */
#endif

    //calculate the power consumed before boosting to determine how much boosting can be performed.
    PowerCalc(LDVal_Input, DriveCurrent_Reduced_, &OverPower, &Overage, &Spare_Power, &Power_used);
    roku_printf("OverPower y/n = %i, Overage = %"PRIu64", spare_power = % "PRIu64", power_used = %" PRIu64" \n", OverPower, Overage, Spare_Power, Power_used);


    if (OverPower == 1) {
        //No boosting possible - we are over budget and need to reduce the drive current
        reduction_type = 0;
        GlobalDriveCurrentReduce(DriveCurrent_Reduced_, reduction_type, zone_type_, white_zones, black_zones, Overage, DriveCurrent_GReduced_);

#ifdef VISUAL_STUDIO
/*        for (i = 0; i < c1.zones; i++) {
            roku_printf("DriveCurrentReduced = %i, zone_type = %i, DriveCurrent_Greduced = %i \n", DriveCurrent_Reduced_[i], zone_type_[i], DriveCurrent_GReduced_[i]);
        }
        */
#endif
    } else {
        // Perform boosting since there is spare power
        // only perform boosting if there are some zones that can be boosted.
        if (white_zones == 0) {
 //           roku_printf("No white zones\n");
            for (j = 0; j < c1.zones; j++) {
                DriveCurrent_boosted_[j] = DriveCurrent_Reduced_[j];
            }
        }
        else {
            BoostDriveCurrent2(zone_type_, white_zones, Knee_outputs.BoostKnee_low, Knee_outputs.BoostKnee_high, LDVal_Input, DriveCurrent_Reduced_, Spare_Power, FSSum, FullScreenFlag,DriveCurrent_boosted_);
        }
#ifdef VISUAL_STUDIO
        for (i = 0; i < c1.zones; i++) {
 //           roku_printf("zone_type = %i, LDVal = %i, DriveCurrent_in = %i, DriveCurrent_boosted = %i \n", zone_type_[i], LDVal_Input[i], DriveCurrent_Reduced_[i], DriveCurrent_boosted_[i]);
        }
#endif
        
        // check if we exceed power budget after boosting
        PowerCalc(PWM_Mapped_, DriveCurrent_boosted_, &OverPower1, &Overage1, &Spare_Power1, &Power_used1);
        roku_printf("OverPower1 y/n = %i, Overage1 = %"PRIu64", spare_power1 = %"PRIu64", power_used1 = %"PRIu64" \n", OverPower1, Overage1, Spare_Power1, Power_used1);
        // reduce drive current if we are over power budget
        if (OverPower1 == 1) {
            reduction_type = a1.LDReduceSel;
            GlobalDriveCurrentReduce(DriveCurrent_boosted_, reduction_type, zone_type_, white_zones, black_zones, Overage1, DriveCurrent_GReduced_);
/*            for (i = 0; i < c1.zones; i++) {
                roku_printf("DriveCurrentReduced = %i, zone_type = %i, DriveCurrent_Greduced = %i \n", DriveCurrentReduced[i], zone_type[i], DriveCurrent_GReduced[i]);
            }
            */
        }
        else {
            for (i = 0; i < c1.zones; i++) {
                DriveCurrent_GReduced_[i] = DriveCurrent_boosted_[i];
            }

        }
    }
 
    //Reduce fast/strong changes to PWM and drive current due to fast changes of the LDValues.
    TemporalFilter(PWM_GBL_lastframe_, DriveCurrent_GBL_lastframe_, PWM_Mapped_, DriveCurrent_GReduced_, DriveCurrent_GBL_temp_, PWM_GBL_temp_);

    

#ifdef VISUAL_STUDIO
    for (i = 0; i < c1.zones; i++) {
        //roku_printf(" DriveCurrentLastFrame = %i, DriveCurrentin = %i, DriveCurrent_out = %i, PWMlastframe = %i, PWMin = %i, PWM_out = %i \n",  DriveCurrent_GBL_lastframe_[i], DriveCurrent_GReduced_[i], DriveCurrent_GBL_temp_[i], PWM_GBL_lastframe_[i], PWM_Mapped_[i], PWM_GBL_temp_[i]);

        }
#endif


    //conver the higher precision values required by IIR filter to unsigned shorts with the use of rounding
    //assign the hist value to last to be used by IIR filters in TemporalFilter and AdaptKnees blocks.
    for (i = 0; i < c1.zones; i++) {
        PWM_temp = PWM_GBL_temp_[i] + 512;
        PWM_temp = PWM_temp >>10;
        PWM_temp = (PWM_temp > 65535) ? 65535 : PWM_temp;
        PWM_GBL_Output[i] = (unsigned short)PWM_temp;
        DC_temp = DriveCurrent_GBL_temp_[i] + 512;
        DC_temp = DC_temp >>10;
        DC_temp = (DC_temp > 65535) ? 65535 : DC_temp;
        DriveCurrent_GBL_Output[i] = (unsigned short)DC_temp;
        PWM_GBL_lastframe_[i] = PWM_GBL_temp_[i];
        DriveCurrent_GBL_lastframe_[i] = DriveCurrent_GBL_temp_[i];
    }
    ReduceKneelow_last = Knee_outputs.ReduceKneeLow_hist;
    BoostKneelow_last = Knee_outputs.BoostKneeLow_hist;
    ReduceKneehigh_last = Knee_outputs.ReduceKneeHigh_hist;
    BoostKneehigh_last = Knee_outputs.BoostKneeHigh_hist;


#ifdef VISUAL_STUDIO
    for (i = 0; i < c1.zones; i++) {
        //roku_printf("PWM_out = %i, DriveCurrent_out = %i \n", PWM_GBL_Output[i], DriveCurrent_GBL_Output[i]);

    }
#endif

}




// This block counts how many blocks will be dimmed, how many will be boosted and tags each block accordingly
// dimming block = 1, boosting block = 2, no change block = 0

void BoostReduceCount(unsigned short* LDValues, unsigned short* black_zones, unsigned short* white_zones, unsigned short ReduceKnee, unsigned short BoostKnee, uint64_t* FSSum, unsigned char* FullScreenFlag, unsigned char* zone_type)
{
    unsigned short bz = 0;
    unsigned short wz = 0;
    unsigned char FSF = 0;
    uint64_t FSS = 0;
    unsigned short FSCount = 0;
    int i;
//    roku_printf("Boost Knee = %i, Reduce_Knee = %i \n", BoostKnee, ReduceKnee);

    for (i = 0; i < c1.zones; i++) {
        // zone is "black"
        if ((LDValues[i] < ReduceKnee) && (a1.DimmingEnable == 1)){
        bz = bz + 1;
        zone_type[i] = 1;
        }  else if ((LDValues[i] > BoostKnee) && (a1.BoostSelect >= 1)) {
            //zone is "white"
            wz = wz + 1;
            zone_type[i] = 2;
            } else {
                // zone is no change
                zone_type[i] = 0;
                } 
        if (LDValues[i] >= a1.FSThreshold) {
            FSCount = FSCount + 1;
        }
        FSS = FSS + LDValues[i];
#ifdef VISUAL_STUDIO
//        roku_printf("LDValue = %d, black = %d, white = %d, zone_type = %d \n", LDValues[i], bz, wz, zone_type[i]);
#endif
    }
    if (FSCount == c1.zones) {
        FSF = 1;
    }
    *FullScreenFlag = FSF;
    *black_zones = bz;
    *white_zones = wz;
    *FSSum = FSS;
}

// This block will reduce the drive current linearly from the global current value to the min current value for zones whose PWM is below a threshold. 
// if the PWM is above the threshold then the global current is applied to the drive current for the zone.
void ReduceDriveCurrent(unsigned short* LDValues, unsigned short ReduceKnee_high, unsigned short ReduceKnee_low, unsigned short* DriveCurrentReduced)
{
    int i;
    unsigned short MinCurrent, RKH;
    unsigned short RKL = ReduceKnee_low;
    uint64_t gain;
    uint64_t DCR;

    //fix any overflow/underflow/not allowed situations
    MinCurrent = ((a1.GlobalCurrent == 0) || (a1.regMinCurrent <= a1.GlobalCurrent)) ? a1.regMinCurrent : a1.GlobalCurrent - 1;
    RKH = (ReduceKnee_high == 0) ? 1 : ReduceKnee_high;
    gain = (a1.GlobalCurrent < MinCurrent) ? 0: (uint64_t)(a1.GlobalCurrent - MinCurrent) * ParamReg_Max / (RKH - RKL) * a1.DimmingSlopeGain;

    roku_printf("RKH = %i, gain = %i \n", RKH, a1.GlobalCurrent);



    //perform the current reduction
    if (a1.DimmingEnable == 1) {
        for (i = 0; i < c1.zones; i++) {
            //when LD is on
#ifdef VISUAL_STUDIO        
    //    roku_printf("LDValue = %i, gain = %"PRIu64", DimmingEnable = %i \n", LDValues[i], gain, a1.DimmingEnable);
#endif        
            if (LDValues[i] < RKH) {

                DCR = (a1.GlobalCurrent < gain* (RKH - LDValues[i]) / ParamReg_Max) ? a1.regMinCurrent : a1.GlobalCurrent - gain * (RKH - LDValues[i]) / ParamReg_Max;
                DCR = (DCR < a1.regMinCurrent) ? a1.regMinCurrent : DCR;

                DriveCurrentReduced[i] = (unsigned short)DCR;
            }
            else {
                DriveCurrentReduced[i] = a1.GlobalCurrent;
            }
        }
    }
    else {
        for (i = 0; i < c1.zones; i++) {
            //when LD is offn
            DriveCurrentReduced[i] = a1.GlobalCurrent;
        }
    }
#ifdef VISUAL_STUDIO
    for (i = 0; i < c1.zones; i++) {
        roku_printf("LDValue = %i, gain = %"PRIu64", DriveCurrent_reduced = %i \n", LDValues[i], gain, DriveCurrentReduced[i]);
    }
#endif
}

//this block performs the power calculation based on the Local Dimming and Drive Current values. It will report if we have exceeded the power budget (PSCurrentMax*PowerLimit)/1024
//OverPower = 1, It will also report the over power value (power used - PSCurrentMax*limit/1024) andthe spare power (PSCurrentMax*limit)/1024 - power_used)
void PowerCalc(unsigned short* LDValues, unsigned short* DriveCurrent, unsigned char* OverPower, uint64_t* overage, uint64_t* spare_power, uint64_t* power_used)
{
    int i;
    unsigned char op;
    uint64_t over, sp, pwr;
    uint64_t pu = 0;
    uint64_t pl1,pl;

 
    //define used power maximum for Calculation of power limit
    pl1 = c1.PSCurrentMax * a1.PSCurrentLimit;
    pl1 = pl1 >> 10;
    pl = pl1;
    //calculate power used
    for (i = 0; i < c1.zones; i++) {
        pwr = (unsigned int)LDValues[i] * DriveCurrent[i];
        pwr = pwr / UINT16_MAX;
        pu = pwr + pu;
#ifdef VISUAL_STUDIO
//        roku_printf("Ldv = %lu, DriveCurrent = %lu, zonepower = %"PRIu64" \n", LDValues[i], DriveCurrent[i],  pwr);
#endif

    }
    //calculate overpower y/n, overage and spare power values
    if (pu > (pl)) {
        op = 1;
        over = pu - (pl);
        sp = 0;
    } else {
        op = 0;
        over = 0;
        sp = (pl) - pu;
    }
    roku_printf("Power used = %"PRIu64", PSCurrentMax = %"PRIu64" pl1 = %"PRIu64", power_limit = %"PRIu64", overpower y/n = %d, overage = %"PRIu64", spare_power = %"PRIu64" \n", pu, c1.PSCurrentMax, pl1, pl, (int)op, over, sp);
    *OverPower = op;
    *overage = over;
    *spare_power = sp;
    *power_used = pu;
}

// This block reduces the drive current for every zone if we have exceeded the power budget
void GlobalDriveCurrentReduce(unsigned short* DriveCurrent,  unsigned char reduction_type, unsigned char* zone_type, unsigned short white_zones, unsigned short black_zones, uint64_t overage, unsigned short* DriveCurrent_Reduced)
{
    int i;
    uint64_t OPZ;
    unsigned short zmb = c1.zones < black_zones ? 0 : c1.zones - black_zones;
    //take care of over/under situations
    if (reduction_type == 0) {
        //reduce all zones
        OPZ = (overage < c1.zones) ? 1 : overage / c1.zones;
    }else if (reduction_type == 1) {
        //reduce only boosted zones
        OPZ = (overage < white_zones) ? 1 : (white_zones == 0) ? overage : overage / white_zones;
    } else if (reduction_type == 2) {   
        // calculate reduction if reducing non-dimmed zones
        OPZ = (overage < zmb) ? 1 : (zmb == 0) ? overage : overage / (c1.zones - black_zones);
    } else {
        OPZ = 1;
    }

    // perform Current reduction
    if (reduction_type == 0) {
        //reduce all zones
        for (i = 0; i < c1.zones; i++) {
            DriveCurrent_Reduced[i] = DriveCurrent[i] - (unsigned short)OPZ;
        }
    } else if (reduction_type == 1) {
        //reduce only boosted zones
        for (i = 0; i < c1.zones; i++) {
            if (zone_type[i] >= 2) {
                DriveCurrent_Reduced[i] = DriveCurrent[i] - (unsigned short)OPZ;
            }
            else {
                DriveCurrent_Reduced[i] = DriveCurrent[i];
            }
        }
    } else {
        //reduce only non-reduced zones
        for (i = 0; i < c1.zones; i++) {
            if ((zone_type[i] >= 2) || (zone_type[i] == 0)) {
                DriveCurrent_Reduced[i] = DriveCurrent[i] - (unsigned short)OPZ;
            }
        }
    }
#ifdef VISUAL_STUDIO
    for (i = 0; i < c1.zones; i++) {
//		roku_printf("DriveCurrent = %i, OPZ = %"PRIu65",  DriveCurrentReduced = %lu \n", DriveCurrent[i], OPZ, DriveCurrent_Reduced[i]);
    }
#endif

}
    


// find Min, Max, sum and average values of LD Value inputs.
void LDValuestats(unsigned short* LDVal, struct LDstats *LDs_out)
{
    int i;
    unsigned short min = UINT16_MAX;
    unsigned short max = 0;
    unsigned short azl = 0;
    unsigned int sum = 0;
    for (i = 0; i < c1.zones; i++) {
        max = (LDVal[i] > max) ? LDVal[i] : max;
        min = (min > LDVal[i]) ? LDVal[i] : min;
        sum += LDVal[i];
    }
    azl = sum / c1.zones;
//    roku_printf("Max = %li, Min = %li, AZL = %li, Sum = %li \n", max, min, azl, sum);
    LDs_out->LDMax = max;
    LDs_out->LDMin = min;
    LDs_out->LDAZL = azl;
}

//apply IIR filter to PWM and Drive Current values.
void TemporalFilter(uint64_t* PWM_GBL_lastframe, uint64_t* DriveCurrent_GBL_lastframe, unsigned short* PWM_GBL, unsigned short* DriveCurrent_GBL, uint64_t* DriveCurrent_Tfiltered, uint64_t* PWM_Tfiltered)
{
    int i;
    unsigned short a, b, gainDC,  gainPWM;
    uint64_t PWM_temp1, PWM_temp2;

    if (a1.TempFilterDCEnable == 1) {
        for (i = 0; i < c1.zones; i++) {
            a = DriveCurrent_GBL[i];
            b = (unsigned short)(DriveCurrent_GBL_lastframe[i] >> 10);

            gainDC = (s1.sceneChange == 1) ? ParamReg_Max : (b <= a) ? a1.UpGain : a1.DownGain;

            //apply temporal filtering
            DriveCurrent_Tfiltered[i] = ((uint64_t)DriveCurrent_GBL[i] - (DriveCurrent_GBL_lastframe[i] >> 10)) * gainDC + DriveCurrent_GBL_lastframe[i];
#ifdef VISUAL_STUDIO
            //   roku_printf("DriveCurrent_last = %"PRIu64",   DriveCurrent = % lu, gain = % lu, DriveCurrent_Tfiltered = %"PRIu64" \n", DriveCurrent_GBL_lastframe[i], DriveCurrent_GBL[i], gainDC, DriveCurrent_Tfiltered[i]);
#endif
        }
    } else {
        for (i = 0; i < c1.zones; i++) {
            DriveCurrent_Tfiltered[i] = (uint64_t)DriveCurrent_GBL[i] * ParamReg_Max;
        }
    }

   if (a1.ApplyTfilter2PWM == 1) {
       for (i = 0; i < c1.zones; i++) {
           gainPWM = (s1.sceneChange == 1) ? 1024 : ((PWM_GBL_lastframe[i] >> 10) <= PWM_GBL[i]) ? a1.UpGain : a1.DownGain;

           PWM_temp1 = ((uint64_t)PWM_GBL[i] - (PWM_GBL_lastframe[i] >> 10));
           PWM_temp2 = PWM_temp1 * gainPWM;
           PWM_Tfiltered[i] = ((uint64_t)PWM_GBL[i] - (PWM_GBL_lastframe[i] >> 10)) * gainPWM + PWM_GBL_lastframe[i];
#ifdef VISUAL_STUDIO
   roku_printf("PWM_last = %"PRIu64",   CurrentPWM = % lu, gain = % lu, PWM_Tfiltered = %"PRIu64" \n", PWM_GBL_lastframe[i], PWM_GBL[i], gainPWM, PWM_Tfiltered[i]);
#endif
       }
   } else {
       for (i = 0; i < c1.zones; i++) {
           PWM_Tfiltered[i] = PWM_GBL[i] * ParamReg_Max;
       }
   }

}

//redefines the reduce and boost Knees based on LDValue statistics
//AdaptPWM = 0 -> RK low = 0, RK_high = register, BK_low = register, BK_high = 65535
//AdaptPWM = 1 -> RK low = LDMin, RK_high = register, BK_low = register, BK_high = LDMax
//AdaptPWM = 2 -> RK low = LDMin, RK_high = register, BK_low = register, BK_high = LDMax
void AdaptKnees(struct LDstats In,  unsigned long ReduceKneelow_last, unsigned long ReduceKneehigh_last, unsigned long BoostKneelow_last, unsigned long BoostKneehigh_last, struct KneeOut *Out)
{
    unsigned short LDMn = In.LDMin;
    unsigned short LDMx = In.LDMax;
    unsigned short LDAZL = In.LDAZL;
 //   unsigned long BKLast = BoostKnee_last;
 //   unsigned long RKLast = ReduceKnee_last;
    unsigned long BKLF, RKLF, BKHF, RKHF;
    unsigned long BKL_hist, RKL_hist,BKH_hist, RKH_hist;
    unsigned short BK, RK, RKL, BKH, RKH, BKL;
    unsigned short RAZLRange, BAZLRange;
    const unsigned int roundpt = 512u;
    unsigned short BKnew, RKnew;
    unsigned short RKNum, BKNum,BAZLNum,RAZLNum;
    uint64_t tmp1,tmp2;
    tmp1 = 0;
    tmp2 = 0;

    //protection for *NUM - being negative.
    RKNum = a1.MaxDimmingKnee - a1.MinDimmingKnee;
    BKNum = a1.MaxBoostKnee - a1.MinBoostKnee;
    BAZLNum = LDAZL - a1.BoostMinAZL;
    RAZLNum = LDAZL - a1.DimmingMinAZL;
    RAZLRange = (a1.DimmingMaxAZL <= a1.DimmingMinAZL) ? 1 : (a1.DimmingMaxAZL - a1.DimmingMinAZL);
    BAZLRange = (a1.BoostMaxAZL <= a1.BoostMinAZL) ? 1 : (a1.BoostMaxAZL - a1.BoostMinAZL);

    if (LDAZL > a1.BoostMaxAZL) {
        BKnew = a1.MaxBoostKnee;
    } else if (LDAZL < a1.BoostMinAZL) {
        BKnew = a1.MinBoostKnee;
    } else {
        tmp1 = (uint64_t)(BKNum) * BAZLNum / BAZLRange;
        BKnew = (unsigned short)(tmp1) + a1.MinBoostKnee;
    }

    if (LDAZL > a1.DimmingMaxAZL) {
        RKnew = a1.MaxDimmingKnee;
    }
    else if (LDAZL < a1.DimmingMinAZL) {
        RKnew = a1.MinDimmingKnee;
    } else {
        tmp2 = (uint64_t)(RKNum) * RAZLNum / RAZLRange;
        RKnew = (unsigned short)(tmp2) + a1.MinDimmingKnee;
    }

    roku_printf("MaxBoostKnee = %i, MinBoostKnee = %i, BoostMaxAZL = %i, BoostMinAZL = %i, LDAZL = %i, BKNum, =%i, BAZLNum = %i, tmp1 = %"PRIu64", BKnew = %i \n", a1.MaxBoostKnee, a1.MinBoostKnee, a1.BoostMaxAZL, a1.BoostMinAZL, LDAZL, BKNum, BAZLNum, tmp1, BKnew);
    roku_printf("MinDimmingKnee = %i, MaxDimmingKnee = %i, DimmingMaxAZL = %i, DimmingMinAZL = %i, LDAZL = %i, RKNum = %i, RAZLNum = %i, tmp2 = %"PRIu64", RKnew = %i \n", a1.MinDimmingKnee, a1.MaxDimmingKnee, a1.DimmingMaxAZL, a1.DimmingMinAZL, LDAZL, RKNum, RAZLNum, tmp2, RKnew);

    // create ReduceKnee and BoostKnee values based on registers and LDVal statistics
    if (a1.AdaptKneeSel >= 1) {
        RK = ((a1.MinDimmingKnee + LDMn) > a1.MaxDimmingKnee) ? a1.MaxDimmingKnee : a1.MinDimmingKnee + LDMn;
        BK = ((a1.MaxBoostKnee - (UINT16_MAX - LDMx)) < a1.MinBoostKnee) ? a1.MinBoostKnee : a1.MaxBoostKnee - (UINT16_MAX - LDMx);
    } else {
        RK = a1.MinDimmingKnee;
        BK = a1.MaxBoostKnee;
    }

    if (a1.AdaptKneeSel < 1) {
        RKL = 0;
        BKH = UINT16_MAX;
        RKH = (unsigned short)RK;
        BKL = (unsigned short)BK;
    } else if (a1.AdaptKneeSel == 1) {
        RKL = (unsigned short)RK - a1.MinDimmingKnee;
        BKH = UINT16_MAX - (a1.MaxBoostKnee - (unsigned short)BK);
        RKH = (unsigned short)RK;
        BKL = (unsigned short)BK;
    } else if (a1.AdaptKneeSel == 2) {
        RKL = (unsigned short)RK - a1.MinDimmingKnee;
        BKH = UINT16_MAX - (a1.MaxBoostKnee - (unsigned short)BK);
        RKH = a1.MinDimmingKnee;
        BKL = a1.MaxBoostKnee;
    } else {
        RKL = (LDMn < RKnew) ? LDMn : 0;
        //   BKH = ((LDMax + BKnew) > UINT16_MAX) ? UINT16_MAX : LDMax + BKnew;
        BKH = (LDMx > BKnew) ? LDMx : (LDMx + BKnew) > UINT16_MAX ? UINT16_MAX : LDMx + BKnew;
        RKH = RKnew;
        BKL = BKnew;
    }
    //   roku_printf("RKL = %i, RKH = %i, BKL = %i, BKH = %i \n",  RKL, RKH, BKL, BKH);

//    roku_printf("LDMax = %i, a1.MaxBoostKnee = %i, BoostKnee = %i, LDMin = %i, a1.MinDimmingKnee = %i, ReduceKnee = %i \n", LDMax, a1.MaxBoostKnee, BK, LDMin, a1.MinDimmingKnee, RK);
    //add temporal filtering of BoostKnee & ReduceKnee
 

    roku_printf(" Current Knees - ReduceKnee_low = %i, ReduceKnee_High = %i, BoostKnee_low = %i, BoostKnee_high = %i \n", RKL, RKH, BKL, BKH);
    roku_printf(" Last Frame Knees - ReduceKnee_low = %i, ReduceKnee_High = %i, BoostKnee_low = %i, BoostKnee_high = %i \n", ReduceKneelow_last, ReduceKneehigh_last, BoostKneelow_last, BoostKneehigh_last);

    if (a1.KneeTFEnable == 1) {
        BKL_hist = (BKL - (BoostKneelow_last >> 10)) * a1.KneeTFGain + BoostKneelow_last;
        RKL_hist = (RKL - (ReduceKneelow_last >> 10)) * a1.KneeTFGain + ReduceKneelow_last;
        BKH_hist = (BKH - (BoostKneehigh_last >> 10)) * a1.KneeTFGain + BoostKneehigh_last;
        RKH_hist = (RKH - (ReduceKneehigh_last >> 10)) * a1.KneeTFGain + ReduceKneehigh_last;
//        roku_printf("KneeTFGain = %i, BoostKnee_last = %i, BoostKnee = %i, ReduceKnee_last = %i, ReduceKnee = %i  BoostKnee_hist =%i, ReduceKnee_hist = %i \n", KneeTFGain, BoostKnee_last, BK, ReduceKnee_last, RK, BoostKnee_hist, ReduceKnee_hist );
        BKLF = BKL_hist + 512;
        RKLF = RKL_hist + 512;
        BKHF = BKH_hist + 512;
        RKHF = RKH_hist + 512;
//        roku_printf("BK = %i, RK = %i \n", BKF, RKF);
        BKLF = BKLF >> 10;
        RKLF = RKLF >> 10;
        BKHF = BKHF >> 10;
        RKHF = RKHF >> 10;

        roku_printf("RKLF =%i, RKHF = %i BKLF = %i, BKHF = %i \n", RKLF, RKHF, BKLF, BKHF);
    } else {
        BKL_hist = 0;
        RKL_hist = 0;
        BKH_hist = 0;
        RKH_hist = 0;
        BKLF = BKL;
        RKLF = RKL;
        BKHF = BKH;
        RKHF = RKH;
    }
//    roku_printf("BoostKnee = %i, ReduceKnee = %i \n", BK, RK);

    Out->ReduceKnee_high = (RKHF > 65535) ? 66635 : (RKHF < 0) ? 0 : (unsigned short)(RKHF);
    Out->BoostKnee_low = (BKLF > 65535) ? 66635 : (BKLF < 0) ? 0 : (unsigned short)(BKLF);
    Out->ReduceKnee_low = (RKLF > 65535) ? 66635 : (RKLF < 0) ? 0 : (unsigned short)(RKLF);
    Out->BoostKnee_high = (BKHF > 65535) ? 66635 : (BKHF < 0) ? 0 : (unsigned short)(BKHF);
    Out->ReduceKneeLow_hist = RKL_hist;
    Out->BoostKneeLow_hist = BKL_hist;
    Out->ReduceKneeHigh_hist = RKH_hist;
    Out->BoostKneeHigh_hist = BKH_hist;
}

// Map the LDValues to PWM value based on the Reduce & Boost Knees
// LDVal <= Reduce_Knee -> PWM = PWM_Min
// LDVal >= Boost_knee -> PWM = PWM_Max
// Reduce_knee < LDVal < BoostKnee -> PWM = LDVal*(PWM_Max-PWM_Min)/(BoostKnee-ReduceKnee)
// Mapping is only performed if Local Dimming is enabled
void PWM_Mapping(unsigned short* LEDValue_in, unsigned short ReduceKnee, unsigned short BoostKnee, unsigned short* PWM_out)
{
    int i;
    unsigned short RK = ReduceKnee;
    unsigned short BK = BoostKnee;
    unsigned short Denom, gain;
    uint64_t temp;
  
    Denom = (BK <= RK) ? 1 : BK - RK;
    gain = (a1.PWM_Max < a1.PWM_Min) ? 1 : (a1.PWM_Max - a1.PWM_Min);
   
    //perform mapping
    if (a1.PWM_MapEnable == 1) {
        for (i = 0; i < c1.zones; i++) {

            if (LEDValue_in[i] < RK) {
                PWM_out[i] = a1.PWM_Min;
            }
            else if (LEDValue_in[i] > BK) {
                PWM_out[i] = a1.PWM_Max;
            }
            else {
                temp = (uint64_t)(LEDValue_in[i] - RK) * gain;
                temp = temp / Denom + a1.PWM_Min;
                PWM_out[i] = (unsigned short)temp;
            }
        }
    } else {
        for (i = 0; i < c1.zones; i++) {
            PWM_out[i] = LEDValue_in[i];
        }
    }
//    roku_printf("KneeTFGain = %i, BoostKnee_last = %i, BoostKnee = %i, ReduceKnee_last = %i, ReduceKnee = %i \n", KneeTFGain, BKL, BK, RKL, RK); 
}

// this block will boost the drive current linearly for zones whose PWM is above a threshold.
//less boosting is applied to SDR through a register
//less boosting can be applied based on Local Dimming strength
void BoostDriveCurrent2(unsigned char* zone_type, unsigned short white_zones, unsigned short BoostKnee_low, unsigned short BoostKnee_high, unsigned short* PWM_in, unsigned short* DriveCurrent_in, uint64_t spare_power, uint64_t FSSum, unsigned char FullScreenFlag, unsigned short* DriveCurrent_boosted)
{
    int i;
    uint64_t zone_boost1, zbt, zbt1, zbf, zbt2a;
    uint64_t FSC;
    unsigned short BoostDenom, PWMScaleFactor;
    unsigned long PWMValueGained;
    unsigned short BKL;
    unsigned short BKH = BoostKnee_high;
    unsigned short FSCurrent;
    uint64_t FSDenom, FSNum, FSGain1;


    //calculate boost available per zone;
    //fix divide by 0 if there are no white zones.
    if (white_zones == 0) white_zones = 1;
    zone_boost1 = spare_power / white_zones;
 
    //calculate divider for PWM scaling and add protection for divide by 0
    BKL = BoostKnee_low;
    BoostDenom = ((BKH - BKL) > 0) ? BKH - BKL : 1;

    //calculate Full Screen Current



    FSDenom = c1.zones * (UINT16_MAX - a1.FSThreshold);
    FSDenom = (FSDenom == 0) ? 1 : FSDenom;
    FSNum = FSSum - c1.zones * a1.FSThreshold;
    FSGain1 = ParamReg_Max - (FSNum * (ParamReg_Max - a1.FSGain) / FSDenom);
    FSC = (uint64_t)(FSGain1)*a1.GlobalCurrent/ ParamReg_Max;
    FSCurrent = FSC > 65535 ? 65535 : (unsigned short)FSC;
//
    roku_printf("FSDenom = %"PRIu64",FSNum = %"PRIu64",FSGain = %"PRIu64" \n", FSDenom, FSNum, FSGain1);
    roku_printf("BoostGain = %d, white_zones = %d,  BoostSlopeGain = %d,  spare_power = %"PRIu64", zone_boost1 = % "PRIu64", BoostDenom = % u,  FSCurrent = % d \n", a1.BoostGain,  white_zones, a1.BoostSlopeGain, spare_power, zone_boost1, BoostDenom, FSCurrent);

    for (i = 0; i < c1.zones; i++) {
            //calculate zone based gain for each zone

            zbt = (a1.BoostSelect == 2) ? a1.MaxCurrent - a1.GlobalCurrent : zone_boost1;
            //calculate PWM based modifier
            PWMValueGained = (unsigned long)PWM_in[i] * a1.BoostSlopeGain;
            PWMValueGained = (PWMValueGained > UINT16_MAX) ? UINT16_MAX : PWMValueGained;

            PWMScaleFactor = (PWMValueGained < BKL) ? 0 : (unsigned short)PWMValueGained - BKL;

            //calculate final boost for each zone
            zbt1 = zbt * PWMScaleFactor;
            zbt2a = zbt1 / BoostDenom;
            DriveCurrent_tmp_[i] = zbt2a;

#ifdef VISUAL_STUDIO
 //           roku_printf("zone_type = %d, zbt = %"PRIu64", PWM = %d, PWMGained = %lu, PWMScaleFactor = %lu, zbt1 = %"PRIu64",  zbt2a = %"PRIu64" \n", zone_type[i],  zbt, PWM_in[i], PWMValueGained, PWMScaleFactor, zbt1, zbt2a);
#endif
    }

    if (FullScreenFlag == 1) {
        for (i = 0; i < c1.zones; i++) {
            DriveCurrent_boosted[i] = FSCurrent;
        }
    } else {
        for (i = 0; i < c1.zones; i++) {
            zbf = (zone_type[i] > 1) ? (uint64_t)DriveCurrent_tmp_[i] * a1.BoostGain / ParamReg_Max : 0;
            DriveCurrent_boosted[i] = ((DriveCurrent_in[i] + zbf) > a1.MaxCurrent) ? a1.MaxCurrent : (DriveCurrent_in[i] + (unsigned short)zbf);
#ifdef VISUAL_STUDIO
//          roku_printf("zone_boost_final = %d, BoostGain = %lu, zbf = %lu, DriveCurrent_boosted = %lu \n", DriveCurrent_tmp_[i], a1.BoostGain, zbf, DriveCurrent_boosted[i]);
#endif
        }
    }   
}
void RokuBoost_setMaxPower(uint64_t maxPower) {
    c1.PSCurrentMax = maxPower;
}
