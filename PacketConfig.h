#pragma once
#include <string>
#include <iomanip> 
#include<vector>
#include<cmath>
using namespace std;
class PacketConfig {

private:
    int lineRateGbps;
    int captureSizeMs;
    int minNumOfIFGsPerPacket;
    string destAddress;
    string sourceAddress;
    int maxPacketSize;
    int oranSCS;
    int oranMaxNrb;
    int oranNrbPerPacket;
    string payloadTypeRandom;
    string oranPayloadFile;
    int added_IFGS;
    int ethernetpayloadsize;
    int oranpayloadsize;
    int ecpriheadersize=8;
    


public:
// Hard-coded values
const string IFG = "07"; // Inter-Frame Gap
const string PreambleSFD = "FB555555555555D5"; // Preamble + SFD
const string ethertype ="0800";
int no_of_packets;
 // Getters
    int getLineRateGbps();
    int getCaptureSizeMs();
    int getMinNumOfIFGsPerPacket();
    string getDestAddress();
    string getSourceAddress();
    int getMaxPacketSize();
    int getAdded_IFGS();
    int getOranSCS();
    int getOranMaxNrb();
    int getOranNrbPerPacket();
    int getethernetpayloadsize();
    int getoranpayloadsize();
    int getecpriheadersize();
    string get_iq_filename();


    // Setters
    void setLineRateGbps(int lineRate);
    void setCaptureSizeMs(int captureSize);
    void setMinNumOfIFGsPerPacket(int minNumOfIFGs);
    void setDestAddress(string& destAddr);
    void setSourceAddress(string& srcAddr);
    void setMaxPacketSize(int maxSize);
    void setAddedIFGS(int bytes);
    void setethernetpayloadsize();
    void setoranpayloadsize();
    // Method to load configuration from file
    bool loadConfig(string& configFilePath);
    int calculate_no_of_packets();
   

};

