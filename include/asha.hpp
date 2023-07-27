#include <simpleble/SimpleBLE.h>

#include <chrono>
#include <cstring>
#include <string>
#include <vector>
#include <iomanip>
#include <ios>
#include <iostream>

typedef uint16_t RenderDelay;
typedef uint16_t CodecIDs;

namespace ASHA {

const std::string SERVICE_UUID = "0000FDF0";
const std::string ROP_UUID    = "6333651e-c481-4a3e-9169-7c902aad37bb";

enum Side : unsigned char {LEFT, RIGHT};
enum Mode : unsigned char {MONO,    BI};

struct ScanPeer;
struct DevicePair;
struct DeviceCapabilities;
struct HiSyncID;
struct FeatureMap;
struct ReadOnlyProperties;

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
    ASHA::Adapter             *adapter;
    bool                       adapterSet = false;
    SimpleBLE::Peripheral      device;
    bool                       deviceSet  = false;
    std::string                ASHA_UUID    = "";
    struct ReadOnlyProperties *properties;
public:
    Peer();
    Peer(Adapter*, SimpleBLE::Peripheral&);
    bool isConnected();
    bool isASHA();
    void getReadOnlyProperties();
};

struct ScanPeer {
    std::string name;
    ASHA::Peer  peer;
};

struct DevicePair {
    ASHA::Peer left;
    ASHA::Peer right;
};

struct DeviceCapabilities {
    Side          side:1;
    Mode          mode:1;
    unsigned char rsvd:6;
};

struct HiSyncID {
    uint16_t companyID;
    char     pairID[6];
};

struct FeatureMap {
    unsigned char leCoC:1;
    unsigned char rsvd:7;
};

struct ReadOnlyProperties {
    uint8_t            VERSION;
    DeviceCapabilities DC;
    HiSyncID           HSID;
    FeatureMap         FM;
    RenderDelay        RD;
    uint16_t           rsvd;
    CodecIDs           CIDs;
};

}
