#ifndef LPC_ANALYZER_SETTINGS
#define LPC_ANALYZER_SETTINGS

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>

class LpcAnalyzerSettings : public AnalyzerSettings
{
public:
    LpcAnalyzerSettings();
    virtual ~LpcAnalyzerSettings();

    virtual bool SetSettingsFromInterfaces();
    virtual void LoadSettings(const char *settings);
    virtual const char *SaveSettings();
    void UpdateInterfacesFromSettings();

    Channel mLAD0Channel;
	Channel mLAD1Channel;
	Channel mLAD2Channel;
	Channel mLAD3Channel;
    Channel mLCLKChannel;
    Channel mLFRAMEChannel;
    bool  mShowMarker;

protected:
    std::auto_ptr< AnalyzerSettingInterfaceChannel >    mLAD0ChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >    mLAD1ChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >    mLAD2ChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >    mLAD3ChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >    mLCLKChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >    mLFRAMEChannelInterface;
    std::auto_ptr< AnalyzerSettingInterfaceBool >       mUseShowMarkerInterface;
};

#endif //LPC_ANALYZER_SETTINGS
