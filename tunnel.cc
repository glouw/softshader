// ORIGINAL AUTHOR: Inigo Quilez (shadertoy.com/user/iq)

#include "softshader.hh"

using namespace softshader;

static uint32_t shade(const int x, const int y, const int xres, const int yres)
{
    const auto res = V2{float(xres), float(yres)};
    const auto coord = V2{float(x), float(y)};
    const auto p = (res * -1.f + coord * 2.f) / res.y;
    const auto a = std::atan2(p.y, p.x);
    const auto r = std::pow(std::pow(p.x * p.x, 4.f) + std::pow(p.y * p.y, 4.f), 1.f / 8.f);
    const auto time = SDL_GetTicks() * 0.001f;
    const auto uv = V2(1.f / r + 0.2f * time, a);
    const auto f = std::cos(12.f * uv.x) * std::cos(6.f * uv.y);
    const auto v = (trig::sin({V3{0.f, 0.5f, 1.f} + trig::PI * f}) * 0.5f + 0.5f) * r;
    return v.color(1.0f);
}

int main()
{
    run(shade);
}
