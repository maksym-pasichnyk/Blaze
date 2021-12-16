#include "Time.hpp"

static float m_DeltaTime = 0.0f;

auto Time::getDeltaTime() -> float {
    return m_DeltaTime;
}

void Time::_setDeltaTime(float deltaTime) {
    m_DeltaTime = deltaTime;
}
