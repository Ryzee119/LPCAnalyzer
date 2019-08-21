#ifndef LPC_ANALYZER_RESULTS
#define LPC_ANALYZER_RESULTS

#include <AnalyzerResults.h>

#define LPC_ERROR_FLAG ( 1 << 0 )

class LpcAnalyzer;
class LpcAnalyzerSettings;

class LpcAnalyzerResults : public AnalyzerResults
{
public:
    LpcAnalyzerResults(LpcAnalyzer *analyzer, LpcAnalyzerSettings *settings);
    virtual ~LpcAnalyzerResults();

    virtual void GenerateBubbleText(U64 frame_index, Channel &channel, DisplayBase display_base);
    virtual void GenerateExportFile(const char *file, DisplayBase display_base, U32 export_type_user_id);

    virtual void GenerateFrameTabularText(U64 frame_index, DisplayBase display_base);
    virtual void GeneratePacketTabularText(U64 packet_id, DisplayBase display_base);
    virtual void GenerateTransactionTabularText(U64 transaction_id, DisplayBase display_base);

protected: //functions

protected: //vars
    LpcAnalyzerSettings *mSettings;
    LpcAnalyzer *mAnalyzer;
};

#endif //LPC_ANALYZER_RESULTS
