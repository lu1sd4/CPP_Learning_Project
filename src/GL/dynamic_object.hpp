#pragma once

#include <unordered_set>

namespace GL {

class DynamicObject
{
public:
    virtual ~DynamicObject() {}

    virtual bool update() = 0;
};

inline std::unordered_set<DynamicObject*> move_queue;

} // namespace GL
