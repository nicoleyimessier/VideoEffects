# VideoEffects

C++ and glsl code that manipulates video textures to create a glitch effect. 

These classes can be used for two different use cases: 
* create a tool to manipulate different videos, showing each filter pass hooked up to a variable sliders; useful for visual prototyping
* use in software for generative video effects

![Tool](https://github.com/nicoleyimessier/VideoEffects/blob/master/documentation/imgs/tools.png)

See [videos](https://github.com/nicoleyimessier/VideoEffects/tree/master/documentation/vids) for example of output. 

## Class Overview

This code was written in C++ (openFrameworks) and glsl. 

A list of relavaent class are below: 
* [VideoEffects](https://github.com/nicoleyimessier/VideoEffects/tree/master/src/VideoEffects): c++ class that uses shaders to create a mutli-pass generative effect on video textures 
* [Video](https://github.com/nicoleyimessier/VideoEffects/tree/master/src/Video): video player wrapper for HAP or WMF 
* [Shader](https://github.com/nicoleyimessier/VideoEffects/tree/master/src/shaders): various shaders for different effects including displacement, blur, and more. 


## License 

The code in this repository is available under the [MIT License](https://en.wikipedia.org/wiki/MIT_License).

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
