#include <GUI.hpp>
#include <iostream>
#include <bitset>

int main(){
    if (sizeof(ASHA::ReadOnlyProperties) != 18){
        throw std::length_error("Incompatible ROP byte size!");
    }
    ASHA::Adapter adapter;
    GUI app = GUI("ASHA-RT", 0, 0);
    if (!adapter.isApapterFound()){
        app.setNoAdapterFound();
    }
    if (!adapter.isBluetoothOn()){
        app.setBluetoothOff();
    }
    app.assignAdapter(&adapter);
    app.run();
    return 0;
}