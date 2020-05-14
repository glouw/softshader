// ORIGINAL AUTHOR: Inigo Quilez (shadertoy.com/user/iq)

#include "softshader.hh"

namespace ss = softshader;

static uint32_t shade(const ss::V2 coord)
{
    const auto p = (ss::res * -1.f + coord * 2.f) / ss::res.y;
    const auto a = std::atan2(p.y, p.x);
    const auto r = std::pow(std::pow(p.x * p.x, 4.f) + std::pow(p.y * p.y, 4.f), 1.f / 8.f);
    const auto uv = ss::V2(1.f / r + 0.2f * ss::uptime(), a);
    const auto f = ss::trig::cos(12.f * uv.x) * ss::trig::cos(6.f * uv.y);
    const auto v = (ss::trig::sin({ ss::V3 { 0.f, 0.5f, 1.f } + ss::trig::PI * f }) * 0.5f + 0.5f) * r;
    return v.color(1.0f);
}

int main()
{
    ss::run(shade);
}
