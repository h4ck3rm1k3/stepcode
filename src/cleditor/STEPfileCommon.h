#ifndef STEPFILECOMMON_H
#define STEPFILECOMMON_H

#include <istream>

/**
 * abstract base class of STEPfile and lazyStepFile
 */
class STEPfileCommon {
protected:
    std::istream in;
    Registry* registry;
    STEPfileCommon( Registry & r ): registry(r) {
    }

public:
    //nothing public?
};

#endif //STEPFILECOMMON_H
