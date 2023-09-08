# mymediapipe

## First: Simple IO

Break mediapipe process into simple IO process

Custom:

  * Simple main
  * Simple graph


Build
```
scripts\build_simple_io.sh
```

Run
```
scripts\run_simple_io.sh
```

## Simple OpenGL

Change medipipe OpenCV -> OpenGL display

Build
```
scripts\build_simple_opengl.sh
```

Run
```
scripts\run_simple_opengl.sh
```

## Simple OpenGL LightSaber

Insert LightSaber Shader into OpenGL node

Build
```
```

Run
```
```


## Unit test 

Include `ut\README.md`

## Next

###

The .uuu format is a simple custom binary format designed to be trivially rendered using OpenGL, so sequenced .obj animations/files (or any other asset format) will need to be parsed into this format in order to be used by MediaPipe. The LoadAnimation and LoadAnimationAndroid functions in gl_animation_overlay_calculator.cc can be used as reference for how the .uuu format is expected to be loaded. In brief, the format expects an arbitrary number of animation frames, concatenated directly together, with each animation frame looking like:
HEADER
VERTICES
TEXTURE_COORDS
INDICES
The header consists of 3 int32 lengths, the sizes of the vertex data, the texcoord data, and the index data, respectively. Let us call those N1, N2, and N3. Then we expect N1 float32's for vertex information (x1,y1,z1,x2,y2,z2,etc.), followed by N2 float32's for texcoord information (u1,v1,u2,v2,u3,v3,etc.), followed by N3 shorts/int16's for triangle indices (a1,b1,c1,a2,b2,c2,etc.). This is precisely the data that a glDrawElements call expects, when drawing triangles.