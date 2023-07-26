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
    std::cout << "Updating scan results : asha.cpp 66" << std::endl;
    lastScan.clear();
    std::cout << "Fetching from adapter : asha.cpp 68" << std::endl;
    std::vector<SimpleBLE::Peripheral> results = hostAdapter.scan_get_results();
    std::cout << "Iterating devices : asha.cpp 70" << std::endl;
    for (SimpleBLE::Peripheral peer : results){
        std::cout << std::endl << "Next Device" << std::endl;
        if (peer.rssi() < -75){ 
            std::cout << "Device proximity too far : asha.cpp 74" << std::endl;
            continue; 
        }
        if (peer.identifier().length() == 0){
            if (peer.is_connected()){ continue; }
            std::cout << "ID length 0... connecting : asha.cpp 79" << std::endl;
            try
            {
                peer.connect();
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << ' : 86 \n';
            }
            std::cout << "Connected : asha.cpp 88" << std::endl;
            continue;
        } else if (peer.is_connected()){
            std::cout << "Disconnecting : asha.cpp 91" << std::endl;
            peer.unpair();
            peer.disconnect();
            std::cout << "Disconnected : asha.cpp 94" << std::endl;
        }
        std::cout << "Added device to results : asha.cpp 96" << std::endl;
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
            return true;
        }
    }
    device.unpair();
    device.disconnect();
    return false;
}

#endif
