#include "PacketGenerator.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <zlib.h>



// Constructor
PacketGenerator::PacketGenerator(PacketConfig& conf) : config(conf) {}


double PacketGenerator::calculatePacketGenerationTime() {
   
    int bytes=config.getMaxPacketSize()+config.getMinNumOfIFGsPerPacket()+config.getAdded_IFGS();
    return (bytes * 8.0) / (config.getLineRateGbps() * 1e9); // time in seconds
}

double PacketGenerator::calculateStreamingDuration() {
    double packetGenerationTime = calculatePacketGenerationTime();
    // Total time for all packets in one burst
    double totalTimeForAllPacketsInBurst = config.getBurstSize() * packetGenerationTime;
     // Calculate the number of bursts based on the capture size and burst periodicity
    int totalBursts = static_cast<int>((config.getCaptureSizeMs() / 1000.0) * (1e9 / config.getBurstPeriodicityUs()));
     // Calculate total periodicity time
    double totalPeriodicityTime = totalBursts * (config.getBurstPeriodicityUs() / 1e6); // convert microseconds to seconds
    // Final streaming duration
    return totalTimeForAllPacketsInBurst + totalPeriodicityTime;

}
string PacketGenerator:: generateCRC(const string& data) {
     unsigned long crc = crc32(0L, reinterpret_cast<const Bytef*>(data.data()), data.size());
       // Convert to hexadecimal string
    std::stringstream ss;
    ss << std::hex << std::setw(8) << std::setfill('0') << crc;

    return ss.str();
}
string PacketGenerator:: generatepacket() {
  
    string packet =config.PreambleSFD+config.getDestAddress()+config.getSourceAddress()+config.ethertype;
    for (int i=0;i<config.payloadsize;i++) {
        packet +="00";
    } 
    string crc = generateCRC(packet);
    packet +=crc;

    return packet;
}

void PacketGenerator::generateStreamedPackets(string& outputFilePath) {

    ofstream outFile(outputFilePath);
    if (!outFile.is_open()) {
        cerr << "Unable to open output file!" << endl;
        return;
    }
    ofstream parsedFile("parsed_packets.txt"); // New file to dump the parsed packets 
    if (!outFile.is_open() || !parsedFile.is_open()) {
        cerr << "Unable to open output or JSON file!" << endl;
        return;
    }


    cout<<"no of bursts "<<config.getno_of_bursts()<<endl;
    cout<<"added IFGS "<<config.getAdded_IFGS()<<endl;
    int count=0;  
    // Generate packets and write them to the output file
    for (int burst = 0; burst < config.getno_of_bursts(); burst++) {
        parsedFile<<"burst ["<<burst<<"]"<<endl;
        for (int i = 0; i < config.getBurstSize(); i++) {
            string packet = generatepacket();
            string preamble = packet.substr(0, 14);  // Preamble (7 bytes = 14 hex chars)
            string sop = packet.substr(14, 2);       // SoP (1 byte = 2 hex chars)
            string destAddress = packet.substr(16, 12); // Dest Address (6 bytes = 12 hex chars)
            string srcAddress = packet.substr(28, 12); // Src Address (6 bytes = 12 hex chars)
            string etherType = packet.substr(40, 4); // EtherType (2 bytes = 4 hex chars)
            string payload = packet.substr(44, packet.length() - 56); // Payload (variable length)
            string crc = packet.substr(packet.length() - 8, 8); // CRC (4 bytes = 8 hex chars)
            parsedFile<<"Packet ["<<i<<"]"<<endl;
            parsedFile<<"Preamble : 0x"<<preamble<<endl;
            parsedFile<<"sop : 0x"<<sop<<endl;
            parsedFile<<"DestinationAddress : 0x"<<destAddress<<endl;
            parsedFile<<"SourceAddress : 0x"<<srcAddress<<endl;
            parsedFile<<"EtherType : 0x"<<etherType<<endl;
            parsedFile<<"Payload : 0x"<<payload<<endl;
            parsedFile<<"CRC : 0x"<<crc<<endl;
            if (count ==8) {
                outFile<<endl;
                count=0;
            }
            for (size_t i = 0; i < packet.length(); i ++) {
                outFile << packet.substr(i, 1); 
                count++;
                if (count ==8) {
                    outFile<<endl;
                    count=0;
                }
            }

            for(int i=0;i<config.getMinNumOfIFGsPerPacket()+config.getAdded_IFGS();i++) {
                if (count ==8) {
                    outFile<<endl;
                    count=0;
                }
                outFile << config.IFG;
                count+=2;
            
            }
            int no_of_ifgs=config.getBurstPeriodicityUs()*config.getLineRateGbps()*1000/8;
            //Add inter-burst gaps (IFG or padding as necessary)
             for (int i=0;i<no_of_ifgs;i++) {
                if (count ==8) {
                    outFile<<endl;
                    count=0;
                }
                outFile << config.IFG;
                count+=2;
            }
        }
    }
    // Close the output file
    outFile.close();
    parsedFile.close();
    std::cout << "Packets written to " << outputFilePath << endl;

}

