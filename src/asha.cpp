#ifndef ASHA_H
#define ASHA_H

#include <asha.hpp>

ASHA::Adapter::Adapter(){
    std::vector<SimpleBLE::Adapter> adapters = SimpleBLE::Adapter::get_adapters();
    if (adapters.size() <= 0){
        this->foundAdapter = false;
    } else {
        this->foundAdapter = true;
        this->hostAdapter = adapters.back();
    }
}

bool ASHA::Adapter::isApapterFound(){
    return this->foundAdapter;
}

bool ASHA::Adapter::isBluetoothOn(){
    return SimpleBLE::Adapter::bluetooth_enabled();
}

std::vector<std::pair<std::string, SimpleBLE::BluetoothAddress>> 
ASHA::Adapter::queryNames(int scanDuration){
    std::vector<
        std::pair<std::string, SimpleBLE::BluetoothAddress>
    > returnData;
    // if (!foundAdapter || !SimpleBLE::Adapter::bluetooth_enabled()){
    //     return returnData;
    // }
    // this->hostAdapter.scan_for(scanDuration * 1000);
    // for (SimpleBLE::Peripheral device : this->hostAdapter.scan_get_results()){
    //     if (device.rssi() < -75){ continue; }
    //     if (device.identifier().length() > 0){ continue; }
    //     device.connect();
    //     while (!device.is_connected()){
    //         std::this_thread::sleep_for(
    //             std::chrono::milliseconds(1)
    //         );
    //     }
    //     while (device.identifier().length() == 0){
    //         std::this_thread::sleep_for(
    //             std::chrono::milliseconds(1)
    //         );
    //     }
    //     returnData.push_back(
    //         std::make_pair(
    //             device.identifier(),
    //             device.address()
    //         )
    //     );
    //     device.unpair();
    // }
    return returnData;
}

void ASHA::Adapter::startScan(){
    hostAdapter.scan_start();
}

void ASHA::Adapter::updateScanResults(){
    lastScan.clear();
    std::vector<SimpleBLE::Peripheral> results = hostAdapter.scan_get_results();
    for (SimpleBLE::Peripheral peer : results){
        if (peer.rssi() < -75){ continue; }
        if (peer.manufacturer_data().size() == 0){ continue; }
        if (peer.identifier().length() == 0){
            if (peer.is_connected()){ continue; }
            std::cout << "ID length 0... connecting : asha.cpp 79" << std::endl;
            try {
                peer.connect();
            } catch (std::exception connectError) {
            }
            continue;
        } else if (peer.is_connected()){
            try {
                peer.unpair();
                peer.disconnect();
            } catch(std::exception disconnectError){
            }
            std::cout << "Disconnected : asha.cpp 94" << std::endl;
        }
        lastScan.push_back(
            ASHA::ScanPeer{
                peer.identifier(),
                ASHA::Peer(this, peer)
            }
        );
    }
    hostAdapter.scan_stop();
}

std::vector<ASHA::ScanPeer> ASHA::Adapter::getLastScan(){
    return lastScan;
}

bool ASHA::Adapter::isScanning(){
    return hostAdapter.scan_is_active();
}

void ASHA::Adapter::scanConnect(SimpleBLE::Peripheral &peer){
}

///////////////////////////////////////////////////////////////////////////////////
ASHA::Peer::Peer(){}

ASHA::Peer::Peer(ASHA::Adapter* hostAdapter, SimpleBLE::Peripheral &device){
    adapter = hostAdapter;
    this->device = device;
}

bool ASHA::Peer::isConnected(){
    if (deviceSet){
        return device.is_connected();
    }
    return false;
}

bool ASHA::Peer::isASHA(){
    if (!isConnected()){
        device.connect();
    }
    for (SimpleBLE::Service serv : device.services()){
        if (serv.uuid().substr(0, 8) == ASHA::SERVICE_UUID){
            ASHA_UUID = serv.uuid();
            return true;
        }
    }
    device.unpair();
    device.disconnect();
    return false;
}

bool ASHA::Peer::getReadOnlyProperties(){
    if (!isConnected()){
        device.connect();
    }
    if (!isConnected() || (ASHA_UUID.length() == 0)){
        return false;
    }
    std::string readData = device.read(ASHA_UUID, ROP_UUID);
    if (sizeof(readData.c_str()) == sizeof(ASHA::ReadOnlyProperties)){
        memcpy(&properties, readData.c_str(), 18);
        return true;
    }
    return false;
}

ASHA::Side ASHA::Peer::getSide(){
    if (!properties->VERSION){
        throw std::runtime_error(
            "Tried reading property before getting them"
        );
    }
    if (properties->DC.side & Side::RIGHT){
        return Side::RIGHT;
    }
    return Side::LEFT;
}

void ASHA::Peer::unset(){
    deviceSet = false;
}


#endif
