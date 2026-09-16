#include <algorithm>
#include <cmath>
#include <glm/vec2.hpp>
#include <random>
#include <vector>
#include "Sampler.h"

namespace Raytracer {
Sampler::Sampler(const unsigned int spp) : spp(spp)
{
    gen_ = std::mt19937(rd_());
    spp_root_ = (float)sqrt(spp);
    for (auto i = 0; i < spp_root_; i++) {
        float min_jitter = (i / spp_root_);
        float max_jitter = (i / spp_root_) + spp_root_ / spp;
        std::uniform_real_distribution<> dist =
            std::uniform_real_distribution<>((double)std::min(min_jitter, max_jitter),
                                             (double)std::max(min_jitter, max_jitter));
        stratified_dist_.push_back(dist);
    }
}
Sampler::~Sampler() {}

Sample Sampler::next_stratified_sample()
{
    const auto jitter = glm::vec2(stratified_dist_[sample_x_ % (int)spp_root_](gen_),
                                  stratified_dist_[sample_y_ % (int)spp_root_](gen_));

    Sample sample{jitter};

    if (++sample_x_ % (int)spp_root_ == 0) {
        ++sample_y_;
    }

    return sample;
}

Sample Sampler::next_uniform_sample()
{
    const auto jitter = glm::vec2(uniform_dist_real_(gen_), uniform_dist_real_(gen_));
    return Sample{jitter};
}

std::vector<float> Sampler::next_uniform_real(const unsigned int count)
{
    std::vector<float> samples(count);
    for (unsigned int i = 0; i < count; i++) {
        samples[i] = (float)uniform_dist_real_(gen_);
    }

    return samples;
}

std::vector<int> Sampler::next_uniform_int(const unsigned int count)
{
    std::vector<int> samples(count);
    for (unsigned int i = 0; i < count; i++) {
        samples[i] = uniform_dist_int_(gen_);
    }

    return samples;
}
} // namespace Raytracer