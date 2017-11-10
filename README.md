Project 3 - Illumination
------------------------
Code to raycast mathematical primitives based on a scene input file
into a pixel buffer where the pixels are colored based on a shading model.
Then pixel buffer is written to a PPM3 formatted file.

Usage
------------------------
To use this program, the JSON input file must be written with the camera object first, then any sphere, plane, or light objects thereafter. 
Then, go to the directory in command line and then call make.
Then execute the raycast exectuable with arguments (int in pixels) height, (int in pixels) width, (string) input_file_name, (string) output_file_name.

Known Issues
------------------------
1) Seg faults after calling make
2) Even though the above is true, this is probably still a problem: 
	does not show multiple spheres at the same time if they are on the same z coordinate. They override each other for some reason.
3) Does not find the diffuse and specular reflections
4) Accepts when files have something like ",]["; I need to change the code to stop using strtok()


Date and Author
-----------------------
10/27/17
Leia Trice - lat222@nau.edu