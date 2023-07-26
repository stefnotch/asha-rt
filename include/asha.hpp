#include <simpleble/SimpleBLE.h>

#include <chrono>
#include <string>
#include <vector>
#include <iomanip>
#include <ios>
#include <iostream>

namespace ASHA {

const std::string SERVICE_UUID = "0000FDF0";

struct ScanPeer;
struct DevicePair;

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

    static void scanConnect(SimpleBLE::Peripheral&);
};

class Peer {
private:
    ASHA::Adapter         *adapter;
    bool                   adapterSet = false;
    SimpleBLE::Peripheral  device;
    bool                   deviceSet  = false;
public:
    Peer();
    Peer(ASHA::Adapter*, SimpleBLE::Peripheral&);
    bool isConnected();
    bool isASHA();
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
