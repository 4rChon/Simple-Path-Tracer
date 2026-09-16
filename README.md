# CPS5124-ImageOrder
An image order implementation for CPS5124.

The image below is rendered based on the definition included in this repo.

![image](https://i.imgur.com/goO1aBQ.png "4096 samples per pixel, 512x512")

### Definition File Format
```
{
  "cameras" : [...]
  "materials" : [...],
  "shapes" : [...],
  "lights" : [...],
  "scene" : {...}
}
```

#### Cameras
Cameras can be lens-based or pinhole cameras:

```
"id" : "camera",
"type" : "lens-based",
"focal distance" : 15.5,
"alpha" : 0.4,
"fov" : 45,
"aspect" : 1.0,
"distance" : 5.0,
"position" : [4, -2, -15],
"target": [0, 0, 0]
```


```
"id" : "camera",
"type" : "pinhole",
"fov" : 45,
"aspect" : 1.0,
"distance" : 5.0,
"position" : [4, -2, -15],
"target": [0, 0, 0]
```
#### Materials
There are 4 explicit material types which contain different BxDF properties.

"diffuse" - diffuse

"specular reflection" - specular and reflective

"glossy reflection" - specular, reflective, and glossy

"fresnel dielectric" - specular, reflective, and transmissive

```
{
  "id" : "red diffuse",
  "type" : "diffuse",
  "rho" : [0.5, 0.25, 0.25]
}, {
  "id" : "glossy",
  "type" : "glossy reflection",
  "rho" : [0.9, 0.9, 0.9],
  "roughness" : 0.1
}, {
  "id" : "very glossy",
  "type" : "glossy reflection",
  "rho" : [0.9, 0.9, 0.9],
  "roughness" : 0.7
}, {
  "id" : "fresnel dielectric",
  "type" : "fresnel dielectric",
  "rhoR" : [0.9, 0.9, 0.9],
  "rhoT" : [0.9, 0.9, 0.9],
  "eta" : 1.5
```

#### Shapes

There is only one type of shape so far---a sphere. I don't intend on adding more shapes any time soon, but spheres should be enough for a typical Cornell box set up (make the sphere walls really big).

```
{
    "id" : "sphere 01",
    "type" : "sphere",
    "centre" : [-3, 0, -3],
    "radius" : 1
}
```

#### Lights
Lights can be area lights or point lights. Area lights are attached to shapes specified in the "shape" attribute:

```
{
    "id" : "pink",
    "type" : "area",
    "shape" : "pink light",
    "power" : [40, 10, 30]
}
```

Point lights require a position:

```
{
  "id" : "blue",
  "type": "point",
  "position": [0, 0, 0],
  "power" : [10, 10, 40]
}
```

#### Scene

A scene is made up of the camera, lights, primitives, and the renderer.

```
{
  "camera" : "lens-based",
  "lights" : ["yellow", "pink"],
  "primitives" : [...],
  "renderer" : {...}
}
```

#### Primitives

A primitive brings together all the components we've specified so far. Primitives can be emissive or geometric. Emissives require an extra "light" attribute.

```
{
  "id": "yellow",
  "type" : "emissive",
  "shape" : sphere_01",
  "light" : "pink",
  "material" : "red diffuse"
}
```

```
{
  "id" : "sphere",
  "type" : "geometric",
  "shape" : "sphere_02",
  "material" : "fresnel dielectric"
}
```

#### Renderers

Renderers can be path tracers (PT) or ray tracers (WRT).

```
{
  "type" : "PT",
  "dimensions" : [100, 100],
  "samples" : 16,
  "depth" : 5,
  "output" : "../output/custom.ppm"
}
```

### Command Line Arguments
```--batch```: Render all files in the specified ```load_path``` directory if true. Render single input file specified by ```input_file``` otherwise. Default: true

```--load_path```: Relative folder containing scene definitions. Default: "./definitions/"

```--save_path```: Relative save folder location. Default: "./renders/"

```--input_file```: Name of definition file. Unused if ```batch``` is true. Default: "in.json"

```--output_file```: Name of output file. Unused if ```batch``` is true. Default: "out.ppm"

```--tonemapper```: "sigmoid" or "linear" tonemapper. Default: "sigmoid"

```--progress```: Output X/Y progress updates every 1000 pixels where X is current pixel and Y is total number of pixels. Default: false

```--stratified_sampling```: "uniform" or "stratified" sampling strategy. Default: "stratified"

### Nuget Packages:
glm 1.0.3, nlohmann.json 3.12.0

### Other Packages:
gflags 2.3.0

