#ifndef DI8_GENERICCONTROLLER_STATE_H
#define DI8_GENERICCONTROLLER_STATE_H

#include "ps3controllerstate.h"
#include "ps4controllerstate.h"
#include "xboxcontrollerstate.h"

namespace DI8 {

struct GenericControllerState {
    GenericControllerState(const Ps3ControllerState& s)
        : leftAnalogLeft_{s.leftAnalogLeft()}, leftAnalogRight_{s.leftAnalogRight()}, leftAnalogUp_{s.leftAnalogUp()}, leftAnalogDown_{s.leftAnalogDown()},
          rightAnalogLeft_{s.rightAnalogLeft()}, rightAnalogRight_{s.rightAnalogRight()}, rightAnalogUp_{s.rightAnalogUp()}, rightAnalogDown_{s.rightAnalogDown()},
          L2_(s.L2() /*TODO*/), R2_(s.R2() /*TODO*/), start_{s.start()}, select_{s.select()}, up_{s.up()}, left_{s.left()}, down_{s.down()}, right_{s.right()},
          triangle_{s.triangle()}, square_{s.square()}, cross_{s.cross()}, circle_{s.circle()}, L1_{s.L1()}, R1_{s.R1()}, L3_{s.L3()}, R3_{s.R3()}, home_{s.ps3()}, touch_{false} {}

    GenericControllerState(const Ps4ControllerState& s)
        : leftAnalogLeft_{s.leftAnalogLeft()}, leftAnalogRight_{s.leftAnalogRight()}, leftAnalogUp_{s.leftAnalogUp()}, leftAnalogDown_{s.leftAnalogDown()},
          rightAnalogLeft_{s.rightAnalogLeft()}, rightAnalogRight_{s.rightAnalogRight()}, rightAnalogUp_{s.rightAnalogUp()}, rightAnalogDown_{s.rightAnalogDown()},
          L2_(s.L2()), R2_(s.R2()), start_{s.start()}, select_{s.select()}, up_{s.up()}, left_{s.left()}, down_{s.down()}, right_{s.right()},
          triangle_{s.triangle()}, square_{s.square()}, cross_{s.cross()}, circle_{s.circle()}, L1_{s.L1()}, R1_{s.R1()}, L3_{s.L3()}, R3_{s.R3()}, home_{s.home()}, touch_{s.touch()} {}

    GenericControllerState(const XboxControllerState& s)
        : leftAnalogLeft_{s.leftAnalogLeft()}, leftAnalogRight_{s.leftAnalogRight()}, leftAnalogUp_{s.leftAnalogUp()}, leftAnalogDown_{s.leftAnalogDown()},
          rightAnalogLeft_{s.rightAnalogLeft()}, rightAnalogRight_{s.rightAnalogRight()}, rightAnalogUp_{s.rightAnalogUp()}, rightAnalogDown_{s.rightAnalogDown()},
          L2_(s.LT()), R2_(s.RT()), start_{s.start()}, select_{s.select()}, up_{s.up()}, left_{s.left()}, down_{s.down()}, right_{s.right()},
          triangle_{s.Y()}, square_{s.X()}, cross_{s.A()}, circle_{s.B()}, L1_{s.LB()}, R1_{s.RB()}, L3_{s.L3()}, R3_{s.R3()}, home_{false}, touch_{false} {}

    inline LONG leftAnalogLeft() const {
        return leftAnalogLeft_;
    }

    inline LONG leftAnalogRight() const {
        return leftAnalogRight_;
    }

    inline LONG leftAnalogUp() const {
        return leftAnalogUp_;
    }

    inline LONG leftAnalogDown() const {
        return leftAnalogDown_;
    }

    inline LONG rightAnalogLeft() const {
        return rightAnalogLeft_;
    }

    inline LONG rightAnalogRight() const {
        return rightAnalogRight_;
    }

    inline LONG rightAnalogUp() const {
        return rightAnalogUp_;
    }

    inline LONG rightAnalogDown() const {
        return rightAnalogDown_;
    }

    inline bool L2() const {
        return L2_;
    }

    inline bool R2() const {
        return R2_;
    }

    inline bool start() const {
        return start_;
    }

    inline bool select() const {
        return select_;
    }

    inline bool up() const {
        return up_;
    }

    inline bool left() const {
        return left_;
    }

    inline bool down() const {
        return down_;
    }

    inline bool right() const {
        return right_;
    }

    inline bool triangle() const {
        return triangle_;
    }

    inline bool square() const {
        return square_;
    }

    inline bool cross() const {
        return cross_;
    }

    inline bool circle() const {
        return circle_;
    }

    inline bool L1() const {
        return L1_;
    }

    inline bool R1() const {
        return R1_;
    }

    inline bool L3() const {
        return L3_;
    }

    inline bool R3() const {
        return R3_;
    }

    inline bool home() const {
        return home_;
    }

    inline bool touch() const {
        return touch_;
    }

private:
    LONG leftAnalogLeft_;
    LONG leftAnalogRight_;
    LONG leftAnalogUp_;
    LONG leftAnalogDown_;
    LONG rightAnalogLeft_;
    LONG rightAnalogRight_;
    LONG rightAnalogUp_;
    LONG rightAnalogDown_;
    bool L2_;
    bool R2_;
    bool start_;
    bool select_;
    bool up_;
    bool left_;
    bool down_;
    bool right_;
    bool triangle_;
    bool square_;
    bool cross_;
    bool circle_;
    bool L1_;
    bool R1_;
    bool L3_;
    bool R3_;
    bool home_;
    bool touch_;
};

}

#endif // DI8_GENERICCONTROLLER_STATE_H