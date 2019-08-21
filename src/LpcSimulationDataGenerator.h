#ifndef LPC_SIMULATION_DATA_GENERATOR
#define LPC_SIMULATION_DATA_GENERATOR

#include <AnalyzerHelpers.h>

class LpcAnalyzerSettings;

class LpcSimulationDataGenerator
{
public:
    LpcSimulationDataGenerator();
    ~LpcSimulationDataGenerator();

    void Initialize(U32 simulation_sample_rate, LpcAnalyzerSettings *settings);
    U32 GenerateSimulationData(U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor **simulation_channels);

protected:
    LpcAnalyzerSettings *mSettings;
    U32 mSimulationSampleRateHz;
    U64 mValue;

protected: //LPC specific
    ClockGenerator mClockGenerator;

    void CreateLpcTransaction(U8 type);
    void OutputNibble(U64 lad_data, bool start);


    SimulationChannelDescriptorGroup mLpcSimulationChannels;
	SimulationChannelDescriptor* mLAD0 = NULL;
	SimulationChannelDescriptor *mLAD1 = NULL;
	SimulationChannelDescriptor *mLAD2 = NULL;
	SimulationChannelDescriptor *mLAD3 = NULL;
	SimulationChannelDescriptor *mLCLK = NULL;
	SimulationChannelDescriptor *mLFRAME = NULL;;
};
#endif //LPC_SIMULATION_DATA_GENERATOR
