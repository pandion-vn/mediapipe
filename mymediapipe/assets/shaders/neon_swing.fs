#version 410
in vec2 TexCoords;
out vec4 color;

uniform sampler2D sprite;
uniform sampler2D viewfinder;
uniform sampler2D background;

uniform vec3 spriteColor;
uniform int phase;
uniform float time;
uniform vec2 resolution;

// shadertoy emulation
#define iChannel0 sprite
#define iTime time
#define iResolution resolution
// #define fragColor color

vec4 phaseViewFinder()
{
    vec2 q = gl_FragCoord.xy / iResolution.xy;
    vec2 uv = 0.5 + (q-0.5)*(0.9 + 0.1*sin(0.2*iTime));

    vec3 oricol = texture(viewfinder, vec2(q.x,1.0-q.y) ).xyz;
    vec3 col;

    col.r = texture(viewfinder, vec2(uv.x+0.003,-uv.y)).x;
    col.g = texture(viewfinder, vec2(uv.x+0.000,-uv.y)).y;
    col.b = texture(viewfinder, vec2(uv.x-0.003,-uv.y)).z;

    col = clamp(col*0.5+0.5*col*col*1.2,0.0,1.0);

    col *= 0.5 + 0.5*16.0*uv.x*uv.y*(1.0-uv.x)*(1.0-uv.y);

    col *= vec3(0.95,1.05,0.95);

    col *= 0.9+0.1*sin(10.0*iTime+uv.y*1000.0);

    col *= 0.99+0.01*sin(110.0*iTime);

    float comp = smoothstep( 0.2, 0.7, sin(iTime) );
    col = mix( col, oricol, clamp(-2.0+2.0*q.x+3.0*comp,0.0,1.0) );

    return vec4(col, 1.0);
}

vec4 phaseSobel1()
{
    vec3 c[9];
    for (int i=0; i < 3; ++i)
    {
        for (int j=0; j < 3; ++j)
        {
            vec2 uv = (gl_FragCoord.xy+vec2(i-1,j-1)) / iResolution.xy;
            vec2 uv1 = vec2(uv.x, 1.0 - uv.y);
            c[3*i+j] = texture(iChannel0, uv1).rgb;
        }
    }
    
    vec3 Lx = 2.0*(c[7]-c[1]) + c[6] + c[8] - c[2] - c[0];
    vec3 Ly = 2.0*(c[3]-c[5]) + c[6] + c[0] - c[2] - c[8];
    vec3 G = sqrt(Lx*Lx+Ly*Ly);
    
    return vec4(G, 1.0);
}

vec4 phaseSobel()
{
    vec2 uv = 1.0 - gl_FragCoord.xy / iResolution.xy;
    // vec2 blurredUV = vec2(uv.x+0.002, uv.y+0.002);
    // vec4 baseColor = vec4(texture(iChannel0, uv).rgb, 1);
    // vec4 edges = 1.0 - (baseColor / vec4(texture(iChannel0, blurredUV).rgb, 1));
    // color = vec4(length(edges));
    vec4 color1 =  texture(iChannel0, uv);
    float gray = length(color1.rgb);
    return vec4(vec3(step(0.06, length(vec2(dFdx(gray), dFdy(gray))))), 1.0);
}

vec4 phaseIdle() {
    return vec4(spriteColor, 1.0) * texture(sprite, TexCoords);
}

vec4 phaseBackground() {
    return vec4(spriteColor, 1.0) * texture(background, TexCoords);
}

void main()
{
    switch(phase) {
       case 2:
           vec4 color1 = phaseViewFinder();
           color = mix(texture(sprite, TexCoords), color1, 0.5);
           break;
       case 1: 
            // color = phaseSobel1(); 
            color = phaseIdle();
           break;
       default: 
           color = phaseBackground();
    }
}
