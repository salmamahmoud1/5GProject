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


string PacketGenerator:: generateCRC(const string& data) {
     unsigned long crc = crc32(0L, reinterpret_cast<const Bytef*>(data.data()), data.size());
       // Convert to hexadecimal string
    std::stringstream ss;
    ss << std::hex << std::setw(8) << std::setfill('0') << crc;

    return ss.str();
}
string PacketGenerator:: generatepacket() {
   
    loadIQSamples(config.get_iq_filename());
    string packet =config.PreambleSFD+config.getDestAddress()+config.getSourceAddress()+config.ethertype;
    packet +=generateECriheader();
    packet+=generateoranheader();
    cout<<"no of packets that can be sent "<<config.calculate_no_of_packets()<<endl;
    cout<<"no of fragments "<<getno_of_fragments()<<endl;
    cout<<"available bytes for oran payload "<<availableoranpayloadsize()<<endl;
    packet+=generateoranpayload();
    string crc = generateCRC(packet);
    packet +=crc;
    int bytes= packet.length()/2;
    cout<<"total bytes "<<bytes<<endl;
    config.setAddedIFGS(bytes);

    return packet;
}

void PacketGenerator::generateStreamedPackets(string& outputFilePath) {

    ofstream outFile(outputFilePath);
    if (!outFile.is_open()) {
        cerr << "Unable to open output file!" << endl;
        return;
    }
   std:: cout<<"added IFGS "<<config.getAdded_IFGS()<<endl;
    int count=0;  
    // Generate packets and write them to the output file
//  for (int burst = 0; burst < config.calculate_no_of_packets(); burst++) {
    
            string packet = generatepacket();
      
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
      // }
     //   }

    // Close the output file
    outFile.close();
    
    std::cout << "Packets written to " << outputFilePath << endl;

}
void PacketGenerator::loadIQSamples(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Unable to open IQ samples file!" << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        iqSamples.push_back(line);
    }

    file.close();
}

string PacketGenerator:: generateoranpayload()
{
    if (iqSamples.empty()) {
        std::cerr << "Error: IQ samples list is empty!" << std::endl;
        return "";
    }
    string oranPayload;
    size_t iqSamplesPerRB = 12; // 12 IQ samples per resource block
    size_t requiredSampleCount = config.getOranNrbPerPacket() * iqSamplesPerRB; // Total number of IQ samples required for the payload

    size_t sampleCount = 0;
    
    // Continue appending samples until we meet the required sample count
    while (sampleCount < requiredSampleCount) {
        if (currentSampleIndex >= iqSamples.size()) {
            currentSampleIndex = 0; // Reset to the beginning if we reach the end of the IQ samples
        }

        // Convert the hexadecimal string in iqSamples[currentSampleIndex] to int16_t
        int16_t sample;
        std::stringstream hexStream(iqSamples[currentSampleIndex]);
        hexStream >> std::hex >> sample;

        // Format the sample as a 4-digit hexadecimal string
        std::stringstream ss;
        ss << std::hex << std::setw(4) << std::setfill('0') << (sample & 0xFFFF);
        oranPayload += ss.str();

        currentSampleIndex++;
        sampleCount++;
    }
    
    return oranPayload;

}
// Generate eCPRI payload including ORAN data
std::string PacketGenerator::generateECriheader() {
    std::string ecpri;
    std::stringstream ss;
    // First byte is 0x00 (dummy byte)
    ecpri += "00";

    // Message Type
    ecpri += "00"; // User Plane

    // ORAN Payload
    if(remainingoranbytes>availableoranpayloadsize())

     ss << std::hex <<setw(4)<<setfill('0')<< availableoranpayloadsize();
    else
     ss << std::hex <<setw(4)<<setfill('0')<<  remainingoranbytes;

    ecpri += ss.str();
    cout<<"oran payload size "<<ss.str()<<endl;
    // PC_RTC
    ecpri += "0000";

    // Sequence ID

ss << std::hex << std::setw(4) << std::setfill('0') << eCPRISeqId++;
ecpri += ss.str();
    if (eCPRISeqId > 255) eCPRISeqId = 0; // Reset after 255
cout<<"ecpri id "<<eCPRISeqId<<endl;
    return ecpri;
}



string PacketGenerator:: generateoranheader() {

    string oran;
    std::stringstream ss;
    oran += "00"; // First byte
    ss << std::hex << std::setw(2) << std::setfill('0') << frameid;
    oran += ss.str();
    ss << std::hex << subframeid;
    oran += ss.str();
    ss << std::hex << slotid;
    oran += ss.str();
    ss << std::hex<< symbolid;
    oran += ss.str();
return oran;
}
int PacketGenerator ::getno_of_fragments() {
  int fragments=0;
  int oransize=config.getoranpayloadsize();
   while (oransize>availableoranpayloadsize()) {
   fragments++;
   oransize-=availableoranpayloadsize();
   }

    return fragments;

}
int PacketGenerator::availableoranpayloadsize() {
int bytes=config.getethernetpayloadsize()-config.getecpriheadersize()-8;
if (config.getoranpayloadsize()>bytes)
    return bytes;
    else
    return config.getoranpayloadsize();
}