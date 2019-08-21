#ifndef LPC_ANALYZER_H
#define LPC_ANALYZER_H

#include <Analyzer.h>
#include "LpcAnalyzerResults.h"
#include "LpcSimulationDataGenerator.h"

class LpcAnalyzerSettings;

class ANALYZER_EXPORT LpcAnalyzer : public Analyzer
{
public:
    LpcAnalyzer();
    virtual ~LpcAnalyzer();
    virtual void SetupResults();
    virtual void WorkerThread();

    virtual U32 GenerateSimulationData(U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor **simulation_channels);
    virtual U32 GetMinimumSampleRateHz();

    virtual const char *GetAnalyzerName() const;
    virtual bool NeedsRerun();

	//Epic LPC State Machine
	enum LPC_STATE {
		START, //1 clock
		CYCTYPE, //1 clock

		//IO READ
		IO_READ_ADD, //4 clocks
		IO_READ_TAR, //2 clocks 1111's
		IO_READ_SYNC, //0000 on complete
		IO_READ_DATA, //2 clocks

		//IO WRITE
		IO_WRITE_ADD, //4 clocks
		IO_WRITE_DATA, //2 clocks
		IO_WRITE_TAR, //2 clocks 1111's
		IO_WRITE_SYNC, //0000 on complete

		//MEMORY READ
		MEM_READ_ADD, //8clocks
		MEM_READ_TAR, //2clocks
		MEM_READ_SYNC, //0000 on complete
		MEM_READ_DATA, //2clocks

		//MEMORY WRITE
		MEM_WRITE_ADD, //8clocks
		MEM_WRITE_DATA, //2clocks
		MEM_WRITE_TAR, //2clocks
		MEM_WRITE_SYNC, //0000 on complete

		TAR, //2 clocks
		COMPLETE
	};

	U8 NO_ERROR = 0;
	U8 SYNC_ERROR = (1 << 0);
	U8 FRAME_ERROR = (1 << 1);

protected: //functions
    void Setup();
    void AdvanceToActiveLFRAMEEdge();
	void ReadLADS(DataBuilder* lad_result);
    void GetLpcPacket();
	AnalyzerResults::MarkerType mMarker;



#pragma warning( push )
#pragma warning( disable : 4251 ) //warning C4251: 'SerialAnalyzer::<...>' : class <...> needs to have dll-interface to be used by clients of class
protected:  //vars
    std::auto_ptr< LpcAnalyzerSettings > mSettings;
    std::auto_ptr< LpcAnalyzerResults > mResults;
    bool mSimulationInitilized;
    LpcSimulationDataGenerator mSimulationDataGenerator;

    AnalyzerChannelData *mLAD0;
	AnalyzerChannelData* mLAD1;
	AnalyzerChannelData* mLAD2;
	AnalyzerChannelData* mLAD3;
    AnalyzerChannelData *mLCLK;
    AnalyzerChannelData *mLFRAME;

    U64 mCurrentSample;
    AnalyzerResults::MarkerType mArrowMarker;
    std::vector<U64> mMarkerLocations;

#pragma warning( pop )
};

extern "C" ANALYZER_EXPORT const char *__cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer *__cdecl CreateAnalyzer();
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer(Analyzer *analyzer);

#endif //LPC_ANALYZER_H
