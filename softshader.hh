#pragma once

#include <cstdio>
#include <cmath>
#include <SDL2/SDL.h>

namespace softshader
{
    struct V2
    {
        float x;
        float y;
        void print() const {
            std::printf("%f %f\n", (double) x, (double) y);
        }
        V2(float x, float y): x{x}, y{y} {}
        V2 operator+(V2 v)    const { return V2{x + v.x, y + v.y }; }
        V2 operator-(V2 v)    const { return V2{x - v.x, y - v.y }; }
        V2 operator*(V2 v)    const { return V2{x * v.x, y * v.y }; }
        V2 operator/(V2 v)    const { return V2{x / v.x, y / v.y }; }
        V2 operator+(float f) const { return V2{x + f,   y + f   }; }
        V2 operator-(float f) const { return V2{x - f,   y - f   }; }
        V2 operator*(float f) const { return V2{x * f,   y * f   }; }
        V2 operator/(float f) const { return V2{x / f,   y / f   }; }
    };

    struct V3
    {
        float x;
        float y;
        float z;
        void print() const {
            std::printf("%f %f %f\n", (double) x, (double) y, (double) z);
        }
        V3(float x, float y, float z): x{x}, y{y}, z{z} {}
        V3 operator+(V3 v)      const { return V3{x + v.x, y + v.y, z + v.z }; }
        V3 operator-(V3 v)      const { return V3{x - v.x, y - v.y, z - v.z }; }
        V3 operator*(V3 v)      const { return V3{x * v.x, y * v.y, z * v.z }; }
        V3 operator/(V3 v)      const { return V3{x / v.x, y / v.y, z / v.z }; }
        V3 operator+(V2 v)      const { return V3{x + v.x, y + v.y, z       }; }
        V3 operator-(V2 v)      const { return V3{x - v.x, y - v.y, z       }; }
        V3 operator*(V2 v)      const { return V3{x * v.x, y * v.y, z       }; }
        V3 operator/(V2 v)      const { return V3{x / v.x, y / v.y, z       }; }
        V3 operator+(float f)   const { return V3{x + f,   y + f,   z + f   }; }
        V3 operator-(float f)   const { return V3{x - f,   y - f,   z - f   }; }
        V3 operator*(float f)   const { return V3{x * f,   y * f,   z * f   }; }
        V3 operator/(float f)   const { return V3{x / f,   y / f,   z / f   }; }
        uint32_t color(float a) const {
            return (uint8_t(a * 0xFF) << 24)
                 | (uint8_t(x * 0xFF) << 16)
                 | (uint8_t(y * 0xFF) <<  8)
                 | (uint8_t(z * 0xFF) <<  0);
        }
    };

    namespace trig
    {
        constexpr auto PI = std::acos(-1.0f);
        V2 cos(V2 v) { return V2{std::cos(v.x), std::cos(v.y)}; }
        V2 sin(V2 v) { return V2{std::sin(v.x), std::sin(v.y)}; }
        V3 cos(V3 v) { return V3{std::cos(v.x), std::cos(v.y), std::cos(v.z)}; }
        V3 sin(V3 v) { return V3{std::sin(v.x), std::sin(v.y), std::sin(v.z)}; }
    }

    class Video
    {
        SDL_Window* window{};
        SDL_Renderer* renderer{};
    public:
        SDL_Texture* texture{};
        const int xres{};
        const int yres{};
        Video(int xres, int yres): xres{xres}, yres{yres}
        {
            window = SDL_CreateWindow(
                "DEMO",
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

    class Vram
    {
        uint32_t* pixels{};
        SDL_Texture* texture{};
    public:
        const int xres{};
        const int yres{};
        Vram(int xres, int yres): xres{xres}, yres{yres} {}

        void put(int x, int y, uint32_t color)
        {
            pixels[x + y * xres] = color;
        }

        void lock(SDL_Texture* texture)
        {
            void* raw;
            int pitch;
            SDL_LockTexture(texture, NULL, &raw, &pitch);
            pixels = (uint32_t*) raw;
            this->texture = texture;
        }

        void unlock()
        {
            SDL_UnlockTexture(texture);
        }
    };

    class Input
    {
        const uint8_t* key{};
    public:
        bool done{false};
        Input(): key{SDL_GetKeyboardState(NULL)} {}
        void update()
        {
            SDL_Event event;
            SDL_PollEvent(&event);
            if(key[SDL_SCANCODE_END] || key[SDL_SCANCODE_ESCAPE] || event.type == SDL_QUIT)
                done = true;
        }
    };

    void run(void (*render)(Vram& vram))
    {
        auto video = Video{600, 600};
        auto vram = Vram{video.xres, video.yres};
        for(auto input = Input{}; !input.done; input.update())
        {
            vram.lock(video.texture);
            render(vram);
            vram.unlock();
            video.render();
        }
    }
}
