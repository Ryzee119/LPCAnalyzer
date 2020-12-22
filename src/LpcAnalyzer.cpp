
#include "LpcAnalyzer.h"
#include "LpcAnalyzerSettings.h"
#include <AnalyzerChannelData.h>

LpcAnalyzer::LpcAnalyzer()
	: Analyzer(),
	mSimulationInitilized(false),
	mLAD0(NULL),
	mLAD1(NULL),
	mLAD2(NULL),
	mLAD3(NULL),
	mLCLK(NULL),
	mLFRAME(NULL)

{
    mSettings = std::make_shared<LpcAnalyzerSettings>();
	SetAnalyzerSettings(mSettings.get());
}

LpcAnalyzer::~LpcAnalyzer()
{
    KillThread();
}

void LpcAnalyzer::SetupResults()
{
	mResults = std::make_shared<LpcAnalyzerResults>(this, mSettings);
	SetAnalyzerResults(mResults.get());

	mResults->AddChannelBubblesWillAppearOn(mSettings->mLCLKChannel);
	mResults->AddChannelBubblesWillAppearOn(mSettings->mLFRAMEChannel);

}

void LpcAnalyzer::WorkerThread()
{
    Setup();

    mResults->CommitPacketAndStartNewPacket();
    mResults->CommitResults();

    mCurrentSample = mLCLK->GetSampleNumber();

	mLAD1 = GetAnalyzerChannelData(mSettings->mLAD1Channel);
	
	for (; ;) {
		AdvanceToActiveLFRAMEEdge();
		GetLpcPacket();
		CheckIfThreadShouldExit();
	}
    
}

void LpcAnalyzer::Setup()
{
	mMarker = AnalyzerResults::Dot;


	mLAD0 = GetAnalyzerChannelData(mSettings->mLAD0Channel);
	mLAD1 = GetAnalyzerChannelData(mSettings->mLAD1Channel);
	mLAD2 = GetAnalyzerChannelData(mSettings->mLAD2Channel);
	mLAD3 = GetAnalyzerChannelData(mSettings->mLAD3Channel);
	mLCLK = GetAnalyzerChannelData(mSettings->mLCLKChannel);
	mLFRAME = GetAnalyzerChannelData(mSettings->mLFRAMEChannel);

}

void LpcAnalyzer::AdvanceToActiveLFRAMEEdge()
{
	if (mLFRAME->GetBitState() != 0) {
		mLFRAME->AdvanceToNextEdge();
	}
	else {
		mLFRAME->AdvanceToNextEdge();
		mLFRAME->AdvanceToNextEdge();
	}
	mCurrentSample = mLFRAME->GetSampleNumber();
	mLCLK->AdvanceToAbsPosition(mCurrentSample);
}

void LpcAnalyzer::ReadLADS(DataBuilder* lad_result)
{
	mLAD3->AdvanceToAbsPosition(mCurrentSample);
	lad_result->AddBit(mLAD3->GetBitState());
	mLAD2->AdvanceToAbsPosition(mCurrentSample);
	lad_result->AddBit(mLAD2->GetBitState());
	mLAD1->AdvanceToAbsPosition(mCurrentSample);
	lad_result->AddBit(mLAD1->GetBitState());
	mLAD0->AdvanceToAbsPosition(mCurrentSample);
	lad_result->AddBit(mLAD0->GetBitState());
	mMarkerLocations.push_back(mCurrentSample);
}


void LpcAnalyzer::GetLpcPacket()
{
	//We assume we come into this function at the start of the LFRAME drop;
	DataBuilder lad_result;
	Frame result_frame;

	LPC_STATE lpc_pos = START; //Track state of LPC transaction
	U64 lad_nibble; //Stores all the bits from a nibble in a frame.
	U8 clock_delay = 0; //used to add clock waits for states that have multiple nibbles
	U8 cyc_type = 0;
	U8 sync_count = 0; //Counts sync attempts
	const U8 MAX_SYNC_ATTEMPT = 16; //Abritrary amount of times to wait for a sync before returning an error


	mMarkerLocations.clear();
	ReportProgress(mLCLK->GetSampleNumber());

	//Let's read the LPC transaction until to end
	while (lpc_pos != COMPLETE) {

		lad_result.Reset(&lad_nibble, AnalyzerEnums::MsbFirst, 4);
		mLCLK->AdvanceToNextEdge(); //Advance to the next falling edge of LCLK
		result_frame.mStartingSampleInclusive= mLCLK->GetSampleNumber(); //Frame starts at beginning of clock edge
		result_frame.mFlags = NO_ERROR;

		//Advance the half way along clock period. This is where the LADs are read.
		mLCLK->Advance((mLCLK->GetSampleOfNextEdge() - mLCLK->GetSampleNumber()) / 2);
		mCurrentSample = mLCLK->GetSampleNumber();
		ReadLADS(&lad_result); //Build the 4 bits into a nibble and put into lad_nibble array.

		//Advance mLFRAME to current sample.
		mLFRAME->AdvanceToAbsPosition(mCurrentSample);
		
		//Put the current lpc state into the results frame. mData2 stores the lpc_state.
		result_frame.mData2 = lpc_pos;

		//If LFRAME goes low during transaction something is wrong.
		if (lpc_pos != START && mLFRAME->GetBitState() == BIT_LOW) {
			result_frame.mFlags |= FRAME_ERROR;
			lpc_pos = COMPLETE;
			return;
		}


		//Add a marker to indicate the beginning/middle/end of transaction
		mMarkerLocations.push_back(mCurrentSample);
		if (mSettings->mShowMarker) {
			if (lpc_pos == START) {
				mMarker = AnalyzerResults::Start;
			}
			else if (lpc_pos == TAR) {
				mMarker = AnalyzerResults::Stop;
			}
			else {
				mMarker = AnalyzerResults::Dot;
			}
			mResults->AddMarker(mCurrentSample, mMarker, mSettings->mLCLKChannel);
		}


		//Work through the lpc state machine
		switch (lpc_pos) {
			/* Work out what type of transaction it is */
		case START:
			if (lad_nibble == 0b0000 && mLFRAME->GetBitState() == BIT_LOW) {
				lpc_pos = CYCTYPE;
			}
			else {
				lpc_pos = COMPLETE;
				return;
			}
			break;

		case CYCTYPE:
			cyc_type = ((U8)lad_nibble) & 0b00001110;
			switch (cyc_type) {
			case 0b0000:
				lpc_pos = IO_READ_ADD;
				clock_delay = 0;
				break;

			case 0b0010:
				lpc_pos = IO_WRITE_ADD;
				clock_delay = 0;
				break;

			case 0b0100:
				lpc_pos = MEM_READ_ADD;
				clock_delay = 0;
				break;

			case 0b0110:
				lpc_pos = MEM_WRITE_ADD;
				clock_delay = 0;
				break;
			default:
				lpc_pos = COMPLETE; //Not supported
				break;
			}
			break;


			/* IO READ */
		case IO_READ_ADD:
			if (clock_delay == 3) {
				lpc_pos = IO_READ_TAR;
				clock_delay = 0;
			}
			else {
				clock_delay++;
			}
			break;

		case IO_READ_TAR:
			if (clock_delay == 1) {
				lpc_pos = IO_READ_SYNC;
				clock_delay = 0;
			}
			else {
				clock_delay++;
			}
			break;

		case IO_READ_SYNC:
			if (lad_nibble == 0b0000) {
				lpc_pos = IO_READ_DATA;
			}
			else {
				sync_count++;
				if (sync_count == MAX_SYNC_ATTEMPT) {
					lpc_pos = COMPLETE;
					result_frame.mFlags |= SYNC_ERROR;
				}	
			}
			break;

		case IO_READ_DATA:
			if (clock_delay == 1) {
				lpc_pos = TAR;
				clock_delay = 0;
			}
			else {
				clock_delay++;
			}
			break;

			/* IO WRITE */
		case IO_WRITE_ADD:
			if (clock_delay == 3) {
				lpc_pos = IO_WRITE_DATA;
				clock_delay = 0;
			}
			else {
				clock_delay++;
			}
			break;

		case IO_WRITE_DATA:
			if (clock_delay == 1) {
				lpc_pos = IO_WRITE_TAR;
				clock_delay = 0;
			}
			else {
				clock_delay++;
			}
			break;

		case IO_WRITE_TAR:
			if (clock_delay == 1) {
				lpc_pos = IO_WRITE_SYNC;
			}
			else {
				clock_delay++;
			}
			break;

		case IO_WRITE_SYNC:
			if (lad_nibble == 0b0000) {
				lpc_pos = TAR;
				clock_delay = 0;
			}
			else {
				sync_count++;
				if (sync_count == MAX_SYNC_ATTEMPT) {
					lpc_pos = COMPLETE;
					result_frame.mFlags |= SYNC_ERROR;
				}
			}
			break;

			/* MEMORY WRITE */
		case MEM_WRITE_ADD:
			if (clock_delay == 7) {
				lpc_pos = MEM_WRITE_DATA;
				clock_delay = 0;
			}
			else {
				clock_delay++;
			}
			break;

		case MEM_WRITE_DATA:
			if (clock_delay == 1) {
				lpc_pos = MEM_WRITE_TAR;
				clock_delay = 0;
			}
			else {
				clock_delay++;
			}
			break;

		case MEM_WRITE_TAR:
			if (clock_delay == 1) {
				lpc_pos = MEM_WRITE_SYNC;
			}
			else {
				clock_delay++;
			}
			break;

		case MEM_WRITE_SYNC:
			if (lad_nibble == 0b0000) {
				lpc_pos = TAR;
				clock_delay = 0;
			}
			else {
				sync_count++;
				if (sync_count == MAX_SYNC_ATTEMPT) {
					lpc_pos = COMPLETE;
					result_frame.mFlags |= SYNC_ERROR;
				}
			}
			break;

			/* MEMORY READ */
		case MEM_READ_ADD:
			if (clock_delay == 7) {
				lpc_pos = MEM_READ_TAR;
				clock_delay = 0;
			}
			else {
				clock_delay++;
			}
			break;

		case MEM_READ_TAR:
			if (clock_delay == 1) {
				lpc_pos = MEM_READ_SYNC;
			}
			else {
				clock_delay++;
			}
			break;

		case MEM_READ_SYNC:
			if (lad_nibble == 0b0000) {
				lpc_pos = MEM_READ_DATA;
				clock_delay = 0;
			}
			else {
				sync_count++;
				if (sync_count == MAX_SYNC_ATTEMPT) {
					lpc_pos = COMPLETE;
					result_frame.mFlags |= SYNC_ERROR;
				}
			}
			break;

		case MEM_READ_DATA:
			if (clock_delay == 1) {
				lpc_pos = TAR;
				clock_delay = 0;
			}
			else {
				clock_delay++;
			}
			break;
		
		case TAR:
			if (clock_delay == 1) {
				lpc_pos = COMPLETE;
				clock_delay = 0;
			}
			else {
				clock_delay++;
			}
			break;
        default:
            break;
		}


		mLCLK->AdvanceToNextEdge(); //Advance to rising edge of LCLK
		result_frame.mEndingSampleInclusive = mLCLK->GetSampleOfNextEdge(); //Frame ends at the falling edge of the next clock
		result_frame.mData1 = lad_nibble; //store the lad values into the frame. mData1 stores the lads.
		mResults->AddFrame(result_frame);
		mResults->CommitResults();
		
	}

	


}

bool LpcAnalyzer::NeedsRerun()
{
    return false;
}

U32 LpcAnalyzer::GenerateSimulationData(U64 minimum_sample_index, U32 device_sample_rate, SimulationChannelDescriptor **simulation_channels)
{
    if (mSimulationInitilized == false) {
        mSimulationDataGenerator.Initialize(GetSimulationSampleRate(), mSettings.get());
        mSimulationInitilized = true;
    }

    return mSimulationDataGenerator.GenerateSimulationData(minimum_sample_index, device_sample_rate, simulation_channels);
}


U32 LpcAnalyzer::GetMinimumSampleRateHz()
{
    return 10000; //Doesnt really matter
}

const char *LpcAnalyzer::GetAnalyzerName() const
{
    return "LPC";
}

const char *GetAnalyzerName()
{
    return "LPC";
}

Analyzer *CreateAnalyzer()
{
    return new LpcAnalyzer();
}

void DestroyAnalyzer(Analyzer *analyzer)
{
    delete analyzer;
}
