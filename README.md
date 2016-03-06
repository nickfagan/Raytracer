Raytracer
=========

A raytracer I intially created for my fourth year computer graphics course in University. Since then I have made a number of changes such as utilizing threads to reduce run time, switching from using lua to json to define scenes, and cleaning up / commenting alot of code.

<b>Features:</b>
- Specular materials
- Reflective materials
- Soft shadows
- Anti-aliasing
- Multithreading

###Running the raytracer

`./rt <scene_json_file> <output_file_name> <width> <height>`

**scene_json_file** - The json file that defineds a scene. There is an example json file called "room.json" that you can use.<br>
**output_file_name** - The name of the output file. (eg. "image.png")<br>
**width** - The width of the output image.<br>
**height** - The height of the output image.<br>
