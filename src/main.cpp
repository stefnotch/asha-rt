#include <GUI.hpp>
#include <iostream>
#include <bitset>

int main(){
    if (sizeof(ASHA::ReadOnlyProperties) != 18){
        throw std::length_error("Incompatible ROP byte size!");
    }
    GUI app = GUI("ASHA-RT", 0, 0);
    app.run();
    return 0;
}