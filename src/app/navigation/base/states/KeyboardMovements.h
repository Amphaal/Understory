#pragma once

#include <Magnum/Platform/Sdl2Application.h>

#include <functional>

using Magnum::Platform::Sdl2Application;

struct AxisMomentum {
    float speed = 0.f;
    float timePassed = 0.f;
    Magnum::Vector2 currentDirection;
    Magnum::Vector2 momentumDirection;

    void goBy(std::function<Magnum::Vector2(Magnum::Float)> axisF, bool indirection) {
        auto val = indirection ? -1 : 1;
        currentDirection = axisF(val);
    }

    //
    void defineMomentum() {
        momentumDirection = currentDirection;
        currentDirection = Magnum::Vector2();
    }
};

struct KeyboardMovements {
 public:
     bool isVecticalAxis(Sdl2Application::KeyEvent::Key key) const {
        return key == Sdl2Application::KeyEvent::Key::Up || key == Sdl2Application::KeyEvent::Key::Down;
    }
    bool verticalPressed() const {
        return _movingUp || _movingDown;
    }


    bool isHorizontalAxis(Sdl2Application::KeyEvent::Key key) const {
        return key == Sdl2Application::KeyEvent::Key::Left || key == Sdl2Application::KeyEvent::Key::Right;
    }
    bool horizontalPressed() const {
        return _movingLeft || _movingRight;
    }

    bool keyPressed() const {
        return verticalPressed() || horizontalPressed();
    }

    // returns true if key corresponds to a movement instruction. If so, updates inner state accordingly
    bool ackMovement(const Sdl2Application::KeyEvent::Key &key, bool isPress) {
        //
        auto updateState = [&, isPress](bool& target, bool &indirection, bool indirectionVal) {
            target = isPress;
            indirection = isPress ? indirectionVal : !indirectionVal;
        };

        //
        switch (key) {
            case Sdl2Application::KeyEvent::Key::Left:
                updateState(_movingLeft, _latestH, false);
                break;
            case Sdl2Application::KeyEvent::Key::Right:
                updateState(_movingRight, _latestH, true);
            break;
            case Sdl2Application::KeyEvent::Key::Up:
                updateState(_movingUp, _latestV, true);
                break;
            case Sdl2Application::KeyEvent::Key::Down:
                updateState(_movingDown, _latestV, false);
            break;

            default:
                return false;
                break;
        }

        return true;
    }

    void updateMomentum(AxisMomentum &x, AxisMomentum &y, Sdl2Application::KeyEvent::Key key) const {
        if (isHorizontalAxis(key)) {
            if(horizontalPressed())
                x.goBy(Magnum::Vector2::xAxis, _latestH);
            else
                x.defineMomentum();
        } else if (isVecticalAxis(key)) {
            if (verticalPressed())
                y.goBy(Magnum::Vector2::yAxis, _latestV);
            else
                y.defineMomentum();
        }
    }

 private:
    bool _movingLeft = false;
    bool _movingRight = false;
    bool _movingUp = false;
    bool _movingDown = false;

    // indirection for latest vertical axis - false means Down, true means Up
    bool _latestV = false;

    // indirection for latest horizontal axis - false means Left, true means Right
    bool _latestH = false;
};
