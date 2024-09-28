#include "PacketConfig.h"
#include <fstream>
#include <iostream>
#include <sstream> 
#include <cctype>  
using namespace std;



// Getters
int PacketConfig::getLineRateGbps(){ return lineRateGbps;}
int PacketConfig::getCaptureSizeMs(){ return captureSizeMs;}
int PacketConfig::getMinNumOfIFGsPerPacket(){ return minNumOfIFGsPerPacket;}
string PacketConfig::getDestAddress(){ return destAddress;}
string PacketConfig::getSourceAddress(){ return sourceAddress;}
int PacketConfig::getMaxPacketSize(){ return maxPacketSize;}
int PacketConfig::getAdded_IFGS(){ return added_IFGS;}    
int PacketConfig::getOranSCS() { return oranSCS;}
int PacketConfig::getOranMaxNrb() { return oranMaxNrb;}
int PacketConfig::getOranNrbPerPacket() { return oranNrbPerPacket;}
int PacketConfig::getethernetpayloadsize() { return ethernetpayloadsize;}
int PacketConfig::getoranpayloadsize() { return oranpayloadsize;}
string PacketConfig:: get_iq_filename() { return oranPayloadFile;}
     // Setters
void PacketConfig::setLineRateGbps(int lineRate){ 
    lineRateGbps = lineRate; }
void PacketConfig::setCaptureSizeMs(int captureSize){ 
    captureSizeMs = captureSize; }
void PacketConfig::setMinNumOfIFGsPerPacket(int minNumOfIFGs){ 
    minNumOfIFGsPerPacket = minNumOfIFGs; }
void PacketConfig::setDestAddress(string& destAddr){ 
    destAddress = destAddr; }
void PacketConfig::setSourceAddress(string& srcAddr){ 
    sourceAddress = srcAddr; }
void PacketConfig::setMaxPacketSize(int maxSize){ 
    maxPacketSize = maxSize; }
void PacketConfig::setAddedIFGS(int bytes){ 
    int rem = bytes%4;
    if (rem!=0) {
        added_IFGS = 4 -rem;
    }
    else {
        added_IFGS = 0;
    }
}
void PacketConfig :: setethernetpayloadsize() {
    ethernetpayloadsize=maxPacketSize-(destAddress.length())/2-(sourceAddress.length())/2-14;
}
void PacketConfig :: setoranpayloadsize() {
       // Constants for calculation
    int iqSamplesPerSubcarrier = 12;       // 12 IQ samples per subcarrier
    int subcarriersPerRb = 12;             // 12 subcarriers per resource block
    int ofdmSymbolsPerSlot = 14;           // 14 OFDM symbols per slot
    int bytesPerIqSample = 2;              // Each IQ sample is 2 bytes (16 bits)

    // Calculate bytes per RB
    int bytesPerRb = iqSamplesPerSubcarrier * subcarriersPerRb * ofdmSymbolsPerSlot * bytesPerIqSample;

    // Calculate ORAN payload size
     oranpayloadsize = oranNrbPerPacket * bytesPerRb;
    
    
}

   bool PacketConfig::loadConfig(string& configFilePath) {
    ifstream configFile(configFilePath);
    string line;
    
    if (configFile.is_open()) {
           
            // Parse key and value
          while (getline(configFile, line)) {
            istringstream iss(line);
            string key;
            string value;
            // Parse key and value
            if (std::getline(iss, key, '=') && std::getline(iss, value)) {
                 // Trim leading and trailing whitespaces
                key.erase(0, key.find_first_not_of(" \n\r\t"));
                key.erase(key.find_last_not_of(" \n\r\t") + 1);
                value.erase(0, value.find_first_not_of(" \n\r\t"));
                value.erase(value.find_last_not_of(" \n\r\t") + 1);
              // Remove '0x' prefix if present
                if (value.find("0x") == 0) {
                    value = value.substr(2);
                }
                // Set the appropriate field based on the key
                if (key == "Eth.LineRate") {
                    lineRateGbps = stoi(value);
                } else if (key == "Eth.CaptureSizeMs") {
                    captureSizeMs = stoi(value);
                } else if (key == "Eth.MinNumOfIFGsPerPacket") {
                    minNumOfIFGsPerPacket = stoi(value);
                } else if (key == "Eth.DestAddress") {
                    destAddress = value;
                } else if (key == "Eth.SourceAddress") {
                    sourceAddress = value;
                } else if (key == "Eth.MaxPacketSize") {
                    maxPacketSize = stoi(value);
                } else if (key == "Oran.SCS") {
                    oranSCS = stoi(value);
                } else if (key == "Oran.MaxNrb") {
                    oranMaxNrb = stoi(value);
                } else if (key == "Oran.NrbPerPacket") {
                    oranNrbPerPacket = stoi(value);
                } else if (key == "Oran.PayloadType") {
                    payloadTypeRandom = value;
                } else if (key == "Oran.Payload") {
                    
                   oranPayloadFile = value;
                   // Trim any characters after .txt
                  size_t pos = oranPayloadFile.find(".txt");
                  if (pos != string::npos) {
                    oranPayloadFile = oranPayloadFile.substr(0, pos + 4); // Include ".txt"
                  }
  
                }
                
                
            }
        }
        setethernetpayloadsize();
        setoranpayloadsize();
        configFile.close();
        return true;
    } 
    else {
        cerr << "Unable to open configuration file!" << endl;
        return false;
    }
}

int PacketConfig::calculate_no_of_packets() {
  // Convert line rate to bits per second
    long long lineRateBps = lineRateGbps * 1e9/8;
    // Convert capture time to seconds
    double captureTimeSeconds = captureSizeMs/1000.0;
    // Calculate total data size in bytes
    long long totalDataSizeBytes = lineRateBps * captureTimeSeconds;
    // Calculate the effective packet size including IFGs
    int effectivePacketSize = maxPacketSize + minNumOfIFGsPerPacket +added_IFGS; 
    // Calculate the number of packets needed
    int numberOfPackets = totalDataSizeBytes / effectivePacketSize;
    return numberOfPackets;
}


int PacketConfig::getecpriheadersize() {

return ecpriheadersize;

}