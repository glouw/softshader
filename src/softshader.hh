#pragma once

#include <SDL2/SDL.h>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <thread>
#include <vector>

namespace softshader
{

const auto xres = 768;
const auto yres = 432;

auto time = 0.f;

inline void tick()
{
    time = SDL_GetTicks() * 0.001f;
}

inline float uptime()
{
    return time;
}

inline float clamp(float v, float lo, float hi)
{
    return v > hi ? hi : v < lo ? lo : v;
}

float smoothstep(float edge0, float edge1, float x)
{
    float t = clamp((x - edge0) / (edge1 - edge0), 0.f, 1.f);
    return t * t * (3.f - 2.f * t);
}

inline float max(float a, float b)
{
    return a > b ? a : b;
}

struct V2
{
    float x {};
    float y {};
    V2()
    {
    }
    V2(float x, float y)
        : x { x }
        , y { y }
    {
    }
    V2 operator+(V2 v) const
    {
        return V2 { x + v.x, y + v.y };
    }
    V2 operator-(V2 v) const
    {
        return V2 { x - v.x, y - v.y };
    }
    V2 operator*(V2 v) const
    {
        return V2 { x * v.x, y * v.y };
    }
    V2 operator/(V2 v) const
    {
        return V2 { x / v.x, y / v.y };
    }
    V2 operator+(float f) const
    {
        return V2 { x + f, y + f };
    }
    V2 operator-(float f) const
    {
        return V2 { x - f, y - f };
    }
    V2 operator*(float f) const
    {
        return V2 { x * f, y * f };
    }
    V2 operator/(float f) const
    {
        return V2 { x / f, y / f };
    }
    V2& operator-=(float f)
    {
        x -= f;
        y -= f;
        return *this;
    }
    V2& operator+=(float f)
    {
        x += f;
        y += f;
        return *this;
    }
    V2& operator*=(float f)
    {
        x *= f;
        y *= f;
        return *this;
    }
    V2& operator/=(float f)
    {
        x /= f;
        y /= f;
        return *this;
    }
    V2& operator-=(V2 v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }
    V2& operator+=(V2 v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }
    V2& operator*=(V2 v)
    {
        x *= v.x;
        y *= v.y;
        return *this;
    }
    V2& operator/=(V2 v)
    {
        x /= v.x;
        y /= v.y;
        return *this;
    }

    float& operator[](int i)
    {
        return (i == 0) ? x : y;
    }

    void print() const
    {
        std::printf("%f %f\n", (double)x, (double)y);
    }
};

struct M2
{
    V2 x {};
    V2 y {};
    M2()
    {
    }
    M2(float a, float b, float c, float d)
        : x { a, b }
        , y { c, d }
    {
    }

    M2(V2 x, V2 y)
        : x { x }
        , y { y }
    {
    }

    V2& operator[](int i)
    {
        return (i == 0) ? x : y;
    }
};

const auto res = V2 { float { xres }, float { yres } };

struct V3
{
    float x {};
    float y {};
    float z {};
    V3()
    {
    }
    V3(float xyz)
        : x { xyz }
        , y { xyz }
        , z { xyz }
    {
    }
    V3(float x, float y, float z)
        : x { x }
        , y { y }
        , z { z }
    {
    }
    V3 operator+(V3 v) const
    {
        return V3 { x + v.x, y + v.y, z + v.z };
    }
    V3 operator-(V3 v) const
    {
        return V3 { x - v.x, y - v.y, z - v.z };
    }
    V3 operator*(V3 v) const
    {
        return V3 { x * v.x, y * v.y, z * v.z };
    }
    V3 operator/(V3 v) const
    {
        return V3 { x / v.x, y / v.y, z / v.z };
    }
    V3 operator+(V2 v) const
    {
        return V3 { x + v.x, y + v.y, z };
    }
    V3 operator-(V2 v) const
    {
        return V3 { x - v.x, y - v.y, z };
    }
    V3 operator*(V2 v) const
    {
        return V3 { x * v.x, y * v.y, z };
    }
    V3 operator/(V2 v) const
    {
        return V3 { x / v.x, y / v.y, z };
    }
    V3 operator+(float f) const
    {
        return V3 { x + f, y + f, z + f };
    }
    V3 operator-(float f) const
    {
        return V3 { x - f, y - f, z - f };
    }
    V3 operator*(float f) const
    {
        return V3 { x * f, y * f, z * f };
    }
    V3 operator/(float f) const
    {
        return V3 { x / f, y / f, z / f };
    }
    V3& operator-=(float f)
    {
        x -= f;
        y -= f;
        z -= f;
        return *this;
    }
    V3& operator+=(float f)
    {
        x += f;
        y += f;
        z += f;
        return *this;
    }
    V3& operator*=(float f)
    {
        x *= f;
        y *= f;
        z *= f;
        return *this;
    }
    V3& operator/=(float f)
    {
        x /= f;
        y /= f;
        z /= f;
        return *this;
    }
    V3& operator-=(V3 v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }
    V3& operator+=(V3 v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }
    V3& operator*=(V3 v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        return *this;
    }
    V3& operator/=(V3 v)
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        return *this;
    }
    float& operator[](int i)
    {
        return (i == 0) ? x : (i == 1) ? y : z;
    }
    uint32_t color(float a) const
    {
        // clang-format off
        return (uint8_t(clamp(a, 0.f, 1.f) * 0xFF) << 24) | (uint8_t(clamp(x, 0.f, 1.f) * 0xFF) << 16)
             | (uint8_t(clamp(y, 0.f, 1.f) * 0xFF) <<  8) | (uint8_t(clamp(z, 0.f, 1.f) * 0xFF) <<  0);
        // clang-format on
    }
    void print() const
    {
        std::printf("%f %f %f\n", (double)x, (double)y, (double)z);
    }
};

struct M3
{
    V3 x {};
    V3 y {};
    V3 z {};
    M3()
    {
    }
    M3(float a, float b, float c,
        float d, float e, float f,
        float g, float h, float i)
        : x { a, b, c }
        , y { d, e, f }
        , z { g, h, i }
    {
    }
    M3(V3 x, V3 y, V3 z)
        : x { x }
        , y { y }
        , z { z }
    {
    }
    V3 operator*(V3 v)
    {
        return V3 {
            x.x * v.x + x.y * v.y + x.z * v.z,
            y.x * v.x + y.y * v.y + y.z * v.z,
            z.x * v.x + z.y * v.y + z.z * v.z,
        };
    }
    V3& operator[](int i)
    {
        return (i == 0) ? x : (i == 1) ? y : z;
    }
};

namespace trig
{
    constexpr auto PI = std::acos(-1.0f);
    inline float sin(float a)
    {
        return std::sin(a);
    }
    inline V2 sin(V2 v)
    {
        return V2 { std::sin(v.x), std::sin(v.y) };
    }
    inline V3 sin(V3 v)
    {
        return V3 { std::sin(v.x), std::sin(v.y), std::sin(v.z) };
    }
    inline float cos(float a)
    {
        return std::cos(a);
    }
    inline V2 cos(V2 v)
    {
        return V2 { std::cos(v.x), std::cos(v.y) };
    }
    inline V3 cos(V3 v)
    {
        return V3 { std::cos(v.x), std::cos(v.y), std::cos(v.z) };
    }
    inline float length(float f)
    {
        return f;
    }
    inline float length(V2 v)
    {
        return std::sqrt(v.x * v.x + v.y * v.y);
    }
    inline float length(V3 v)
    {
        return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    }
    inline float abs(float a)
    {
        return std::fabs(a);
    }
    inline V2 abs(V2 v)
    {
        return V2 { std::fabs(v.x), std::fabs(v.y) };
    }
    inline V3 abs(V3 v)
    {
        return V3 { std::fabs(v.x), std::fabs(v.y), std::fabs(v.z) };
    }
    inline float mod(float x, float y)
    {
        return x - y * std::floor(x / y);
    }
    inline V2 mod(V2 v, float f)
    {
        return V2 { mod(v.x, f), mod(v.y, f) };
    }
    inline V3 mod(V3 v, float f)
    {
        return V3 { mod(v.x, f), mod(v.y, f), mod(v.z, f) };
    }
    inline float atan2(float y, float x)
    {
        return std::atan2(y, x);
    }
    inline float pow(float x, float n)
    {
        return std::pow(x, n);
    }
    inline V2 pow(V2 v, float n)
    {
        return V2 { std::pow(v.x, n), std::pow(v.y, n) };
    }
    inline V3 pow(V3 v, float n)
    {
        return V3 { std::pow(v.x, n), std::pow(v.y, n), std::pow(v.z, n) };
    }
    inline float dot(V2 x, V2 y)
    {
        return x.x * y.x + x.y * y.y;
    }
    inline float dot(V3 x, V3 y)
    {
        return x.x * y.x + x.y * y.y + x.z * y.z;
    }
    inline float mix(float x, float y, float a)
    {
        return x * (1.f - a) + y * a;
    }
    inline V2 mix(V2 x, V2 y, float a)
    {
        return V2 { mix(x.x, y.x, a), mix(x.y, y.y, a) };
    }
    inline V2 mix(V2 x, V2 y, V2 a)
    {
        return V2 { mix(x.x, y.x, a.x), mix(x.y, y.y, a.y) };
    }
    inline V3 mix(V3 x, V3 y, float a)
    {
        return V3 { mix(x.x, y.x, a), mix(x.y, y.y, a), mix(x.z, y.z, a) };
    }
    inline V3 mix(V3 x, V3 y, V3 a)
    {
        return V3 { mix(x.x, y.x, a.x), mix(x.y, y.y, a.y), mix(x.z, y.z, a.z) };
    }
    inline V2 reflect(V2 i, V2 n)
    {
        return i - n * dot(n, i) * 2.f;
    }
    inline V3 reflect(V3 i, V3 n)
    {
        return i - n * dot(n, i) * 2.f;
    }
    inline V2 normalize(V2 v)
    {
        return v / length(v);
    }
    inline V3 normalize(V3 v)
    {
        return v / length(v);
    }
}

inline float floor(float x)
{
    return std::floor(x);
}

inline V2 floor(V2 v)
{
    return V2 { floor(v.x), floor(v.y) };
}

inline float fract(float x)
{
    return x - floor(x);
}

inline V2 fract(V2 v)
{
    return V2 { fract(v.x), fract(v.y) };
}

inline V2 mul(V2 v, M2 m)
{
    return V2 {
        v.x * m.x.x + v.y * m.x.y,
        v.x * m.y.x + v.y * m.y.y,
    };
}

inline V3 mul(V3 v, M3 m)
{
    return V3 {
        v.x * m.x.x + v.y * m.x.y + v.z * m.x.z,
        v.x * m.y.x + v.y * m.y.y + v.z * m.y.z,
        v.x * m.z.x + v.y * m.z.y + v.z * m.z.z,
    };
}

class Video
{
    SDL_Window* window {};
    SDL_Renderer* renderer {};

public:
    SDL_Texture* texture {};
    Video()
    {
        window = SDL_CreateWindow("SOFTSHADER", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, xres, yres, SDL_WINDOW_SHOWN);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, xres, yres);
    }

    void render()
    {
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    ~Video()
    {
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyTexture(texture);
        SDL_Quit();
    }
};

class Vram
{
    uint32_t* pixels {};
    SDL_Texture* texture {};

public:
    std::vector<int> slices {};
    const int cpus {};
    Vram()
        : cpus { SDL_GetCPUCount() }
    {
        const auto width = yres / cpus;
        for(int i = 0; i < cpus + 1; i++)
            slices.push_back(i * width);
    }

    void put(int x, int y, uint32_t color)
    {
        pixels[x + y * xres] = color;
    }

    void lock(SDL_Texture* texture)
    {
        void* raw;
        int pitch;
        SDL_LockTexture(texture, NULL, &raw, &pitch);
        pixels = (uint32_t*)raw;
        this->texture = texture;
    }

    void unlock()
    {
        SDL_UnlockTexture(texture);
    }
};

class Input
{
    const uint8_t* key {};

public:
    bool done { false };
    Input()
        : key { SDL_GetKeyboardState(NULL) }
    {
    }
    void update()
    {
        SDL_Event event;
        SDL_PollEvent(&event);
        if(key[SDL_SCANCODE_END] || key[SDL_SCANCODE_ESCAPE] || event.type == SDL_QUIT)
            done = true;
    }
};

using Shade = uint32_t (*)(const V2);

struct Needle
{
    Vram& vram;
    const Shade shade {};
    const int y0 {};
    const int y1 {};
    Needle(Vram& vram, Shade shade, int y0, int y1)
        : vram { vram }
        , shade { shade }
        , y0 { y0 }
        , y1 { y1 }
    {
    }
    void operator()()
    {
        for(int y = y0; y < y1; y++)
            for(int x = 0; x < xres; x++)
            {
                const auto coord = V2 { float(x), float(y) };
                vram.put(x, y, shade(coord));
            }
    }
};

void draw(Vram& vram, Shade shade)
{
    auto threads = std::vector<std::thread>();
    for(int i = 0; i < vram.cpus; i++)
    {
        const auto needle = Needle {
            vram,
            shade,
            vram.slices[i + 0], // MULTITHREADS RENDER WITH HORIZONTAL SLICES.
            vram.slices[i + 1],
        };
        threads.push_back(std::thread { needle });
    }
    for(auto& thread : threads)
        thread.join();
}

void run(Shade shade)
{
    auto video = Video {};
    auto vram = Vram {};
    auto cycles = int {};
    for(auto input = Input {}; !input.done; input.update())
    {
        tick();
        vram.lock(video.texture);
        const auto t0 = std::chrono::high_resolution_clock::now();
        draw(vram, shade);
        const auto t1 = std::chrono::high_resolution_clock::now();
        vram.unlock();
        video.render();
        if(cycles % 10 == 0)
        {
            std::chrono::duration<double> dt = std::chrono::duration_cast<std::chrono::duration<double>>(t1 - t0);
            std::printf("draw fps: %f\n", 1.0 / dt.count());
        }
        cycles += 1;
    }
}
}

namespace ss = softshader;
namespace tr = ss::trig;
