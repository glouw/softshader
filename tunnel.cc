#include <cstdio>
#include <cmath>
#include <SDL2/SDL.h>

struct V2
{
    float x, y;
    V2(float x, float y): x{x}, y{y} {}
    V2 operator+(V2 v)    const { return V2{x + v.x, y + v.y }; }
    V2 operator-(V2 v)    const { return V2{x - v.x, y - v.y }; }
    V2 operator*(V2 v)    const { return V2{x * v.x, y * v.y }; }
    V2 operator/(V2 v)    const { return V2{x / v.x, y / v.y }; }
    V2 operator+(float f) const { return V2{x + f,   y + f   }; }
    V2 operator-(float f) const { return V2{x - f,   y - f   }; }
    V2 operator*(float f) const { return V2{x * f,   y * f   }; }
    V2 operator/(float f) const { return V2{x / f,   y / f   }; }
    void print() const { std::printf("%f %f\n", (double) x, (double) y); };
};

struct V3
{
    float x, y, z;
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
    void print()            const { std::printf("%f %f %f\n", (double) x, (double) y, (double) z); };
    uint32_t color(float a) const { return (uint8_t(a * 0xFF) << 24)
                                         | (uint8_t(x * 0xFF) << 16)
                                         | (uint8_t(y * 0xFF) <<  8)
                                         | (uint8_t(z * 0xFF) <<  0); }
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

static void Render(Vram& vram)
{
    const auto res = V2{float(vram.xres), float(vram.yres)};
    for(int x = 0; x < vram.xres; x++)
    for(int y = 0; y < vram.yres; y++)
    {
        const auto coord = V2{float(x), float(y)};
        const auto p = (res * -1.f + coord * 2.f) / res.y;
        const auto a = std::atan2(p.y, p.x);
        const auto r = std::pow(std::pow(p.x * p.x, 4.f) + std::pow(p.y * p.y, 4.f), 1.f / 8.f);
        const auto time = SDL_GetTicks() * 0.001f;
        const auto uv = V2(1.f / r + 0.2f * time, a);
        const auto f = std::cos(12.f * uv.x) * std::cos(6.f * uv.y);
        const auto v = (trig::sin({V3{0.f, 0.5f, 1.f} + trig::PI * f}) * 0.5f + 0.5f) * r;
        const auto color = v.color(1.0f);
        vram.put(x, y, color);
    }
}

int main()
{
    auto video = Video{600, 600};
    auto vram = Vram{video.xres, video.yres};
    for(auto input = Input{}; !input.done; input.update())
    {
        vram.lock(video.texture);
        Render(vram);
        vram.unlock();
        video.render();
    }
}
