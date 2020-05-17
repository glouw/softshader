// ORIGINAL AUTHOR: Alex Alekseev (shadertoy.com/user/TDM)

#include "softshader.hh"

namespace
{
    const auto NUM_STEPS = 8;
    const auto EPSILON = 1e-3f;
    const auto EPSILON_NRM = 0.1f / ss::res.x;
    const auto ITER_GEOMETRY = 3;
    const auto ITER_FRAGMENT = 5;
    const auto SEA_HEIGHT = 0.6f;
    const auto SEA_CHOPPY = 4.f;
    const auto SEA_SPEED = 0.8f;
    const auto SEA_FREQ = 0.16f;
    const auto SEA_BASE = ss::V3 { 0.0f, 0.09f, 0.18f };
    const auto SEA_WATER_COLOR = ss::V3 { 0.8f, 0.9f, 0.6f } * 0.6f;
    const auto OCTAVE = ss::M2 { 1.6f, 1.2f, -1.2f, 1.6f };

    inline float sea_time()
    {
        return ss::uptime() * SEA_SPEED;
    }

    inline ss::M3 from_euler(ss::V3 ang)
    {
        const auto a1 = ss::V2 { ss::sin(ang.x), ss::cos(ang.x) };
        const auto a2 = ss::V2 { ss::sin(ang.y), ss::cos(ang.y) };
        const auto a3 = ss::V2 { ss::sin(ang.z), ss::cos(ang.z) };
        // clang-format off
    return ss::M3 {
        ss::V3 {  a1.y * a3.y + a1.x * a2.x * a3.x, a1.y * a2.x * a3.x + a3.y * a1.x, -a2.y * a3.x },
        ss::V3 { -a2.y * a1.x,                      a1.y * a2.y,                       a2.x        },
        ss::V3 {  a3.y * a1.x * a2.x + a1.y * a3.x, a1.x * a3.x - a1.y * a3.y * a2.x,  a2.y * a3.y },
    };
        // clang-format on
    }

    inline float hash(ss::V2 p)
    {
        return ss::fract(ss::sin(ss::dot(p, ss::V2 { 127.1, 311.7 })) * 43758.5453123f);
    }

    inline float noise(ss::V2 p)
    {
        const auto i = ss::floor(p);
        const auto f = ss::fract(p);
        const auto u = f * f * (f * -2.f + 3.f);
        // clang-format off
    return -1.f + 2.f *
        ss::mix(ss::mix(hash(i + ss::V2 { 0.f, 0.f }), hash(i + ss::V2 { 1.f, 0.f }), u.x),
                ss::mix(hash(i + ss::V2 { 0.f, 1.f }), hash(i + ss::V2 { 1.f, 1.f }), u.x), u.y);
        // clang-format on
    }

    inline float diffuse(ss::V3 n, ss::V3 l, float p)
    {
        return ss::pow(ss::dot(n, l) * 0.4f + 0.6f, p);
    }

    inline float specular(ss::V3 n, ss::V3 l, ss::V3 e, float s)
    {
        return ss::pow(ss::max(ss::dot(ss::reflect(e, n), l), 0.f), s) * ((s + 8.f) / (ss::PI * 8.f));
    }

    inline ss::V3 sky_color(ss::V3 e)
    {
        e.y = (ss::max(e.y, 0.f) * 0.8f + 0.2f) * 0.8f;
        return ss::V3(ss::pow(1.f - e.y, 2.f), 1.f - e.y, 0.6f + (1.f - e.y) * 0.4f) * 1.1f;
    }

    inline float sea_octave(ss::V2 uv, float choppy)
    {
        uv += noise(uv);
        const auto swv = ss::abs(ss::cos(uv));
        auto wv = ss::abs(ss::sin(uv)) * -1.f + 1.f;
        wv = ss::mix(wv, swv, wv);
        return ss::pow(1.f - ss::pow(wv.x * wv.y, 0.65f), choppy);
    }

    inline float map(ss::V3 p, const int bound)
    {
        auto freq = SEA_FREQ;
        auto amp = SEA_HEIGHT;
        auto choppy = SEA_CHOPPY;
        auto uv = ss::V2 { p.x * 0.75f, p.z };
        auto d = 0.f;
        auto h = 0.f;
        for(int i = 0; i < bound; i++)
        {
            d = sea_octave((uv + sea_time()) * freq, choppy)
                + sea_octave((uv - sea_time()) * freq, choppy);
            h += d * amp;
            uv = ss::mul(uv, OCTAVE);
            freq *= 1.9f;
            amp *= 0.22f;
            choppy = ss::mix(choppy, 1.f, 0.2f);
        }
        return p.y - h;
    }

    inline ss::V3 sea_color(ss::V3 p, ss::V3 n, ss::V3 l, ss::V3 eye, ss::V3 dist)
    {
        auto fresnel = ss::clamp(1.f - ss::dot(n, eye * -1.f), 0.f, 1.f);
        fresnel = ss::pow(fresnel, 3.f) * 0.5f;
        const auto reflected = sky_color(ss::reflect(eye, n));
        const auto refracted = SEA_BASE + SEA_WATER_COLOR * 0.12f * diffuse(n, l, 80.f);
        const auto atten = ss::max(1.f - ss::dot(dist, dist) * 0.001f, 0.f);
        auto color = ss::mix(refracted, reflected, fresnel);
        color += SEA_WATER_COLOR * (p.y - SEA_HEIGHT) * 0.18f * atten;
        color += ss::V3 { specular(n, l, eye, 60.f) };
        return color;
    }

    inline ss::V3 normal(ss::V3 p, float eps)
    {
        const auto y = map(p, ITER_FRAGMENT);
        return ss::normalize(ss::V3 {
            map(ss::V3 { p.x + eps, p.y, p.z }, ITER_FRAGMENT) - y,
            eps,
            map(ss::V3 { p.x, p.y, p.z + eps }, ITER_FRAGMENT) - y,
        });
    }

    inline float height_map_tracing(ss::V3 ori, ss::V3 dir, ss::V3& p)
    {
        auto tm = 0.f;
        auto tx = 1000.f;
        auto hx = map(ori + dir * tx, ITER_GEOMETRY);
        if(hx > 0.f)
            return tx;
        auto hm = map(ori + dir * tm, ITER_GEOMETRY);
        auto tmid = 0.f;
        for(int i = 0; i < NUM_STEPS; i++)
        {
            tmid = ss::mix(tm, tx, hm / (hm - hx));
            p = ori + dir * tmid;
            auto hmid = map(p, ITER_GEOMETRY);
            if(hmid < 0.f)
            {
                tx = tmid;
                hx = hmid;
            }
            else
            {
                tm = tmid;
                hm = hmid;
            }
        }
        return tmid;
    }

    inline ss::V3 pixel(ss::V2 coord, float time)
    {
        auto uv = coord / ss::res;
        uv = uv * 2.f - 1.f;
        uv.x *= ss::res.x / ss::res.y;
        const auto ang = ss::V3 { ss::sin(time * 3.f) * 0.1f, ss::sin(time) * 0.2f + 0.3f, time };
        const auto ori = ss::V3 { 0.f, 3.5f, time * 5.f };
        auto dir = ss::normalize(ss::V3 { uv.x, uv.y, -2.f });
        dir.z += ss::length(uv) * 0.14f;
        dir = from_euler(ang) * ss::normalize(dir);
        auto p = ss::V3 {};
        height_map_tracing(ori, dir, p);
        auto dist = p - ori;
        auto n = normal(p, ss::dot(dist, dist) * EPSILON_NRM);
        auto light = ss::normalize(ss::V3 { 0.f, 1.f, 0.8f });
        return ss::mix(sky_color(dir), sea_color(p, n, light, dir, dist), ss::pow(ss::smoothstep(0.f, -0.02f, dir.y), 0.2f));
    }

    uint32_t shade(const ss::V2 coord)
    {
        const auto time = ss::uptime() * 0.3f;
        const auto v = ss::pow(pixel(ss::res - coord, time), 0.65f);
        return v.color(1.f);
    }
}

int main()
{
    ss::run(shade);
}
