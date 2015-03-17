To run the code, simply compile the source code, or run mp4.exe.  There are no command line inputs to the program, the model name is hardcoded in as "i.obj".  

Useful keys:
	s - Performs Catmull-Clark subdivision
	w - Toggles wireframe on/off

This project uses Catmull-Clark subdivision on an obj file with the following format: vertex: v x y z  face: f v1 v2 v3 v4

This .obj file is stored using a half edge data structure, which is useful for the subdivision.

There is no camera movement.  I could not get the b-spline code working, 

The subdivision can go 3 deep, but the normals at the center of the I get messed up for some reason, did not trouble shoot as it appeared correct for the first 2 subdivisions.