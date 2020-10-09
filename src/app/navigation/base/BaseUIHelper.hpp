#pragma once

#include <Magnum/Timeline.h>
#include <Magnum/Math/Matrix3.h>

#include <vector>

class BaseUIHelper {
 public:
    BaseUIHelper(Magnum::Timeline* timeline, Magnum::Matrix3* mainMatrix) : _timeline(timeline), _mainMatrix(mainMatrix) {}

    virtual void stopAnim() = 0;
    virtual void advance() = 0;

    void setExcludedWhenPlaying(std::initializer_list<BaseUIHelper*> list) {
        _excluded = list;
    }

 protected:
    Magnum::Timeline* _timeline = nullptr;
    Magnum::Matrix3* _mainMatrix = nullptr;

    // must be overriden and reused
    virtual void _startAnim() {
        for(auto &excluded : _excluded) excluded->stopAnim();
    }

 private:
    std::vector<BaseUIHelper*> _excluded;
};
