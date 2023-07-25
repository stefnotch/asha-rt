#include <simpleble/SimpleBLE.h>

#include <chrono>
#include <string>
#include <vector>
#include <iomanip>
#include <ios>
#include <iostream>

namespace ASHA {

struct ScanPeer;
struct DevicePair;

const std::chrono::duration BLE_SCAN_PAUSE = std::chrono::seconds(10);

class Adapter {
private:
    bool                        foundAdapter = false;
    SimpleBLE::Adapter          hostAdapter;
    std::vector<ASHA::ScanPeer> lastScan     = {};
public:
    Adapter();
    bool isApapterFound();
    static bool isBluetoothOn();
    void startScan();
    std::vector<std::pair<
        std::string, SimpleBLE::BluetoothAddress>
    > queryNames(int);
    void updateScanResults();
    std::vector<ASHA::ScanPeer> getLastScan();
    std::vector<std::map<uint16_t, std::string>> queryDevices(int);
    bool isScanning();
};

class Peer {
private:
    ASHA::Adapter         *adapter;
    bool                   adapterSet = false;
    SimpleBLE::Peripheral *device;
    bool                   deviceSet  = false;
public:
    Peer();
    Peer(ASHA::Adapter*, SimpleBLE::Peripheral*);
    bool isPaired();
};

struct ScanPeer {
    std::string name;
    ASHA::Peer  peer;
};

struct DevicePair {
    ASHA::Peer left;
    ASHA::Peer right;
};

}
