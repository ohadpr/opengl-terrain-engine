Terrain Engine

I started working on a terrain engine around October 2000. 
I was deeply moved by screenshots from the game black and white :)
My intention and main goal was to create something visually stunning. 
This meant writing a terrain texture-generator, and working hard to find good textures and blending modes.
The outcome is an engine that hold a (supposedly) 1km by 1km piece of terrain with very high accuracy. 
There is no triangle-cutting algorithm employed, simple brute-force. I use a large terrain texture and a tiling detail texture. There's a skybox and the terrain reflects on the water.

I used OpenGL for this one, employing the OGLC skeleton to avoid most of the opengl-win32 business.

You can move through this world using descent keys (a,z, keypad). use 'w' to switch to wire-frame and 'd' to render without the detail texture.

ohadpr.com