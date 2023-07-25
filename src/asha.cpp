#include <vector>

#ifndef ASHA_H
#define ASHA_H

#include <asha.hpp>
#include <thread>

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
    if (!foundAdapter || !SimpleBLE::Adapter::bluetooth_enabled()){
        return returnData;
    }
    this->hostAdapter.scan_for(scanDuration * 1000);
    for (SimpleBLE::Peripheral device : this->hostAdapter.scan_get_results()){
        if (device.rssi() < -75){ continue; }
        if (device.identifier().length() > 0){ continue; }
        device.connect();
        while (!device.is_connected()){
            std::this_thread::sleep_for(
                std::chrono::milliseconds(1)
            );
        }
        while (device.identifier().length() == 0){
            std::this_thread::sleep_for(
                std::chrono::milliseconds(1)
            );
        }
        returnData.push_back(
            std::make_pair(
                device.identifier(),
                device.address()
            )
        );
        device.unpair();
    }
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
        // if (peer.identifier().length() > 0){ continue; }
        for (SimpleBLE::Service s : peer.services()){
            std::cout << s.uuid() << std::endl;
        }
        if (peer.identifier().length() == 0){ 
            peer.connect();
        }
        for (int count = 0; count < 100; ++count){
            std::this_thread::sleep_for(
                std::chrono::microseconds(50)
            );
        }
        if (peer.is_paired()){
            peer.disconnect();
        }
        if (peer.identifier().length() == 0){ continue; }
        lastScan.push_back(
            ASHA::ScanPeer{
                peer.identifier(),
                ASHA::Peer(this, &peer)
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

///////////////////////////////////////////////////////////////////////////////////
ASHA::Peer::Peer(){}

ASHA::Peer::Peer(ASHA::Adapter* hostAdapter, SimpleBLE::Peripheral* device){
    adapter = hostAdapter;
    this->device = device;
}

bool ASHA::Peer::isPaired(){
    if (deviceSet){
        return device->is_paired();
    }
    return false;
}

#endif
