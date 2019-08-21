#include "LpcSimulationDataGenerator.h"
#include "LpcAnalyzerSettings.h"

LpcSimulationDataGenerator::LpcSimulationDataGenerator()
{
}

LpcSimulationDataGenerator::~LpcSimulationDataGenerator()
{
}

void LpcSimulationDataGenerator::Initialize(U32 simulation_sample_rate, LpcAnalyzerSettings *settings)
{
	mSimulationSampleRateHz = simulation_sample_rate;
	mSettings = settings;
	mClockGenerator.Init(33000000, simulation_sample_rate);

	mLAD0 = mLpcSimulationChannels.Add(settings->mLAD0Channel, mSimulationSampleRateHz, BIT_HIGH);
	mLAD1 = mLpcSimulationChannels.Add(settings->mLAD1Channel, mSimulationSampleRateHz, BIT_HIGH);
	mLAD2 = mLpcSimulationChannels.Add(settings->mLAD2Channel, mSimulationSampleRateHz, BIT_HIGH);
	mLAD3 = mLpcSimulationChannels.Add(settings->mLAD3Channel, mSimulationSampleRateHz, BIT_HIGH);
    mLCLK = mLpcSimulationChannels.Add(settings->mLCLKChannel, mSimulationSampleRateHz, BIT_HIGH);
	mLFRAME = mLpcSimulationChannels.Add(settings->mLFRAMEChannel, mSimulationSampleRateHz, BIT_HIGH);

    mLpcSimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(10.0));     //insert 10 bit-periods of idle

}

U32 LpcSimulationDataGenerator::GenerateSimulationData(U64 largest_sample_requested, U32 sample_rate, SimulationChannelDescriptor **simulation_channels)
{
    U64 adjusted_largest_sample_requested = AnalyzerHelpers::AdjustSimulationTargetSample(largest_sample_requested, sample_rate, mSimulationSampleRateHz);

    while (mLCLK->GetCurrentSampleNumber() < adjusted_largest_sample_requested) {
        CreateLpcTransaction(0); //IO Write
		CreateLpcTransaction(4); //idle clocks (20 clocks)
		CreateLpcTransaction(1); //IO Read
		CreateLpcTransaction(4); //idle clocks (20 clocks)
		CreateLpcTransaction(2); //Memory Write
		CreateLpcTransaction(4); //idle clocks (20 clocks)
		CreateLpcTransaction(3); //Memory Read
		CreateLpcTransaction(4); //idle clocks (20 clocks)
    }

    *simulation_channels = mLpcSimulationChannels.GetArray();
    return mLpcSimulationChannels.GetCount();
}

void LpcSimulationDataGenerator::CreateLpcTransaction(U8 type)
{
	switch (type) {
	case 0:
		//Generate a IO Write
		OutputNibble(0b0000, true); //Start
		OutputNibble(0b0010, false); //IO Write
		OutputNibble(0b0000, false); //ADD1 (MSNibble First)
		OutputNibble(0b0000, false); //ADD2 (MSNibble First)
		OutputNibble(0b1110, false); //ADD3 (MSNibble First)
		OutputNibble(0b0011, false); //ADD4 (LSNibble First)
		OutputNibble(0b0111, false); //DATA1 (LSNibble First)
		OutputNibble(0b0101, false); //DATA2 (LSNibble First)
		OutputNibble(0b1111, false); //TAR
		OutputNibble(0b1111, false); //TAR
		OutputNibble(0b0101, false); //Short wait
		OutputNibble(0b0101, false); //Short wait
		OutputNibble(0b0101, false); //Short wait
		OutputNibble(0b0000, false); //Sync OK
		OutputNibble(0b1111, false); //TAR
		OutputNibble(0b1111, false); //TAR
		break;
	case 1:
		//Generate a IO Read
		OutputNibble(0b0000, true); //Start
		OutputNibble(0b0000, false); //IO Write
		OutputNibble(0b0000, false); //ADD1 (MSNibble First)
		OutputNibble(0b0000, false); //ADD2 (MSNibble First)
		OutputNibble(0b1110, false); //ADD3 (MSNibble First)
		OutputNibble(0b0011, false); //ADD4 (LSNibble First)
		OutputNibble(0b1111, false); //TAR
		OutputNibble(0b1111, false); //TAR
		OutputNibble(0b0101, false); //Short wait
		OutputNibble(0b0101, false); //Short wait
		OutputNibble(0b0101, false); //Short wait
		OutputNibble(0b0000, false); //Sync OK
		OutputNibble(0b0111, false); //DATA1 (LSNibble First)
		OutputNibble(0b0101, false); //DATA2 (LSNibble First)
		OutputNibble(0b1111, false); //TAR
		OutputNibble(0b1111, false); //TAR
		break;

	case 2:
		//Generate a Memory Write
		OutputNibble(0b0000, 1); //Start
		OutputNibble(0b0110, 0); //Memory Write
		OutputNibble(0b1111, 0); //ADD1 (MSNibble First)
		OutputNibble(0b1111, 0); //ADD2 (MSNibble First)
		OutputNibble(0b0000, 0); //ADD3 (MSNibble First)
		OutputNibble(0b0000, 0); //ADD4 (MSNibble First)
		OutputNibble(0b1010, 0); //ADD5 (MSNibble First)
		OutputNibble(0b1010, 0); //ADD6 (MSNibble First)
		OutputNibble(0b1010, 0); //ADD7 (MSNibble First)
		OutputNibble(0b1010, 0); //ADD8 (MSNibble First)
		OutputNibble(0b0000, 0); //DATA1 (LSNibble First)
		OutputNibble(0b0011, 0); //DATA2 (LSNibble First)
		OutputNibble(0b1111, 0); //TAR
		OutputNibble(0b1111, 0); //TAR
		OutputNibble(0b0101, 0); //Short wait
		OutputNibble(0b0101, 0); //Short wait
		OutputNibble(0b0101, 0); //Short wait
		OutputNibble(0b0000, 0); //Sync OK
		OutputNibble(0b1111, 0); //TAR
		OutputNibble(0b1111, 0); //TAR
		break;

	case 3:
		//Generate a Memory Read
		OutputNibble(0b0000, 1); //Start
		OutputNibble(0b0100, 0); //Memory Write
		OutputNibble(0b1111, 0); //ADD1 (MSNibble First)
		OutputNibble(0b1111, 0); //ADD2 (MSNibble First)
		OutputNibble(0b0000, 0); //ADD3 (MSNibble First)
		OutputNibble(0b0000, 0); //ADD4 (MSNibble First)
		OutputNibble(0b1010, 0); //ADD5 (MSNibble First)
		OutputNibble(0b1010, 0); //ADD6 (MSNibble First)
		OutputNibble(0b1010, 0); //ADD7 (MSNibble First)
		OutputNibble(0b1010, 0); //ADD8 (MSNibble First)
		OutputNibble(0b1111, 0); //TAR
		OutputNibble(0b1111, 0); //TAR
		OutputNibble(0b0101, 0); //Short wait
		OutputNibble(0b0101, 0); //Short wait
		OutputNibble(0b0101, 0); //Short wait
		OutputNibble(0b0000, 0); //Sync OK
		OutputNibble(0b0000, 0); //DATA1 (LSNibble First)
		OutputNibble(0b0011, 0); //DATA2 (LSNibble First)
		OutputNibble(0b1111, 0); //TAR
		OutputNibble(0b1111, 0); //TAR
		break;

	case 4:
		for (U32 i = 0; i < 10; i++) {
			OutputNibble(0b1111, 0); //idle clocks
		}
		

		break;


	}
}

void LpcSimulationDataGenerator::OutputNibble(U64 lad_data, bool start)
{

	BitExtractor lad_bits(lad_data, AnalyzerEnums::MsbFirst, 4);
	mLAD3->TransitionIfNeeded(lad_bits.GetNextBit());
	mLAD2->TransitionIfNeeded(lad_bits.GetNextBit());
	mLAD1->TransitionIfNeeded(lad_bits.GetNextBit());
	mLAD0->TransitionIfNeeded(lad_bits.GetNextBit());


	if (start) {
		mLFRAME->TransitionIfNeeded(BIT_LOW);
	}

	mLpcSimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(.25));
	mLCLK->TransitionIfNeeded(BIT_LOW);  //data valid
	mLpcSimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(.5));
	mLCLK->TransitionIfNeeded(BIT_HIGH);  //data valid
	mLpcSimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(.25));

	if (start) {
		mLFRAME->TransitionIfNeeded(BIT_HIGH);
	}
}
