#include "mediapipe/framework/port/ret_check.h"
#include "mediapipe/framework/port/status.h"
#include "gl_base_calculator.h"
// #include "mediapipe/gpu/gl_simple_shaders.h"
#include "mediapipe/gpu/shader_util.h"

namespace mediapipe {

enum { ATTRIB_VERTEX, ATTRIB_TEXTURE_POSITION, NUM_ATTRIBUTES };

class GlShaderCalculator : public GlBaseCalculator {
public:
    absl::Status GlSetup() override;
    absl::Status GlBind() override;
    absl::Status GlRender(const GlTexture& src, const GlTexture& dst, double timestamp) override;
    absl::Status GlRenderDrawArrays(GLint& positionLocation);
    absl::Status GlRenderDrawElements(GLint& positionLocation);
    absl::Status GlTeardown() override;

private:
    GLuint program_ = 0;
    GLint texture_;
};

REGISTER_CALCULATOR(GlShaderCalculator);

absl::Status GlShaderCalculator::GlSetup() {
    // Load vertex and fragment shaders
    const GLint attr_location[NUM_ATTRIBUTES] = {
        ATTRIB_VERTEX,
        ATTRIB_TEXTURE_POSITION,
    };

    const GLchar* attr_name[NUM_ATTRIBUTES] = {
        "position",
        "tex_coord",
    };

    const GLchar* vert_src = R"(
        attribute vec4 position;
        attribute vec4 tex_coord;

        varying vec2 pos;

        void main() {
            pos = tex_coord.xy;

            vec4 aposition = vec4(position);
            aposition.xy = aposition.xy * 2. - 1.;

            gl_Position = position;
        }
    )";

    const GLchar* frag_src1 = R"(
        precision mediump float;

        varying vec2 pos;
        uniform sampler2D texture;

        uniform float millis;

        // vec3 lensflare(vec2 uv, vec2 pos)
        // {
        //     float intensity = 1.5;
        //     vec2 main = uv-pos;
        //     vec2 uvd = uv*(length(uv));
            
        //     float dist=length(main); dist = pow(dist,.1);
        //     float f1 = max(0.01-pow(length(uv+1.2*pos),1.9),.0)*7.0;
        //     float f2 = max(1.0/(1.0+32.0*pow(length(uvd+0.8*pos),2.0)),.0)*00.1;
        //     float f22 = max(1.0/(1.0+32.0*pow(length(uvd+0.85*pos),2.0)),.0)*00.08;
        //     float f23 = max(1.0/(1.0+32.0*pow(length(uvd+0.9*pos),2.0)),.0)*00.06;
            
        //     vec2 uvx = mix(uv,uvd,-0.5);
            
        //     float f4 = max(0.01-pow(length(uvx+0.4*pos),2.4),.0)*6.0;
        //     float f42 = max(0.01-pow(length(uvx+0.45*pos),2.4),.0)*5.0;
        //     float f43 = max(0.01-pow(length(uvx+0.5*pos),2.4),.0)*3.0;
            
        //     uvx = mix(uv,uvd,-.4);
            
        //     float f5 = max(0.01-pow(length(uvx+0.2*pos),5.5),.0)*2.0;
        //     float f52 = max(0.01-pow(length(uvx+0.4*pos),5.5),.0)*2.0;
        //     float f53 = max(0.01-pow(length(uvx+0.6*pos),5.5),.0)*2.0;
            
        //     uvx = mix(uv,uvd,-0.5);
            
        //     float f6 = max(0.01-pow(length(uvx-0.3*pos),1.6),.0)*6.0;
        //     float f62 = max(0.01-pow(length(uvx-0.325*pos),1.6),.0)*3.0;
        //     float f63 = max(0.01-pow(length(uvx-0.35*pos),1.6),.0)*5.0;
            
        //     vec3 c = vec3(.0);
            
        //     c.r+=f2+f4+f5+f6; c.g+=f22+f42+f52+f62; c.b+=f23+f43+f53+f63;
        //     c = c*1.3 - vec3(length(uvd)*.05);
            
        //     return c * intensity;
        // }

        // vec3 cc(vec3 color, float factor,float factor2) // color modifier
        // {
        //     float w = color.x+color.y+color.z;
        //     return mix(color,vec3(w)*factor,w*factor2);
        // }

        const vec3 suncolor = vec3(0.643,0.494,0.867);
        float getSun(vec2 uv){
            return length(uv) < 0.009 ? 1.0 : 0.0;
        }
        //from: https://www.shadertoy.com/view/XdfXRX
        vec3 lensflares(vec2 uv, vec2 pos, out vec3 sunflare, out vec3 lensflare)
        {
            vec2 main = uv-pos;
            vec2 uvd = uv*(length(uv));

            float ang = atan(main.y, main.x);
            float dist = length(main);
            dist = pow(dist, 0.1);

            float f0 = 1.0/(length(uv-pos)*25.0+1.0);
            f0 = pow(f0, 2.0);

            f0 = f0+f0*(sin((ang+1.0/18.0)*12.0)*.1+dist*.1+.8);

            float f2 = max(1.0/(1.0+32.0*pow(length(uvd+0.8*pos),2.0)),.0)*00.25;
            float f22 = max(1.0/(1.0+32.0*pow(length(uvd+0.85*pos),2.0)),.0)*00.23;
            float f23 = max(1.0/(1.0+32.0*pow(length(uvd+0.9*pos),2.0)),.0)*00.21;

            vec2 uvx = mix(uv,uvd,-0.5);

            float f4 = max(0.01-pow(length(uvx+0.4*pos),2.4),.0)*6.0;
            float f42 = max(0.01-pow(length(uvx+0.45*pos),2.4),.0)*5.0;
            float f43 = max(0.01-pow(length(uvx+0.5*pos),2.4),.0)*3.0;

            uvx = mix(uv,uvd,-.4);

            float f5 = max(0.01-pow(length(uvx+0.2*pos),5.5),.0)*2.0;
            float f52 = max(0.01-pow(length(uvx+0.4*pos),5.5),.0)*2.0;
            float f53 = max(0.01-pow(length(uvx+0.6*pos),5.5),.0)*2.0;

            uvx = mix(uv,uvd,-0.5);

            float f6 = max(0.01-pow(length(uvx-0.3*pos),1.6),.0)*6.0;
            float f62 = max(0.01-pow(length(uvx-0.325*pos),1.6),.0)*3.0;
            float f63 = max(0.01-pow(length(uvx-0.35*pos),1.6),.0)*5.0;

            sunflare = vec3(f0);
            lensflare = vec3(f2+f4+f5+f6, f22+f42+f52+f62, f23+f43+f53+f63);

            return sunflare+lensflare;
        }

        vec3 anflares(vec2 uv, float threshold, float intensity, float stretch, float brightness)
        {
            threshold = 1.0 - threshold;

            vec3 hdr = vec3(getSun(uv));
            hdr = vec3(floor(threshold+pow(hdr.r, 1.0)));

            float d = intensity;
            float c = intensity*stretch;

            for (float i=c; i>-1.0; i--){
                float texL = getSun(uv+vec2(i/d, 0.0));
                float texR = getSun(uv-vec2(i/d, 0.0));
                
                hdr += floor(threshold+pow(max(texL,texR), 4.0))*(1.0-i/c);
            }
            
            return hdr*brightness;
        }

        vec3 anflares(vec2 uv, float intensity, float stretch, float brightness)
        {
            uv.x *= 1.0/(intensity*stretch);
            uv.y *= 0.5;
            return vec3(smoothstep(0.009, 0.0, length(uv)))*brightness;
        }


        void main() {
            // vec4 c1 = vec4(0.5, 0.1, 0.9, 1.);
            // vec4 c2 = vec4(0.1, 0.8, 0.7, 1.);
            // vec4 c = mix(c1, c2, pos.x);
            // gl_FragColor = c;
            // sine wave
            // float c = (sin(pos.x * 16. + millis) + 1.) / 2.;
            // gl_FragColor = vec4(c, 0., 1., 1.);
            // cycle
            // vec3 cycle = vec3(0.5, 0.5, 0.3);
            // float d = length(pos - cycle.xy) - cycle.z;
            // d = step(0., d);
            // gl_FragColor = vec4(d, d, d, 1.);
            // texture
            // gl_FragColor = texture2D(texture, pos) * c;
            // lens flare
            // vec2 uv = pos.xy / iResolution.xy - 0.5;
	        // uv.x *= iResolution.x / iResolution.y; //fix aspect ratio
            // vec2 uv = pos.xy;

            // vec3 mouse = vec3(pos.xy - 0.5, .5);
            // mouse.x = sin(millis) * .5;
            // mouse.y = sin(millis * .913) * .5;
            
            // vec3 color = vec3(1.5, 1.2, 1.2) * lensflare(uv, mouse.xy);
	        // color = cc(color, .5, .1);
            // vec3 color = lensflare(uv, mouse.xy);
            // color = cc(color, .5, .1);
            // gl_FragColor = texture2D(texture, pos) * vec4(color, (color.r + color.g + color.b) / 3.0);

            // anamorphic lens flare
            vec2 uv = pos.xy - 0.5;
            vec2 mouse = pos.xy - 0.5;

            vec3 col;
            vec3 sun, sunflare, lensflare;
            vec3 flare = lensflares(uv*1.5, mouse*1.5, sunflare, lensflare);

            vec3 anflare = pow(anflares(uv-mouse, 0.5, 400.0, 0.9, 0.1), vec3(4.0));
            sun += getSun(uv-mouse) + (flare + anflare)*suncolor*2.0;
            col += sun;
            col = pow(col, vec3(1.0/2.2));
            gl_FragColor = texture2D(texture, pos) * vec4(col, 1.);
        }
    )";

    const GLchar* frag_src2 = R"(
        precision mediump float;

        varying vec2 pos;
        uniform sampler2D texture;
        uniform float millis;

        // Combine distance field functions
        float smoothMerge(float d1, float d2, float k) {
            float h = clamp(0.5 + 0.5*(d2 - d1)/k, 0.0, 1.0);
            return mix(d2, d1, h) - k * h * (1.0-h);
        }

        float merge(float d1, float d2) {
            return min(d1, d2);
        }

        float mergeExclude(float d1, float d2) {
            return min(max(-d1, d2), max(-d2, d1));
        }

        float substract(float d1, float d2) {
            return max(-d1, d2);
        }

        float intersect(float d1, float d2) {
            return max(d1, d2);
        }

        // Rotation and translation
        vec2 rotateCCW(vec2 p, float a) {
            mat2 m = mat2(cos(a), sin(a), -sin(a), cos(a));
            return p * m;	
        }

        vec2 rotateCW(vec2 p, float a) {
            mat2 m = mat2(cos(a), -sin(a), sin(a), cos(a));
            return p * m;
        }

        vec2 translate(vec2 p, vec2 t) {
            return p - t;
        }

        // Distance field functions
        float pie(vec2 p, float angle) {
            angle = radians(angle) / 2.0;
            vec2 n = vec2(cos(angle), sin(angle));
            return abs(p).x * n.x + p.y*n.y;
        }

        float circleDist(vec2 p, float radius) {
            return length(p) - radius;
        }

        float triangleDist(vec2 p, float radius) {
            return max(	abs(p).x * 0.866025 + 
                        p.y * 0.5, -p.y) 
                        -radius * 0.5;
        }

        float triangleDist(vec2 p, float width, float height) {
            vec2 n = normalize(vec2(height, width / 2.0));
            return max(	abs(p).x*n.x + p.y*n.y - (height*n.y), -p.y);
        }

        float semiCircleDist(vec2 p, float radius, float angle, float width) {
            width /= 2.0;
            radius -= width;
            return substract(pie(p, angle), 
                             abs(circleDist(p, radius)) - width);
        }

        float boxDist(vec2 p, vec2 size, float radius) {
            size -= vec2(radius);
            vec2 d = abs(p) - size;
            return min(max(d.x, d.y), 0.0) + length(max(d, 0.0)) - radius;
        }

        float lineDist(vec2 p, vec2 start, vec2 end, float width) {
            vec2 dir = start - end;
            float lngth = length(dir);
            dir /= lngth;
            vec2 proj = max(0.0, min(lngth, dot((start - p), dir))) * dir;
            return length( (start - p) - proj ) - (width / 2.0);
        }

        // Masks for drawing
        float fillMask(float dist) {
            return clamp(-dist, 0.0, 1.0);
        }


        float innerBorderMask(float dist, float width) {
            //dist += 1.0;
            float alpha1 = clamp(dist + width, 0.0, 1.0);
            float alpha2 = clamp(dist, 0.0, 1.0);
            return alpha1 - alpha2;
        }

        float outerBorderMask(float dist, float width) {
            //dist += 1.0;
            float alpha1 = clamp(dist, 0.0, 1.0);
            float alpha2 = clamp(dist - width, 0.0, 1.0);
            return alpha1 - alpha2;
        }

        // The scene
        float sceneDist(vec2 p) {
            float c = circleDist(		translate(p, vec2(100, 250)), 40.0);
            float b1 =  boxDist(		translate(p, vec2(200, 250)), vec2(40, 40), 	0.0);
            float b2 =  boxDist(		translate(p, vec2(300, 250)), vec2(40, 40), 	10.0);
            float l = lineDist(			p, 			 vec2(370, 220),  vec2(430, 280),	10.0);
            float t1 = triangleDist(	translate(p, vec2(500, 210)), 80.0, 			80.0);
            float t2 = triangleDist(	rotateCW(translate(p, vec2(600, 250)), millis), 40.0);
            
            float m = 	merge(c, b1);
            m = 		merge(m, b2);
            m = 		merge(m, l);
            m = 		merge(m, t1);
            m = 		merge(m, t2);
            
            float b3 = boxDist(		translate(p, vec2(100, sin(millis * 3.0 + 1.0) * 40.0 + 100.0)), 
                                    vec2(40, 15), 	0.0);
            float c2 = circleDist(	translate(p, vec2(100, 100)),	30.0);
            float s = substract(b3, c2);
            
            float b4 = boxDist(		translate(p, vec2(200, sin(millis * 3.0 + 2.0) * 40.0 + 100.0)), 
                                    vec2(40, 15), 	0.0);
            float c3 = circleDist(	translate(p, vec2(200, 100)), 	30.0);
            float i = intersect(b4, c3);
            
            float b5 = boxDist(		translate(p, vec2(300, sin(millis * 3.0 + 3.0) * 40.0 + 100.0)), 
                                    vec2(40, 15), 	0.0);
            float c4 = circleDist(	translate(p, vec2(300, 100)), 	30.0);
            float a = merge(b5, c4);
            
            float b6 = boxDist(		translate(p, vec2(400, 100)),	vec2(40, 15), 	0.0);
            float c5 = circleDist(	translate(p, vec2(400, 100)), 	30.0);
            float sm = smoothMerge(b6, c5, 10.0);
            
            float sc = semiCircleDist(translate(p, vec2(500,100)), 40.0, 90.0, 10.0);
            
            float b7 = boxDist(		translate(p, vec2(600, sin(millis * 3.0 + 3.0) * 40.0 + 100.0)), 
                                    vec2(40, 15), 	0.0);
            float c6 = circleDist(	translate(p, vec2(600, 100)), 	30.0);
            float e = mergeExclude(b7, c6);
            
            m = merge(m, s);
            m = merge(m, i);
            m = merge(m, a);
            m = merge(m, sm);
            m = merge(m, sc);
            m = merge(m, e);
            
            return m;
        }

        float sceneSmooth(vec2 p, float r) {
            float accum = sceneDist(p);
            accum += sceneDist(p + vec2(0.0, r));
            accum += sceneDist(p + vec2(0.0, -r));
            accum += sceneDist(p + vec2(r, 0.0));
            accum += sceneDist(p + vec2(-r, 0.0));
            return accum / 5.0;
        }

        // Shadow and light
        float shadow(vec2 p, vec2 pos, float radius) {
            vec2 dir = normalize(pos - p);
            float dl = length(p - pos);
            
            // fraction of light visible, starts at one radius (second half added in the end);
            float lf = radius * dl;
            
            // distance traveled
            float dt = 0.01;

            for (int i = 0; i < 64; ++i) {				
                // distance to scene at current position
                float sd = sceneDist(p + dir * dt);

                // early out when this ray is guaranteed to be full shadow
                if (sd < -radius) 
                    return 0.0;
                
                // width of cone-overlap at light
                // 0 in center, so 50% overlap: add one radius outside of loop to get total coverage
                // should be '(sd / dt) * dl', but '*dl' outside of loop
                lf = min(lf, sd / dt);
                
                // move ahead
                dt += max(1.0, abs(sd));
                if (dt > dl) break;
            }

            // multiply by dl to get the real projected overlap (moved out of loop)
            // add one radius, before between -radius and + radius
            // normalize to 1 ( / 2*radius)
            lf = clamp((lf*dl + radius) / (2.0 * radius), 0.0, 1.0);
            lf = smoothstep(0.0, 1.0, lf);
            return lf;
        }

        vec4 drawLight(vec2 p, vec2 pos, vec4 color, float dist, float range, float radius) {
            // distance to light
            float ld = length(p - pos);
            
            // out of range
            if (ld > range) return vec4(0.0);
            
            // shadow and falloff
            float shad = shadow(p, pos, radius);
            float fall = (range - ld)/range;
            fall *= fall;
            float source = fillMask(circleDist(p - pos, radius));
            return (shad * fall + source) * color;
        }

        float luminance(vec4 col) {
            return 0.2126 * col.r + 0.7152 * col.g + 0.0722 * col.b;
        }

        void setLuminance(inout vec4 col, float lum) {
            lum /= luminance(col);
            col *= lum;
        }

        float AO(vec2 p, float dist, float radius, float intensity) {
            float a = clamp(dist / radius, 0.0, 1.0) - 1.0;
            return 1.0 - (pow(abs(a), 5.0) + 1.0) * intensity + (1.0 - intensity);
            return smoothstep(0.0, 1.0, dist / radius);
        }

        // The program
        void main() {
            vec2 p = pos.xy + vec2(0.5);
            vec2 reso = vec2(1., 1.);
            vec2 c = reso.xy / 2.0;
            
            //float dist = sceneSmooth(p, 5.0);
            float dist = sceneDist(p);
            
            vec2 light1Pos = pos.xy;
            vec4 light1Col = vec4(0.75, 1.0, 0.5, 1.0);
            setLuminance(light1Col, 0.4);
            
            vec2 light2Pos = vec2(reso.x * (sin(millis + 3.1415) + 1.2) / 2.4, 175.0);
            vec4 light2Col = vec4(1.0, 0.75, 0.5, 1.0);
            setLuminance(light2Col, 0.5);
            
            vec2 light3Pos = vec2(reso.x * (sin(millis) + 1.2) / 2.4, 340.0);
            vec4 light3Col = vec4(0.5, 0.75, 1.0, 1.0);
            setLuminance(light3Col, 0.6);
            
            // gradient
            vec4 col = vec4(0.5, 0.5, 0.5, 1.0) * (1.0 - length(c - p)/reso.x);
            // grid
            // col *= clamp(min(mod(p.y, 10.0), mod(p.x, 10.0)), 0.9, 1.0);
            // ambient occlusion
            // col *= AO(p, sceneSmooth(p, 10.0), 40.0, 0.4);
            //col *= 1.0-AO(p, sceneDist(p), 40.0, 1.0);
            // light
            col += drawLight(p, light1Pos, light1Col, dist, 10.0, 6.0);
            // col += drawLight(p, light2Pos, light2Col, dist, 200.0, 8.0);
            // col += drawLight(p, light3Pos, light3Col, dist, 300.0, 12.0);
            // shape fill
            // col = mix(col, vec4(1.0, 0.4, 0.0, 1.0), fillMask(dist));
            // shape outline
            // col = mix(col, vec4(0.1, 0.1, 0.1, 1.0), innerBorderMask(dist, 1.5));

            gl_FragColor = texture2D(texture, pos) * clamp(col, 0.0, 1.0);
        }
    )";
    const GLchar* frag_src = R"(
    )";

    // shader program
    GlhCreateProgram(vert_src, frag_src, NUM_ATTRIBUTES,
                    (const GLchar**)&attr_name[0], attr_location, &program_);
    RET_CHECK(program_) << "Problem initializing the program.";
    
    // texture_ = glGetUniformLocation(program_, "texture");
    // RET_CHECK_NE(texture_, -1) << "Failed to find `texture` uniform!";

    return absl::OkStatus();
}

absl::Status GlShaderCalculator::GlBind() {
    return absl::OkStatus();
}

absl::Status GlShaderCalculator::GlRenderDrawArrays(GLint& positionLocation) {
    //1. enable the attribute vertex location
    glEnableVertexAttribArray(positionLocation);

    //2. Start the rendering process
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    //3. Disable the attribute vertex location
    glDisableVertexAttribArray(positionLocation);

    return absl::OkStatus();
}

absl::Status GlShaderCalculator::GlRenderDrawElements(GLint& positionLocation) {
    //1. enable the attribute vertex location
    glEnableVertexAttribArray(positionLocation);

    //2. Start the rendering process
    // glDrawElements(GL_TRIANGLES, sizeof(cubeIndex)/4, GL_UNSIGNED_INT, (void*)0);

    //3. Disable the attribute vertex location
    glDisableVertexAttribArray(positionLocation);

    return absl::OkStatus();
}

absl::Status GlShaderCalculator::GlRender(const GlTexture& src, const GlTexture& dst, 
                                          double timestamp) {

    // program
    glUseProgram(program_);
    
    // Set up the GL state.
    // glEnable(GL_BLEND);
    // glFrontFace(GL_CCW);
    // glBlendFunc(GL_ONE, GL_ZERO);
    // glDisable(GL_DEPTH_TEST);
    // glDepthMask(GL_FALSE);

    //Vertex data of character
    // float vertexData[36]={1.0,0.4,0.9,1.0,....};
    static const GLfloat vVertices[] = {  -1.0f,  -1.0f, 0.0f,   // bottom left
                                           1.0f,  -1.0f, 0.0f,   // bottom right
                                          -1.0f,   1.0f, 0.0f,   // top left
                                           1.0f,   1.0f, 0.0f }; // top right 

    static const GLfloat tVertices[] = {  0.0f, 0.0f, // bottom left
                                          1.0f, 0.0f, // bottom right
                                          0.0f, 1.0f, // top left
                                          1.0f, 1.0f, // top right 
                                       };
    
    //1. Create a buffer
    GLuint vbo[2];
    glGenBuffers(2, vbo);
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // parameters
    GLint millis_ = glGetUniformLocation(program_, "millis");
    glUniform1f(millis_, timestamp);

    //2. Bind a buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

    //3. Load data in the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices), vVertices, GL_STATIC_DRAW);

    //4. Get the location of the shader attribute called "position". Assume positionLocation is a global GLuint variable
    GLint positionLocation = glGetAttribLocation(program_, "position");

    //5. Get Location of uniforms
    // GLint modelViewProjectionUniformLocation = glGetUniformLocation(program_, "modelViewProjectionMatrix");

    //6. Enable the attribute location
    glEnableVertexAttribArray(positionLocation);

    //7. Link the buffer data to the shader attribute locations.
    glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)0);

    // vbo 1
    GLint textureLocation = glGetAttribLocation(program_, "tex_coord");
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tVertices), tVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(textureLocation);
    glVertexAttribPointer(textureLocation, 2, GL_FLOAT, 0, 0, nullptr);

    // texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(src.target(), src.name());
    glUniform1i(texture_, 1);


    MP_RETURN_IF_ERROR(GlRenderDrawArrays(positionLocation));

    // Unbind textures and uniforms.
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(src.target(), 0);

    // clean up
    glDisableVertexAttribArray(positionLocation);
    glDisableVertexAttribArray(textureLocation);
    //11. Delete buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(2, vbo);

    glUseProgram(0);
    glFlush();

    return absl::OkStatus();
}

absl::Status GlShaderCalculator::GlTeardown() {
    if (program_) {
        glDeleteProgram(program_);
        program_ = 0;
    }
    return absl::OkStatus();
}


} // mediapipe