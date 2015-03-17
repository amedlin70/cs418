MP3 documentation

My implementation of mp3 takes 3 arguments, these are 1. a vertex shader, 2. a fragment shader, and 3. an object file
usage: MP3 phong.vs phong.fs teapot_0.obj

Inputs to the program.

s: turns on/off the fragment/vertex shader
t: turns on/off the primary texture. This will display only the sphere_map on the object
b: turns on/off the background
l: turns on/off the rotation of the light (from the sample code provided by instructor)
o: turns on/off the rotation of the object (from the sample code provided by instructor)

My program uses the vertex and fragment shaders to implement the phong lighting.  
This code is modified from the example code provided by the instructor.  
It works fine with one texture, but I had problems getting it to work while using the sphere-map.  To get around this, when viewing the sphere-map on the object, I deactivate the phong fragment and vertex shaders and use the default ones instead.  This program loads an object from file, computes the normals and texture locations, and then displays that object.  It can be displayed with a primary texture only, a sphere-map texture only, or a combination of primary texture with sphere-map environment mapping.
