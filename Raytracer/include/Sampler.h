#pragma once

#include <glm/vec2.hpp>
#include <random>
#include <vector>

namespace Raytracer {
struct Sample {
    glm::vec2 jitter;
};

class Sampler {
  private:
    std::random_device rd_;
    std::mt19937 gen_;
    std::vector<std::uniform_real_distribution<>> stratified_dist_;
    std::uniform_real_distribution<> uniform_dist_real_;
    std::uniform_int_distribution<> uniform_dist_int_;

    float spp_root_;

    // Used to keep track of sample index in stratified sample
    unsigned int sample_x_ = 0;
    unsigned int sample_y_ = 0;

  public:
    const unsigned int spp;
    Sampler(const unsigned int spp);
    ~Sampler();
    float get_weight() const
    {
        return 1.f / spp;
    }
    Sample next_stratified_sample();
    Sample next_uniform_sample();
    std::vector<float> next_uniform_real(const unsigned int count);
    std::vector<int> next_uniform_int(const unsigned int count);
};
} // namespace Raytracer