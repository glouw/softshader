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

float sea_time()
{
    return ss::uptime() * SEA_SPEED;
}

ss::M3 from_euler(ss::V3 ang)
{
    const auto a1 = ss::V2(tr::sin(ang.x), tr::cos(ang.x));
    const auto a2 = ss::V2(tr::sin(ang.y), tr::cos(ang.y));
    const auto a3 = ss::V2(tr::sin(ang.z), tr::cos(ang.z));
    // clang-format off
    return ss::M3 {
        ss::V3 {  a1.y * a3.y + a1.x * a2.x * a3.x, a1.y * a2.x * a3.x + a3.y * a1.x, -a2.y * a3.x },
        ss::V3 { -a2.y * a1.x,                      a1.y * a2.y,                       a2.x        },
        ss::V3 {  a3.y * a1.x * a2.x + a1.y * a3.x, a1.x * a3.x - a1.y * a3.y * a2.x,  a2.y * a3.y },
    };
    // clang-format on
}

const auto octave_m = ss::M2 { 1.6f, 1.2f, -1.2f, 1.6f };

float hash(ss::V2 p)
{
    const auto h = tr::dot(p, ss::V2(127.1, 311.7));
    return ss::fract(tr::sin(h) * 43758.5453123f);
}

float noise(ss::V2 p)
{
    const auto i = ss::floor(p);
    const auto f = ss::fract(p);
    const auto u = f * f * (f * -2.f + 3.f);
    // clang-format off
    return -1.f + 2.f *
        tr::mix(tr::mix(hash(i + ss::V2 { 0.f, 0.f }), hash(i + ss::V2 { 1.f, 0.f }), u.x),
                tr::mix(hash(i + ss::V2 { 0.f, 1.f }), hash(i + ss::V2 { 1.f, 1.f }), u.x), u.y);
    // clang-format on
}

float diffuse(ss::V3 n, ss::V3 l, float p)
{
    return tr::pow(tr::dot(n, l) * 0.4f + 0.6f, p);
}

float specular(ss::V3 n, ss::V3 l, ss::V3 e, float s)
{
    const auto nrm = (s + 8.f) / (tr::PI * 8.f);
    return tr::pow(ss::max(tr::dot(tr::reflect(e, n), l), 0.f), s) * nrm;
}

ss::V3 get_sky_color(ss::V3 e)
{
    e.y = (ss::max(e.y, 0.f) * 0.8f + 0.2f) * 0.8f;
    return ss::V3(tr::pow(1.f - e.y, 2.f), 1.f - e.y, 0.6f + (1.f - e.y) * 0.4f) * 1.1f;
}

float sea_octave(ss::V2 uv, float choppy)
{
    uv += noise(uv);
    auto wv = tr::abs(tr::sin(uv)) * -1.f + 1.f;
    const auto swv = tr::abs(tr::cos(uv));
    wv = tr::mix(wv, swv, wv);
    return tr::pow(1.f - tr::pow(wv.x * wv.y, 0.65f), choppy);
}

float map(ss::V3 p, const int bound = ITER_GEOMETRY)
{
    auto freq = SEA_FREQ;
    auto amp = SEA_HEIGHT;
    auto choppy = SEA_CHOPPY;
    auto uv = ss::V2 { p.x, p.z };
    uv.x *= 0.75f;
    auto d = 0.f;
    auto h = 0.0f;
    for(int i = 0; i < bound; i++)
    {
        d = sea_octave((uv + sea_time()) * freq, choppy);
        d += sea_octave((uv - sea_time()) * freq, choppy);
        h += d * amp;
        uv = ss::mul(uv, octave_m);
        freq *= 1.9f;
        amp *= 0.22f;
        choppy = tr::mix(choppy, 1.f, 0.2f);
    }
    return p.y - h;
}

float map_detailed(ss::V3 p)
{
    return map(p, ITER_FRAGMENT);
}

ss::V3 get_sea_color(ss::V3 p, ss::V3 n, ss::V3 l, ss::V3 eye, ss::V3 dist)
{
    auto fresnel = ss::clamp(1.f - tr::dot(n, eye * -1.f), 0.f, 1.f);
    fresnel = tr::pow(fresnel, 3.f) * 0.5f;
    const auto reflected = get_sky_color(tr::reflect(eye, n));
    const auto refracted = SEA_BASE + SEA_WATER_COLOR * 0.12f * diffuse(n, l, 80.f);
    auto color = tr::mix(refracted, reflected, fresnel);
    const auto atten = ss::max(1.f - tr::dot(dist, dist) * 0.001f, 0.f);
    color += SEA_WATER_COLOR * (p.y - SEA_HEIGHT) * 0.18f * atten;
    color += ss::V3{specular(n, l, eye, 60.f)}; // XXX. MAYBE SINGLE CONSTRUCTOR MAKES ALL COLORS THE SAME? MAKES SENSE SINCE LIGHT IS WHITE.
    return color;
}

ss::V3 get_normal(ss::V3 p, float eps)
{
    ss::V3 n {};
    n.y = map_detailed(p);
    n.x = map_detailed(ss::V3(p.x + eps, p.y, p.z)) - n.y;
    n.z = map_detailed(ss::V3(p.x, p.y, p.z + eps)) - n.y;
    n.y = eps;
    return tr::normalize(n);
}

float height_map_tracing(ss::V3 ori, ss::V3 dir, /* OUT */ ss::V3& p)
{
    auto tm = 0.f;
    auto tx = 1000.f;
    auto hx = map(ori + dir * tx);
    if(hx > 0.f)
        return tx;
    auto hm = map(ori + dir * tm);
    auto tmid = 0.f;
    for(int i = 0; i < NUM_STEPS; i++)
    {
        tmid = tr::mix(tm, tx, hm / (hm - hx));
        p = ori + dir * tmid;
        auto hmid = map(p);
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

ss::V3 get_pixel(ss::V2 coord, float time)
{
    auto uv = coord / ss::res;
    uv = uv * 2.f - 1.f;
    uv.x *= ss::res.x / ss::res.y;
    auto ang = ss::V3 { tr::sin(time * 3.f) * 0.1f, tr::sin(time) * 0.2f + 0.3f, time };
    auto ori = ss::V3 { 0.f, 3.5f, time * 5.f };
    auto dir = tr::normalize(ss::V3 { uv.x, uv.y, -2.f });
    dir.z += tr::length(uv) * 0.14f;
    dir = from_euler(ang) * tr::normalize(dir);
    auto p = ss::V3 {};
    height_map_tracing(ori, dir, p);
    auto dist = p - ori;
    auto n = get_normal(p, tr::dot(dist, dist) * EPSILON_NRM);
    auto light = tr::normalize(ss::V3 { 0.f, 1.f, 0.8f });
    return tr::mix(get_sky_color(dir), get_sea_color(p, n, light, dir, dist), tr::pow(ss::smoothstep(0.f, -0.02f, dir.y), 0.2f));
}

uint32_t shade(const ss::V2 coord)
{
    const auto time = ss::uptime() * 0.3f;
    const auto v = tr::pow(get_pixel(ss::res - coord, time), 0.65f);
    return v.color(1.f);
}
}

int main()
{
    ss::run(shade);
}
