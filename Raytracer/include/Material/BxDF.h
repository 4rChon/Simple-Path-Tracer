#pragma once

#include "IMaterial.h"

namespace Raytracer {
class BxDF : public IMaterial {
  private:
    const unsigned int type_;

  public:
    enum Type {
        Reflect = 1 << 0,
        Transmit = 1 << 1,
        Diffuse = 1 << 2,
        Glossy = 1 << 3,
        Specular = 1 << 4,
        All = Reflect | Transmit | Diffuse | Glossy | Specular
    };

    BxDF(const unsigned int type);

    bool has_type(const int type) const override
    {
        return (type_ & type) != 0;
    }
};
} // namespace Raytracer