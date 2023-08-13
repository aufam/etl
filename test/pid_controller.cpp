#include "etl/pid_controller.h"
#include "etl/placeholder.h"
#include "gtest/gtest.h"
#include "etl/keywords.h"

using namespace Project::etl;

TEST(PID, PID) {
    val kp = 0.1;
    val ki = 0.2;
    val kd = 0.3;
    val kdf = 0.4;

    var controller = pid(
        placeholder::retval = kp,
        placeholder::retval = ki,
        placeholder::retval = kd,
        placeholder::retval = kdf
    );
}