#include "Planet.h"
#include <iostream>

Planet::Planet()
: _position{0.0f, 0.0f, 0.0f},
  _radius{100.0f},
  _gravityFactor{9.8f},
  _texture{0u}
{
}
