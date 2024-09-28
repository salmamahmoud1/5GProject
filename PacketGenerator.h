#include <vector>
#include <string>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <cstring>
#include "PacketConfig.h"


#pragma once
using namespace std;

class PacketGenerator {
private:
 PacketConfig& config; // Reference to the PacketConfig object
 double calculatePacketGenerationTime();
  size_t currentSampleIndex; // Index for looping through samples
  int eCPRISeqId=0; // eCPRI Sequence ID
  int frameid=0;
  int subframeid=0;
  int slotid=0;
  int symbolid=0;
  int maxpayloadsize=65535;
  int remainingoranbytes=config.getoranpayloadsize();
  int no_of_fragments;
  vector<std::string> iqSamples; // Vector to store IQ samples
  void loadIQSamples(const std::string& filename);
public:
  PacketGenerator(PacketConfig& conf);
 
  void generateStreamedPackets(string& outputFilePath);
  string generateCRC(const string& data);
  string generatepacket();
  string generateoranpayload();
  string generateECriheader();
  string generateoranheader();
  int getno_of_fragments();
  int availableoranpayloadsize();
};