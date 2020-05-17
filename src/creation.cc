// ORIGINAL AUTHOR: Danilo Guanabara (shadertoy.com/user/Danguafer)

#include "softshader.hh"

static uint32_t shade(const ss::V2 coord)
{
    const auto per = coord / ss::res;
    auto c = ss::V3 {};
    auto l = 0.f;
    auto z = ss::uptime();
    for(int i = 0; i < 3; i++)
    {
        const auto p = (per - 0.5f) * ss::V2 { ss::res.x / ss::res.y, 1.f };
        l = ss::length(p);
        z += 0.07f;
        const auto uv = per + p / l * (ss::sin(z) + 1.f) * ss::abs(ss::sin(l * 9.f - z * 2.f));
        const auto cc = ss::length(ss::abs(ss::mod(uv, 1.f) - 0.5f));
        c[i] = (cc == 0.f) ? 1.f : (0.01f / cc);
    }
    const auto v = c / l;
    return v.color(ss::uptime());
}

int main()
{
    ss::run(shade);
}
