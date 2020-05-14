#pragma once

#include <SDL2/SDL.h>
#include <cmath>
#include <cstdio>
#include <thread>
#include <vector>

namespace softshader {
struct V2 {
    float x;
    float y;
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

struct V3 {
    float x;
    float y;
    float z;
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
    float clamp(float v, float lo, float hi) const
    {
        return v > hi ? hi : v < lo ? lo : v;
    }
};

namespace trig {
    constexpr auto PI = std::acos(-1.0f);
    V2 cos(V2 v) { return V2 { std::cos(v.x), std::cos(v.y) }; }
    V2 sin(V2 v) { return V2 { std::sin(v.x), std::sin(v.y) }; }
    V3 cos(V3 v) { return V3 { std::cos(v.x), std::cos(v.y), std::cos(v.z) }; }
    V3 sin(V3 v) { return V3 { std::sin(v.x), std::sin(v.y), std::sin(v.z) }; }
}

class Video {
    SDL_Window* window {};
    SDL_Renderer* renderer {};

public:
    SDL_Texture* texture {};
    const int xres {};
    const int yres {};
    Video(int xres, int yres)
        : xres { xres }
        , yres { yres }
    {
        window = SDL_CreateWindow(
            "SOFT SHADER",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            xres,
            yres,
            SDL_WINDOW_SHOWN);
        renderer = SDL_CreateRenderer(
            window,
            -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            xres,
            yres);
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
    const int xres {};
    const int yres {};
    std::vector<int> channels {};
    const int tasks {};
    Vram(int xres, int yres)
        : xres { xres }
        , yres { yres }
        , tasks { SDL_GetCPUCount() }
    {
        const auto width = yres / tasks;
        for (int i = 0; i < tasks + 1; i++)
            channels.push_back(i * width);
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

typedef uint32_t (*Shade)(const int x, const int y, const int xres, const int yres);

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
        const auto xres = vram.xres;
        const auto yres = vram.yres;
        for (int y = y0; y < y1; y++)
            for (int x = 0; x < xres; x++) {
                const auto color = shade(x, y, xres, yres);
                vram.put(x, y, color);
            }
    }
};

void draw(Vram& vram, Shade shade)
{
    auto threads = std::vector<std::thread>();
    for (int i = 0; i < vram.tasks; i++) {
        const auto needle = Needle { vram, shade, vram.channels[i], vram.channels[i + 1] };
        threads.push_back(std::thread { needle });
    }
    for (auto& thread : threads)
        thread.join();
}

void run(Shade shade)
{
    auto video = Video { 768, 432 };
    auto vram = Vram { video.xres, video.yres };
    for (auto input = Input {}; !input.done; input.update()) {
        vram.lock(video.texture);
        const auto t0 = SDL_GetTicks();
        draw(vram, shade);
        const auto t1 = SDL_GetTicks();
        vram.unlock();
        video.render();
        const int dt = t1 - t0;
        const auto ms = 15 - dt;
        const auto fps = 1000.0 / dt;
        std::printf("fps: %f\n", fps);
        if (ms > 0)
            SDL_Delay(ms); // FOR 144HZ MONITORS OR IF VSYNC FAILS.
    }
}
}
