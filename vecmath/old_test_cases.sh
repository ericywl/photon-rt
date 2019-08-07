#!/bin/sh
rm out/*.tga
rm out/*.bmp

build/asg5 -input data/scene/scene01_plane.txt  -size 200 200 -output out/1.bmp
build/asg5 -input data/scene/scene02_cube.txt  -size 200 200 -output out/2.bmp
build/asg5 -input data/scene/scene03_sphere.txt  -size 200 200 -output out/3.bmp
build/asg5 -input data/scene/scene04_axes.txt  -size 200 200 -output out/4.bmp
build/asg5 -input data/scene/scene05_bunny_200.txt  -size 200 200 -output out/5.bmp
build/asg5 -input data/scene/scene06_bunny_1k.txt  -size 200 200 -output out/6.bmp
build/asg5 -input data/scene/scene07_shine.txt  -size 200 200 -output out/7.bmp
build/asg5 -input data/scene/scene08_c.txt -size 200 200 -output out/8.bmp
build/asg5 -input data/scene/scene09_s.txt -size 200 200 -output out/9.bmp

if [ true = false ]; then
    build/asg5 -input data/scene/scene01_plane.txt  -size 200 200 -output out/1b.bmp -blinn
    build/asg5 -input data/scene/scene02_cube.txt  -size 200 200 -output out/2b.bmp -blinn
    build/asg5 -input data/scene/scene03_sphere.txt  -size 200 200 -output out/3b.bmp -blinn
    build/asg5 -input data/scene/scene04_axes.txt  -size 200 200 -output out/4b.bmp -blinn
    build/asg5 -input data/scene/scene05_bunny_200.txt  -size 200 200 -output out/5b.bmp -blinn
    build/asg5 -input data/scene/scene06_bunny_1k.txt  -size 200 200 -output out/6b.bmp -blinn
    build/asg5 -input data/scene/scene07_shine.txt  -size 200 200 -output out/7b.bmp -blinn
    build/asg5 -input data/scene/scene08_c.txt -size 200 200 -output out/8b.bmp -blinn
    build/asg5 -input data/scene/scene09_s.txt -size 200 200 -output out/9b.bmp -blinn
fi
