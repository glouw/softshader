#pragma once

#include <SDL2/SDL.h>
#include <cmath>
#include <cstdio>
#include <thread>
#include <vector>
#include <chrono>

namespace softshader {

using Shade = uint32_t (*)(const int x, const int y);

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

struct V2 {
    float x { };
    float y { };
    V2(float x, float y)
        : x { x }
        , y { y }
    {
    }
    V2 operator+(V2 v) const { return V2 { x + v.x, y + v.y }; }
    V2 operator-(V2 v) const { return V2 { x - v.x, y - v.y }; }
    V2 operator*(V2 v) const { return V2 { x * v.x, y * v.y }; }
    V2 operator/(V2 v) const { return V2 { x / v.x, y / v.y }; }
    V2 operator+(float f) const { return V2 { x + f, y + f }; }
    V2 operator-(float f) const { return V2 { x - f, y - f }; }
    V2 operator*(float f) const { return V2 { x * f, y * f }; }
    V2 operator/(float f) const { return V2 { x / f, y / f }; }
    void print() const
    {
        std::printf("%f %f\n", (double)x, (double)y);
    }
};

const auto res = V2 { float(xres), float(yres) };

struct V3 {
    float x { };
    float y { };
    float z { };
    V3(float x, float y, float z)
        : x { x }
        , y { y }
        , z { z }
    {
    }
    V3 operator+(V3 v) const { return V3 { x + v.x, y + v.y, z + v.z }; }
    V3 operator-(V3 v) const { return V3 { x - v.x, y - v.y, z - v.z }; }
    V3 operator*(V3 v) const { return V3 { x * v.x, y * v.y, z * v.z }; }
    V3 operator/(V3 v) const { return V3 { x / v.x, y / v.y, z / v.z }; }
    V3 operator+(V2 v) const { return V3 { x + v.x, y + v.y, z }; }
    V3 operator-(V2 v) const { return V3 { x - v.x, y - v.y, z }; }
    V3 operator*(V2 v) const { return V3 { x * v.x, y * v.y, z }; }
    V3 operator/(V2 v) const { return V3 { x / v.x, y / v.y, z }; }
    V3 operator+(float f) const { return V3 { x + f, y + f, z + f }; }
    V3 operator-(float f) const { return V3 { x - f, y - f, z - f }; }
    V3 operator*(float f) const { return V3 { x * f, y * f, z * f }; }
    V3 operator/(float f) const { return V3 { x / f, y / f, z / f }; }
    uint32_t color(float a) const
    {
        return (uint8_t(clamp(a, 0.f, 1.f) * 0xFF) << 24)
             | (uint8_t(clamp(x, 0.f, 1.f) * 0xFF) << 16)
             | (uint8_t(clamp(y, 0.f, 1.f) * 0xFF) <<  8)
             | (uint8_t(clamp(z, 0.f, 1.f) * 0xFF) <<  0);
    }
    void print() const
    {
        std::printf("%f %f %f\n", (double)x, (double)y, (double)z);
    }
};

namespace trig {
    constexpr auto PI = std::acos(-1.0f);
    inline V2 cos(V2 v) { return V2 { std::cos(v.x), std::cos(v.y) }; }
    inline V2 sin(V2 v) { return V2 { std::sin(v.x), std::sin(v.y) }; }
    inline V3 cos(V3 v) { return V3 { std::cos(v.x), std::cos(v.y), std::cos(v.z) }; }
    inline V3 sin(V3 v) { return V3 { std::sin(v.x), std::sin(v.y), std::sin(v.z) }; }
}

class Video {
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

class Vram {
    uint32_t* pixels {};
    SDL_Texture* texture {};

public:
    std::vector<int> slices {};
    const int tasks {};
    Vram()
        : tasks { SDL_GetCPUCount() }
    {
        const auto width = yres / tasks;
        for (int i = 0; i < tasks + 1; i++)
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

class Input {
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
        if (key[SDL_SCANCODE_END] || key[SDL_SCANCODE_ESCAPE] || event.type == SDL_QUIT)
            done = true;
    }
};

struct Needle {
    Vram& vram;
    Shade shade {};
    int y0 {};
    int y1 {};
    Needle(Vram& vram, Shade shade, int y0, int y1)
        : vram { vram }
        , shade { shade }
        , y0 { y0 }
        , y1 { y1 }
    {
    }
    void operator()()
    {
        for (int y = y0; y < y1; y++)
            for (int x = 0; x < xres; x++)
                vram.put(x, y, shade(x, y));
    }
};

void draw(Vram& vram, Shade shade)
{
    auto threads = std::vector<std::thread>();
    for (int i = 0; i < vram.tasks; i++) {
        const auto needle = Needle {
            vram,
            shade,
            vram.slices[i + 0], // MULTITHREADS RENDER WITH HORIZONTAL SLICES.
            vram.slices[i + 1],
        };
        threads.push_back(std::thread { needle });
    }
    for (auto& thread : threads)
        thread.join();
}

void run(Shade shade)
{
    auto video = Video {};
    auto vram = Vram {};
    for (auto input = Input {}; !input.done; input.update()) {
        tick();
        vram.lock(video.texture);
        const auto t0 = std::chrono::high_resolution_clock::now();
        draw(vram, shade);
        const auto t1 = std::chrono::high_resolution_clock::now();
        vram.unlock();
        video.render();
        std::chrono::duration<double> dt = std::chrono::duration_cast<std::chrono::duration<double>>(t1 - t0);
        std::printf("draw fps: %f\n", 1.0 / dt.count());
    }
}
}
