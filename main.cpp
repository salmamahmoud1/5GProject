#include "PacketConfig.h"
#include"PacketGenerator.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[]){
if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <config_file> <output_file>" << endl;
        return 1;
    }
    string configFilePath = argv[1];
    string outputFilePath = argv[2];

    PacketConfig config;
    
    if (config.loadConfig(configFilePath)) {
        PacketGenerator packetGenerator(config);
        packetGenerator.generateStreamedPackets(outputFilePath);
        
    }


return 0;


}