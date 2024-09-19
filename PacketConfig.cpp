#include "PacketConfig.h"
#include <fstream>
#include <iostream>
#include <sstream> 
#include <cctype>  
using namespace std;



// Getters
int PacketConfig::getLineRateGbps(){ 
    return lineRateGbps; }
int PacketConfig::getCaptureSizeMs(){ 
    return captureSizeMs; }
int PacketConfig::getMinNumOfIFGsPerPacket(){ 
    return minNumOfIFGsPerPacket; }
string PacketConfig::getDestAddress(){ 
    return destAddress; }
string PacketConfig::getSourceAddress(){ 
    return sourceAddress; }
int PacketConfig::getMaxPacketSize(){
     return maxPacketSize; }
int PacketConfig::getBurstSize(){
     return burstSize; }
int PacketConfig::getBurstPeriodicityUs(){
     return burstPeriodicityUs; }
int PacketConfig::getAdded_IFGS(){
     return added_IFGS; }    
int PacketConfig::getno_of_bursts(){
  
    int totalbits = (getMaxPacketSize() + getMinNumOfIFGsPerPacket() + getAdded_IFGS()) * 8;
    int totalburstsize = totalbits * getBurstSize(); // Burst size in bits
    double timeperburst = (double)totalburstsize / (getLineRateGbps() * 1e9) + getBurstPeriodicityUs() / 1e6;
    // Total number of bursts
   return getCaptureSizeMs() / (timeperburst * 1000); 
   }      
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
void PacketConfig::setBurstSize(int BurstSize){ 
    burstSize = BurstSize; }
void PacketConfig::setBurstPeriodicityUs(int burstPeriodicity){ 
    burstPeriodicityUs = burstPeriodicity; }
void PacketConfig::setAddedIFGS(){ 

    int bytes=getMaxPacketSize()+getMinNumOfIFGsPerPacket();
    int rem = bytes%4;
    if (rem!=0) {
        added_IFGS = 4 -rem;
    }
    else {
        added_IFGS = 0;
    }
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
                // Trim any leading or trailing whitespace from key and value
                key.erase(key.find_last_not_of(" \n\r\t") + 1);
                value.erase(0, value.find_first_not_of(" \n\r\t"));
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
                } else if (key == "Eth.BurstSize") {
                    burstSize = stoi(value);
                } else if (key == "Eth.BurstPeriodicity_us") {
                    burstPeriodicityUs = stoi(value);
                }
            }
        }
        payloadsize=maxPacketSize-(destAddress.length())/2-(sourceAddress.length())/2-14;
        setAddedIFGS();
        configFile.close();
        return true;
    } 
    else {
        cerr << "Unable to open configuration file!" << endl;
        return false;
    }
}