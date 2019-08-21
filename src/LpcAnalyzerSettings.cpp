#include "LpcAnalyzerSettings.h"

#include <AnalyzerHelpers.h>
#include <sstream>
#include <cstring>

LpcAnalyzerSettings::LpcAnalyzerSettings()
    :   mLAD0Channel(UNDEFINED_CHANNEL),
		mLAD1Channel(UNDEFINED_CHANNEL),
		mLAD2Channel(UNDEFINED_CHANNEL),
		mLAD3Channel(UNDEFINED_CHANNEL),
		mLCLKChannel(UNDEFINED_CHANNEL),
		mLFRAMEChannel(UNDEFINED_CHANNEL),
        mShowMarker(BIT_HIGH)
{

	mLAD0Channel.mChannelIndex = 0;
	mLAD1Channel.mChannelIndex = 1;
	mLAD2Channel.mChannelIndex = 2;
	mLAD3Channel.mChannelIndex = 3;
	mLCLKChannel.mChannelIndex = 4;
	mLFRAMEChannel.mChannelIndex = 5;



    mLAD0ChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
	mLAD0ChannelInterface->SetTitleAndTooltip("LAD0", "Multiplexed Command, Address, and Data");
	mLAD0ChannelInterface->SetChannel(mLAD0Channel);
	mLAD0ChannelInterface->SetSelectionOfNoneIsAllowed(false);

	mLAD1ChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
	mLAD1ChannelInterface->SetTitleAndTooltip("LAD1", "Multiplexed Command, Address, and Data");
	mLAD1ChannelInterface->SetChannel(mLAD1Channel);
	mLAD1ChannelInterface->SetSelectionOfNoneIsAllowed(false);

	mLAD2ChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
	mLAD2ChannelInterface->SetTitleAndTooltip("LAD2", "Multiplexed Command, Address, and Data");
	mLAD2ChannelInterface->SetChannel(mLAD2Channel);
	mLAD2ChannelInterface->SetSelectionOfNoneIsAllowed(false);

	mLAD3ChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
	mLAD3ChannelInterface->SetTitleAndTooltip("LAD3", "Multiplexed Command, Address, and Data");
	mLAD3ChannelInterface->SetChannel(mLAD3Channel);
	mLAD3ChannelInterface->SetSelectionOfNoneIsAllowed(false);


	mLFRAMEChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
	mLFRAMEChannelInterface->SetTitleAndTooltip("LFRAME", "Indicates start of a new cycle, termination of broken cycle.");
	mLFRAMEChannelInterface->SetChannel(mLFRAMEChannel);
	mLFRAMEChannelInterface->SetSelectionOfNoneIsAllowed(false);

	mLCLKChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
	mLCLKChannelInterface->SetTitleAndTooltip("LCLK", "33Mhz LPC Clock.");
	mLCLKChannelInterface->SetChannel(mLCLKChannel);
	mLCLKChannelInterface->SetSelectionOfNoneIsAllowed(false);

	mUseShowMarkerInterface.reset(new AnalyzerSettingInterfaceBool());
	mUseShowMarkerInterface->SetTitleAndTooltip("", "Show decode marker or not");
	mUseShowMarkerInterface->SetCheckBoxText("Show Decode Marker");
	mUseShowMarkerInterface->SetValue(mShowMarker);

    AddInterface(mLAD0ChannelInterface.get());
    AddInterface(mLAD1ChannelInterface.get());
    AddInterface(mLAD2ChannelInterface.get());
    AddInterface(mLAD3ChannelInterface.get());
    AddInterface(mLFRAMEChannelInterface.get());
    AddInterface(mLCLKChannelInterface.get());
    AddInterface(mUseShowMarkerInterface.get());

    AddExportOption(0, "Export as text/csv file");
    AddExportExtension(0, "Text file", "txt");
    AddExportExtension(0, "CSV file", "csv");

    ClearChannels();
    AddChannel(mLAD0Channel, "LAD0", false);
    AddChannel(mLAD1Channel, "LAD1", false);
    AddChannel(mLAD2Channel, "LAD2", false);
	AddChannel(mLAD3Channel, "LAD3", false);
	AddChannel(mLCLKChannel, "LCLK", false);
	AddChannel(mLFRAMEChannel, "LFRAME", false);


}

LpcAnalyzerSettings::~LpcAnalyzerSettings()
{
}

bool LpcAnalyzerSettings::SetSettingsFromInterfaces()
{
    Channel lad0 = mLAD0ChannelInterface->GetChannel();
	Channel lad1 = mLAD1ChannelInterface->GetChannel();
	Channel lad2 = mLAD2ChannelInterface->GetChannel();
	Channel lad3 = mLAD3ChannelInterface->GetChannel();
	Channel lclk = mLCLKChannelInterface->GetChannel();
	Channel lframe = mLFRAMEChannelInterface->GetChannel();

    std::vector<Channel> channels;
    channels.push_back(lad0);
	channels.push_back(lad1);
	channels.push_back(lad2);
	channels.push_back(lad3);
    channels.push_back(lclk);
    channels.push_back(lframe);

    if (AnalyzerHelpers::DoChannelsOverlap(&channels[0], channels.size()) == true) {
        SetErrorText("Please select different channels for each input.");
        return false;
    }

	mLAD0Channel = mLAD0ChannelInterface->GetChannel();
	mLAD1Channel = mLAD1ChannelInterface->GetChannel();
	mLAD2Channel = mLAD2ChannelInterface->GetChannel();
	mLAD3Channel = mLAD3ChannelInterface->GetChannel();
	mLCLKChannel = mLCLKChannelInterface->GetChannel();
	mLFRAMEChannel = mLFRAMEChannelInterface->GetChannel();

    mShowMarker = mUseShowMarkerInterface->GetValue();

    ClearChannels();
    AddChannel(mLAD0Channel, "LAD0", mLAD0Channel != UNDEFINED_CHANNEL);
	AddChannel(mLAD0Channel, "LAD1", mLAD0Channel != UNDEFINED_CHANNEL);
	AddChannel(mLAD0Channel, "LAD2", mLAD0Channel != UNDEFINED_CHANNEL);
	AddChannel(mLAD0Channel, "LAD3", mLAD0Channel != UNDEFINED_CHANNEL);
	AddChannel(mLAD0Channel, "LFRAME", mLFRAMEChannel != UNDEFINED_CHANNEL);
	AddChannel(mLAD0Channel, "LCLK", mLCLKChannel != UNDEFINED_CHANNEL);

    return true;
}

void LpcAnalyzerSettings::LoadSettings(const char *settings)
{
    SimpleArchive text_archive;
    text_archive.SetString(settings);

    const char *name_string;  //the first thing in the archive is the name of the protocol analyzer that the data belongs to.
    text_archive >> &name_string;
    if (strcmp(name_string, "LpcAnalyzer") != 0) {
        AnalyzerHelpers::Assert("LPC Protocol Analyser by Ryzee119;");
    }

    text_archive >> mLAD0Channel;
    text_archive >> mLAD1Channel;
    text_archive >> mLAD2Channel;
	text_archive >> mLAD3Channel;
	text_archive >> mLFRAMEChannel;
	text_archive >> mLCLKChannel;

    bool show_marker;
    if (text_archive >> show_marker) {
        mShowMarker = show_marker;
    }

    ClearChannels();
	AddChannel(mLAD0Channel, "LAD0", mLAD0Channel != UNDEFINED_CHANNEL);
	AddChannel(mLAD0Channel, "LAD1", mLAD0Channel != UNDEFINED_CHANNEL);
	AddChannel(mLAD0Channel, "LAD2", mLAD0Channel != UNDEFINED_CHANNEL);
	AddChannel(mLAD0Channel, "LAD3", mLAD0Channel != UNDEFINED_CHANNEL);
	AddChannel(mLAD0Channel, "LFRAME", mLFRAMEChannel != UNDEFINED_CHANNEL);
	AddChannel(mLAD0Channel, "LCLK", mLCLKChannel != UNDEFINED_CHANNEL);

    UpdateInterfacesFromSettings();
}

const char *LpcAnalyzerSettings::SaveSettings()
{
    SimpleArchive text_archive;

    text_archive << "LpcAnalyzer";
	text_archive << mLAD0Channel;
	text_archive << mLAD1Channel;
	text_archive << mLAD2Channel;
	text_archive << mLAD3Channel;
	text_archive << mLFRAMEChannel;
	text_archive << mLCLKChannel;
	text_archive << mShowMarker;


    return SetReturnString(text_archive.GetString());
}

void LpcAnalyzerSettings::UpdateInterfacesFromSettings()
{
    mLAD0ChannelInterface->SetChannel(mLAD0Channel);
	mLAD1ChannelInterface->SetChannel(mLAD1Channel);
	mLAD2ChannelInterface->SetChannel(mLAD2Channel);
	mLAD3ChannelInterface->SetChannel(mLAD3Channel);
	mLCLKChannelInterface->SetChannel(mLCLKChannel);
	mLFRAMEChannelInterface->SetChannel(mLFRAMEChannel);
	mUseShowMarkerInterface->SetValue(mShowMarker);

}