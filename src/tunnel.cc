// ORIGINAL AUTHOR: Inigo Quilez (shadertoy.com/user/iq)

#include "softshader.hh"

static uint32_t shade(const ss::V2 coord)
{
    const auto p = (ss::res * -1.f + coord * 2.f) / ss::res.y;
    const auto a = ss::atan2(p.y, p.x);
    const auto r = ss::pow(ss::pow(p.x * p.x, 4.f) + ss::pow(p.y * p.y, 4.f), 1.f / 8.f);
    const auto uv = ss::V2(r == 0.f ? 1.f : (1.f / r) + 0.2f * ss::uptime(), a);
    const auto f = ss::cos(12.f * uv.x) * ss::cos(6.f * uv.y);
    const auto v = (ss::sin({ ss::V3 { 0.f, 0.5f, 1.f } + ss::PI * f }) * 0.5f + 0.5f) * r;
    return v.color(1.0f);
}

int main()
{
    ss::run(shade);
}
