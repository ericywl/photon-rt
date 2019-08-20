# Ray Tracer

### What kind of arguments (if any) that you program needs?

- The program takes in some necessary arguments and some optional.

  ```
  Usage: build/asg5 -input <INPUT_SCENE> -output <OUTPUT_IMG> -size <WIDTH> <HEIGHT> ...
  ```

  | Argument               | Description                                                                          |
  | ---------------------- | ------------------------------------------------------------------------------------ |
  | -input <INPUT_SCENE>   | Takes in text file that describes the scene to be rendered                           |
  | -output <OUTPUT_IMG>   | Takes in the filename that the rendered image will be saved as                       |
  | -size <WIDTH> <HEIGHT> | takes in width and height integers describing the dimensions of the image            |
  | -normals <NORMAL_IMG>  | Takes in filename to save normals for visualization, mainly for debugging (optional) |
  | -gamma <GAMMA>         | Takes in a float that is used for gamma correction (optional)                        |
  | -blinn                 | A toggle to use Blinn-Phong specular shading instead (optional)                      |

TODO: Add more arguments for ray tracer
