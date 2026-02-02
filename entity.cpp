#include "entity.hpp"

auto Entity::nextId() -> int32_t
{
  static int32_t id = 0;
  return id++;
}

Entity::Entity() : id(nextId()) {}
