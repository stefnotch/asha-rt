#ifndef ASHA_H
#define ASHA_H

#include <asha.hpp>
#include <thread>

ASHA::Adapter::Adapter(){
    for (SimpleBLE::Adapter adapter : SimpleBLE::Adapter::get_adapters()){
        if (adapter.initialized()){
            hostAdapter = adapter;
            foundAdapter = true;
            return;
        }
    }
}

bool ASHA::Adapter::isApapterFound(){
    return this->foundAdapter;
}

bool ASHA::Adapter::isBluetoothOn(){
    return SimpleBLE::Adapter::bluetooth_enabled();
}

void ASHA::Adapter::startScan(){
    hostAdapter.scan_start();
}

void ASHA::Adapter::updateScanResults(){
    lastScan.clear();
    std::vector<SimpleBLE::Peripheral> results = hostAdapter.scan_get_results();
    hostAdapter.scan_stop();
    for (SimpleBLE::Peripheral peer : results){
        if (peer.rssi() < -75){ continue; }
        if (!peer.is_connectable()){ continue; }
        if (peer.identifier().length() == 0){ continue; }
        if (peer.manufacturer_data().size() == 0){ continue; }

        // if (peer.identifier().length() == 0){
        //     peer.connect();
        //     peer.disconnect();
        //     peer.unpair();
        //     continue;
        // }

        lastScan.push_back(
            ASHA::ScanPeer{
                peer.identifier(),
                new ASHA::Peer(peer)
            }
        );
    }
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

ASHA::Peer::Peer(SimpleBLE::Peripheral &device){
    this->device = device;
    deviceSet = true;
}

bool ASHA::Peer::isConnected(){
    if (!deviceSet){ return false; }
    return device.is_connected();
}

bool ASHA::Peer::isPaired(){
    if (!deviceSet){ return false; }
    return device.is_paired();
}

bool ASHA::Peer::isASHA(){
    while (!isConnected()){
        try {
            device.connect();
        } catch (const std::exception e){
            std::cout << e.what() << std::endl;
        }
    }
    std::cout << "Device services:" << std::endl;
    for (SimpleBLE::Service serv : device.services()){
        std::cout << "\t" << serv.uuid() << std::endl;
        if (serv.uuid().substr(0, 8) == ASHA::SERVICE_UUID){
            ASHA_UUID = serv.uuid();
            return true;
        }
    }
    std::cout << std::endl;
    if (isPaired()){
        try {
            device.unpair();
        } catch(std::exception disconnectError){
        }
    }
    if (isConnected()){
        try {
            device.disconnect();
        } catch(std::exception disconnectError){
        }
    }
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
