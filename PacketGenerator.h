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
 double calculateStreamingDuration();

public:
  PacketGenerator(PacketConfig& conf);
 
  void generateStreamedPackets(string& outputFilePath);
  string generateCRC(const string& data);
  string generatepacket();

};