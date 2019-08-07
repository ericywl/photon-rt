#!/bin/sh
rm out/*.tga
rm out/*.bmp

build/asg5 -input data/scene/scene06_bunny_1k.txt  -size 300 300 -output out/6.bmp\
   -shadows -bounces 4 -aa
build/asg5 -input data/scene/scene10_sphere.txt  -size 300 300 -output out/10.bmp\
   -shadows -bounces 4 -aa
build/asg5 -input data/scene/scene11_cube.txt  -size 300 300 -output out/11.bmp\
 -shadows -bounces 4 -aa
build/asg5 -input data/scene/scene12_vase.txt  -size 300 300 -output out/12.bmp\
 -shadows -bounces 4 -aa

