
# MineralApp icons

Original SVG files, created with Inkscape:

 - `mineralapp-hires.svg` for high resolution images
 - `mineralapp-lowres.svg` for low resolution images

Just make sure they are square images.

From the SVG export, always from Inkscape, images at high resolution for both
hires and lowres:

 - `mineralapp-hires.png`
 - `mineralapp-lowres.png`

From the PNG, create a series of smaller PNG images at different resolutions.
The lowres is used for 16px and 32px, hires for all the other:

 - `convert mineralapp-lowres.png -resize 16x16 mineralapp-16.png`
 - `convert mineralapp-lowres.png -resize 32x32 mineralapp-32.png`
 - `convert mineralapp-hires.png -resize 48x48 mineralapp-64.png`
 - `convert mineralapp-hires.png -resize 64x64 mineralapp-64.png`
 - `convert mineralapp-hires.png -resize 128x128 mineralapp-128.png`
 - `convert mineralapp-hires.png -resize 196x196 mineralapp-196.png`
 - `convert mineralapp-hires.png -resize 256x256 mineralapp-256.png`
 - `convert mineralapp-hires.png -resize 512x512 mineralapp-512.png`

For MacOS you need a icns file:

 - `sips -s format icns mineralapp-512.png --out mineralapp.icns`

For Windows you need a ico file. To generate this one open all
mineralapp-yyy.png images (from 16px to 512px) on GIMP as layers, and export as
ico. Do not use compression.


