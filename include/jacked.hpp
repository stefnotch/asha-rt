#include <jack/jack.h>

namespace JACKED {

class VirtualOutput {
public:
    VirtualOutput();
    auto getAudioFrames();
    ~VirtualOutput();
};

}