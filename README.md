ray_tracer
==========

Basic ray tracing program using OpenGL

Still a work in progress.  Currently calculates diffuse lighting and shadows.  Assumes all light sources are white light.  Specular lighting, refraction, etc to come later.  

COMPILING / RUNNING THE APPLICATION
1. In the raytracer director, issue the command "make".  This will compile the code and create the executable called "raytracer"
2. To run, call this executable and add the parameter of the text file to be parsed and used.  A second argument can be passed if you wish to save the rendered image.  In this case specify the desired output file.  

The input file scheme works with three types of objects: spheres and triangles.  The operate with the following formats:

sphere
  position (3 floats)
  radius (1 float)
  diffuse color (3 floats)
  specular color (3 floats)
  shininess (1 float)

triangle
  //the following, repeated three times (once for every vertex)
  position (3 floats)
  normal (3 floats)
  diffuse color (3 floats)
  specular color (3 floats)
  shininess (1 float)

light
  position (3 floats)
  color (3 floats)

Also note the first line of every file is the total number of objects in the scene.  

For coordinates, the scene is set up such that the view is at the origin looking in the negative z direction.  

An example is in the screenfile.txt file provided.