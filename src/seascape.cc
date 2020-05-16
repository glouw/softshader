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
    const auto SEA_BASE = ss::V3{0.0f, 0.09f, 0.18f};
    const auto SEA_WATER_COLOR = ss::V3{0.8f, 0.9f, 0.6f} * 0.6f;

    float sea_time()
    {
        return ss::uptime() * SEA_SPEED;
    }

    ss::M3 from_euler(ss::V3 ang)
    {
        const auto a1 = ss::V2(tr::sin(ang.x), tr::cos(ang.x));
        const auto a2 = ss::V2(tr::sin(ang.y), tr::cos(ang.y));
        const auto a3 = ss::V2(tr::sin(ang.z), tr::cos(ang.z));
        return ss::M3 {
            ss::V3 {  a1.y * a3.y + a1.x * a2.x * a3.x, a1.y * a2.x * a3.x + a3.y * a1.x, -a2.y * a3.x },
            ss::V3 { -a2.y * a1.x,  a1.y * a2.y,  a2.x                                                 },
            ss::V3 {  a3.y * a1.x * a2.x + a1.y * a3.x, a1.x * a3.x - a1.y * a3.y * a2.x,  a2.y * a3.y },
        };
    }

    const auto octave_m = ss::M2 {
         1.6f, 1.2f,
        -1.2f, 1.6f,
    };

    float hash(ss::V2 p)
    {
        const auto h = tr::dot(p, ss::V2(127.1, 311.7));	
        return ss::fract(tr::sin(h) * 43758.5453123);
    }

    float noise(ss::V2 p)
    {
        const auto i = ss::floor(p);
        const auto f = ss::fract(p);	
        const auto u = f * f * (f * -2.f + 3.f);
        return -1.f + 2.f *
            tr::mix(tr::mix(hash(i + ss::V2{0.f, 0.f}),
                            hash(i + ss::V2{1.f, 0.f}), u.x),
                    tr::mix(hash(i + ss::V2{0.f, 1.f}),
                            hash(i + ss::V2{1.f, 1.f}), u.x), u.y);
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

    ss::V3 getSkyColor(ss::V3 e)
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

    float map(ss::V3 p)
    {
        auto freq = SEA_FREQ;
        auto amp = SEA_HEIGHT;
        auto choppy = SEA_CHOPPY;
        auto uv = ss::V2 { p.x, p.z };
        uv.x *= 0.75f;
        auto d = float{};
        auto h = 0.0f;
        for(int i = 0; i < ITER_GEOMETRY; i++)
        {
            // d = sea_octave((uv + sea_time()) * freq,choppy);
            // d += sea_octave((uv - sea_time()) * freq,choppy);
            // h += d * amp;
            // uv *= octave_m;
            // freq *= 1.9f;
            // amp *= 0.22f;
            // choppy = tr::mix(choppy, 1.f, 0.2f);
        }
        return p.y - h;
    }
}

int main()
{
}
//
//float map_detailed(vec3 p) {
//    float freq = SEA_FREQ;
//    float amp = SEA_HEIGHT;
//    float choppy = SEA_CHOPPY;
//    vec2 uv = p.xz; uv.x *= 0.75;
//    
//    float d, h = 0.0;    
//    for(int i = 0; i < ITER_FRAGMENT; i++) {        
//    	d = sea_octave((uv+SEA_TIME)*freq,choppy);
//    	d += sea_octave((uv-SEA_TIME)*freq,choppy);
//        h += d * amp;        
//    	uv *= octave_m; freq *= 1.9; amp *= 0.22;
//        choppy = mix(choppy,1.0,0.2);
//    }
//    return p.y - h;
//}
//
//vec3 getSeaColor(vec3 p, vec3 n, vec3 l, vec3 eye, vec3 dist) {  
//    float fresnel = clamp(1.0 - dot(n,-eye), 0.0, 1.0);
//    fresnel = pow(fresnel,3.0) * 0.5;
//        
//    vec3 reflected = getSkyColor(reflect(eye,n));    
//    vec3 refracted = SEA_BASE + diffuse(n,l,80.0) * SEA_WATER_COLOR * 0.12; 
//    
//    vec3 color = mix(refracted,reflected,fresnel);
//    
//    float atten = max(1.0 - dot(dist,dist) * 0.001, 0.0);
//    color += SEA_WATER_COLOR * (p.y - SEA_HEIGHT) * 0.18 * atten;
//    
//    color += vec3(specular(n,l,eye,60.0));
//    
//    return color;
//}
//
//// tracing
//vec3 getNormal(vec3 p, float eps) {
//    vec3 n;
//    n.y = map_detailed(p);    
//    n.x = map_detailed(vec3(p.x+eps,p.y,p.z)) - n.y;
//    n.z = map_detailed(vec3(p.x,p.y,p.z+eps)) - n.y;
//    n.y = eps;
//    return normalize(n);
//}
//
//float heightMapTracing(vec3 ori, vec3 dir, out vec3 p) {  
//    float tm = 0.0;
//    float tx = 1000.0;    
//    float hx = map(ori + dir * tx);
//    if(hx > 0.0) return tx;   
//    float hm = map(ori + dir * tm);    
//    float tmid = 0.0;
//    for(int i = 0; i < NUM_STEPS; i++) {
//        tmid = mix(tm,tx, hm/(hm-hx));                   
//        p = ori + dir * tmid;                   
//    	float hmid = map(p);
//		if(hmid < 0.0) {
//        	tx = tmid;
//            hx = hmid;
//        } else {
//            tm = tmid;
//            hm = hmid;
//        }
//    }
//    return tmid;
//}
//
//vec3 getPixel(in vec2 coord, float time) {    
//    vec2 uv = coord / iResolution.xy;
//    uv = uv * 2.0 - 1.0;
//    uv.x *= iResolution.x / iResolution.y;    
//        
//    // ray
//    vec3 ang = vec3(sin(time*3.0)*0.1,sin(time)*0.2+0.3,time);    
//    vec3 ori = vec3(0.0,3.5,time*5.0);
//    vec3 dir = normalize(vec3(uv.xy,-2.0)); dir.z += length(uv) * 0.14;
//    dir = normalize(dir) * fromEuler(ang);
//    
//    // tracing
//    vec3 p;
//    heightMapTracing(ori,dir,p);
//    vec3 dist = p - ori;
//    vec3 n = getNormal(p, dot(dist,dist) * EPSILON_NRM);
//    vec3 light = normalize(vec3(0.0,1.0,0.8)); 
//             
//    // color
//    return mix(
//        getSkyColor(dir),
//        getSeaColor(p,n,light,dir,dist),
//    	pow(smoothstep(0.0,-0.02,dir.y),0.2));
//}
//
//// main
//void mainImage( out vec4 fragColor, in vec2 fragCoord ) {
//    float time = iTime * 0.3 + iMouse.x*0.01;
//	
//#ifdef AA
//    vec3 color = vec3(0.0);
//    for(int i = -1; i <= 1; i++) {
//        for(int j = -1; j <= 1; j++) {
//        	vec2 uv = fragCoord+vec2(i,j)/3.0;
//    		color += getPixel(uv, time);
//        }
//    }
//    color /= 9.0;
//#else
//    vec3 color = getPixel(fragCoord, time);
//#endif
//    
//    // post
//	fragColor = vec4(pow(color,vec3(0.65)), 1.0);
//}