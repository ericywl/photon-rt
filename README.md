# Indirect illumination using Custom Photon Mapping

### Build from source and run the program
* `cmake . && make`. This should produce an executable `ray-tracer`.

### Example arguments
* `./ray-tracer -input data/scene/cornell.txt -output render.bmp -size 800 600 -shadows -rBounces 10 -pBounces 10 -photons 50000 -nearest 10`

### What kind of arguments (if any) that you program needs?

- The program takes in some necessary arguments and some optional.

  ```
  Usage: ./ray-tracer -input <INPUT_SCENE> -output <OUTPUT_IMG> -size <WIDTH> <HEIGHT> ...
  ```

  | Argument               | Description                                                                          |
  | ---------------------- | ------------------------------------------------------------------------------------ |
  | -input <INPUT_SCENE>   | Takes in text file that describes the scene to be rendered                           |
  | -output <OUTPUT_IMG>   | Takes in the filename that the rendered image will be saved as                       |
  | -size <WIDTH> <HEIGHT> | takes in width and height integers describing the dimensions of the image            |
  | -normals <NORMAL_IMG>  | Takes in filename to save normals for visualization, mainly for debugging (optional) |
  | -gamma <GAMMA>         | Takes in a float that is used for gamma correction (optional)                        |
  | -blinn                 | A toggle to use Blinn-Phong specular shading instead (optional)                      |
  | -shadows               | A toggle to render with shadows (optional)                                           |
  | -aa                    | A toggle to render with anti-aliasing (optional)                                     |
  | -secRays               | (int) The number of secondary rays in the Monte Carlo tracer   (default: 5)          |
  | -rBounces              | (int) The maximum number of bounces for the Monte Carlo tracer (default: 3)          |
  | -pBounces              | (int) The maximum number of bounces for the photon tracer      (default: 3)          |
  | -photons               | (int) The number of photons to fire                            (default: 10000)      |  
  | -nearest               | (int) The number of nearest neighbors for the kd-tree search   (default: 5)          |
