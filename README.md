# img2sky :stars:
- img2sky is a skybox builder tool, as seen in HomeWorld 2 .HOD files. 
- img2sky reads texture files and outputs portable vertex-color plane/sphere meshes, that can be later reexported/transformed/quantized/binarized as desired.
- That said, resulting vertex-color meshes are the background of your skybox. Once rendered, you are supposed to render shiny billboards and/or submeshes on top of them (like stars, galaxies, nebulas, suns, clouds, zeppelins and flying cows).

## Download
[v0.0.0 (win32 pre-built binary)](img2sky.exe)

## Usage and options
```c++
img2sky: RELEASE. Compiled on Mar 25 2015 - https://github.com/r-lyeh/img2sky

Usage:  img2sky [options] input.img [...]
                -q,--quality integer   Quality polygon setting [0..100] (lowest..highest) (default: 90)
                -v,--vertices integer  Specify maximum number of vertices (default: disabled)
                -f,--fast              Disable stripfication (default: enabled)
                -p,--plane             Create plane mesh (default: sphere)

        img2sky reads .bmp, .dds, .gif, .hdr, .jpg, .pic, .pkm, .png, .psd, .pvr, .svg, .tga, .webp, .pnm, .pug texture files.
        img2sky writes .ply mesh files.

Quality and number of vertices should be mutually exclusive options. Ie, you cannot specify both at same time.
If you are the big/expensive meshes guy, use any of { large textures, high quality setting and/or high number of vertex settings };
else use any of { small textures, low quality setting and/or low number of vertex settings }.
```

## Sample showcase
| input preview |
|:-----:|
|![image](images/image.webp.png "512x256 texture, 4KiB webp (q=90)")<br/>512x256 texture, 4KiB webp (q=90)|

|output plane (wireframe)|output details|
|:-----:|:-----:|
|![image](https://raw.github.com/r-lyeh/img2sky/master/images/ply-e20-v5338-f10633-plane-wf.png "5338 vertices, 10633 polys, 393 KiB ply")|5338 vertices, 10633 polys, 308 KiB ply|
|![image](https://raw.github.com/r-lyeh/img2sky/master/images/ply-e15-v8935-f17804-plane-wf.png "8935 vertices, 17804 polys, 660 KiB ply")|8935 vertices, 17804 polys, 517 KiB ply|
|![image](https://raw.github.com/r-lyeh/img2sky/master/images/ply-e05-v30055-f59910-plane-wf.png "30055 vertices, 59910 polys, 2280 KiB ply")|30055 vertices, 59910 polys, 1870 KiB ply|

|output sphere|output sphere (wireframe)|output details|
|:-----:|:-----:|:-----:|
|![image](https://raw.github.com/r-lyeh/img2sky/master/images/ply-e20-v5399-f10633.png "5399 vertices, 10633 polys, 393 KiB ply")|![image](https://raw.github.com/r-lyeh/img2sky/master/images/ply-e20-v5399-f10633-wf.png "5399 vertices, 10633 polys, 393 KiB ply")|5399 vertices, 10633 polys, 393 KiB ply|
|![image](https://raw.github.com/r-lyeh/img2sky/master/images/ply-e15-v9055-f17804.png "9055 vertices, 17804 polys, 660 KiB ply")|![image](https://raw.github.com/r-lyeh/img2sky/master/images/ply-e15-v9055-f17804-wf.png "9055 vertices, 17804 polys, 660 KiB ply")|9055 vertices, 17804 polys, 660 KiB ply|
|![image](https://raw.github.com/r-lyeh/img2sky/master/images/ply-e05-v30228-f59910.png "30228 vertices, 59910 polys, 2280 KiB ply")|![image](https://raw.github.com/r-lyeh/img2sky/master/images/ply-e05-v30228-f59910-wf.png "30228 vertices, 59910 polys, 2280 KiB ply")|30228 vertices, 59910 polys, 2280 KiB ply|

## Redistribution
- `echo windows && cl src\*.c* /Feimg2sky.exe /Ox /Oy /DNDEBUG /MT /EHsc /link setargv.obj`
- `echo others  && gcc -o img2sky src/*.c* -O2`

## Details
- Input texture is read, processed and mapped.
- Vertices are greedy grouped, then projected to a plane (or to a sphere).
- Mesh is written to disk as .PLY mesh format (vertex-color friendly format).
- Additional tech information can be found on following links:
  - [Homeworld 2: Backgrounds Tech (I)](http://simonschreibt.de/gat/homeworld-2-backgrounds)
  - [Homeworld 2: Backgrounds Tech (II)](http://simonschreibt.de/gat/homeworld-2-backgrounds-tech/)

## Licenses
- Original code [hw2bgbuilder v1.3 by 4E534B, Michael Garland and evillejedi](http://forums.relicnews.com/showthread.php?148734-Homeworld2-Background-Builder-v1-3), unknown license.
- tri_stripper.h/.cpp by Tanguy Fautré, ZLIB/libPNG licensed.
- Additional code by r-lyeh, Public Domain.

## Credits
- Samples images taken from [http://walter-nest.deviantart.com/gallery/44007823/Homeworld-2](Walter Nest's hw2 deviantart library).
