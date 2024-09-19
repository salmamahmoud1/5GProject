#pragma once
#include <string>
#include <iomanip> 
using namespace std;
class PacketConfig {

private:
    int lineRateGbps;
    int captureSizeMs;
    int minNumOfIFGsPerPacket;
    string destAddress;
    string sourceAddress;
    int maxPacketSize;
    int burstSize;
    int burstPeriodicityUs;
    int added_IFGS;

public:
// Hard-coded values
const string IFG = "07"; // Inter-Frame Gap
const string PreambleSFD = "FB555555555555D5"; // Preamble + SFD
const string ethertype ="0800";
int payloadsize;
int no_of_packets;
 // Getters
    int getLineRateGbps();
    int getCaptureSizeMs();
    int getMinNumOfIFGsPerPacket();
    string getDestAddress();
    string getSourceAddress();
    int getMaxPacketSize();
    int getBurstSize();
    int getBurstPeriodicityUs();
    int getAdded_IFGS();
    int getno_of_bursts();


    // Setters
    void setLineRateGbps(int lineRate);
    void setCaptureSizeMs(int captureSize);
    void setMinNumOfIFGsPerPacket(int minNumOfIFGs);
    void setDestAddress(string& destAddr);
    void setSourceAddress(string& srcAddr);
    void setMaxPacketSize(int maxSize);
    void setBurstSize(int burstSize);
    void setBurstPeriodicityUs(int burstPeriodicity);
    void setAddedIFGS();
    // Method to load configuration from file
    bool loadConfig(string& configFilePath);



};

