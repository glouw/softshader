// ORIGINAL AUTHOR: Danilo Guanabara (shadertoy.com/user/Danguafer)

#include "softshader.hh"

namespace ss = softshader;

static uint32_t shade(const ss::V2 coord)
{
    auto c = ss::V3{};
    auto l = float {};
    auto z = ss::uptime();
    for(int i = 0; i < 3; i++) {
        auto p = coord / ss::res;
        auto uv = p;
        p -= 0.5f;
        p.x *= ss::res.x / ss::res.y;
        z += 0.07f;
        l = ss::trig::length(p);
        uv += p / l * (std::sin(z) + 1.f) * ss::trig::abs(std::sin(l * 9.f - z * 2.f));
        const auto a = ss::trig::mod(uv, 1.f) - 0.5f;
        const auto b = ss::trig::abs(a);
        const auto cc = ss::trig::length(b);
        c[i] = cc == 0.f ? 1.0f : .01f / cc;
    }
    const auto v = c / l;
    return v.color(ss::uptime());
}

int main()
{
    ss::run(shade);
}
