# img2sky :waxing_crescent_moon:
- img2sky is a vertex-color mesh builder tool for skyboxes, as seen in HomeWorld 2 .HOD files. 
- img2sky reads texture files and outputs portable vertex-color plane/sphere meshes, that can be later reexported/transformed/quantized/binarized as desired.
- img2sky creates meshes that are more vertex dense on bright areas and less dense in obscure areas, according to human eye perception.
- That said, resulting vertex-color meshes are good as a background of your skybox. Once rendered, you are supposed to top it with shiny billboards, submeshes, stars, galaxies, nebulas, suns, clouds, zeppelins and flying cows.

## Download
[v0.0.1 (win32 pre-built binary)](img2sky.exe)

## Usage and options
```c++
~/prj/img2sky> img2sky
img2sky: img2sky 0.0.1 (RELEASE). Compiled on Mar 26 2015 - https://github.com/r-lyeh/img2sky

Usage:
    img2sky [options] image.img [...]
        -q,--quality float     Quality for vertex density [0..100] (lowest..highest) (default: 50.00)
        -v,--vertices integer  Specify maximum number of vertices [4..N] (default: disabled)
        -f,--fast              Disable triangle stripification (default: enabled)
        -s,--sphere            Create sphere mesh (default: plane)

    img2sky reads .bmp, .dds, .gif, .hdr, .jpg, .pic, .pkm, .png, .psd, .pvr, .svg, .tga, .webp, .pnm, .pug texture files.
    img2sky writes .ply mesh files.

Quality and number of vertices should be mutually exclusive options. You can specify both at same time, but does not make much sense at all.
And if you are the expensive/smooth meshes guy kind, then use larger textures, increase quality and/or number of vertices.

~/prj/img2sky> img2sky --plane --quality 12.5 images/*.webp
[ OK ] images/image.webp -> images/image.webp.ply (plane) (error-threshold: 4.15888) (max-error: 4.13793) (vertices: 1659) (tris: 3298) (92 KiB)
```

## Sample showcase
| input preview |
|:-----:|
|![image](images/image.webp.png "512x256 texture 4 KiB webp (q=90)")<br/>512x256 texture<br/>4 KiB .webp (q=90)|

| output preview |
|:-----:|
|![image](https://raw.github.com/r-lyeh/img2sky/master/images/ply-e20-v5338-f10633-plane-wf.png "5338 verts, 10633 faces, 393 KiB .ply")<br/>5338 verts, 10633 faces<br/>308 KiB .ply|
|![image](https://raw.github.com/r-lyeh/img2sky/master/images/ply-e15-v8935-f17804-plane-wf.png "8935 verts, 17804 faces, 660 KiB .ply")<br/>8935 verts, 17804 faces<br/>517 KiB .ply|
|![image](https://raw.github.com/r-lyeh/img2sky/master/images/ply-e05-v30055-f59910-plane-wf.png "30055 verts, 59910 faces, 2280 KiB .ply")<br/>30055 verts, 59910 faces<br/>1870 KiB .ply|
|![image](https://raw.github.com/r-lyeh/img2sky/master/images/ply-e20-v5399-f10633-wf.png "5399 verts, 10633 faces, 393 KiB .ply")<br/>5399 verts, 10633 faces<br/>393 KiB .ply|
|![image](https://raw.github.com/r-lyeh/img2sky/master/images/ply-e15-v9055-f17804-wf.png "9055 verts, 17804 faces, 660 KiB .ply")<br/>9055 verts, 17804 faces<br/>660 KiB .ply|
|![image](https://raw.github.com/r-lyeh/img2sky/master/images/ply-e05-v30228-f59910-wf.png "30228 verts, 59910 faces, 2280 KiB .ply")<br/>30228 verts, 59910 faces<br/>2280 KiB .ply|

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
- Original [hw2bgbuilder code v1.3 by 4E534B, Michael Garland and evillejedi](http://forums.relicnews.com/showthread.php?148734-Homeworld2-Background-Builder-v1-3), unknown license.
- tri_stripper.h/.cpp by Tanguy Fautré, ZLIB/libPNG licensed.
- Additional code by r-lyeh, Public Domain.

## Credits
- Sample images taken from [http://walter-nest.deviantart.com/gallery/44007823/Homeworld-2](Walter Nest's hw2 deviantart library).
