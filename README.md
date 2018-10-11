# ORM - Object Recognition Module for FSAE driverless  
This is basic software intended for testing the usage of yolov3-tiny for object detection in FSAE driverless.  
Any notes and suggestions are welcome!

<iframe width="560" height="315" src="https://www.youtube.com/embed/_1xGcDi-pW8" frameborder="0" allow="autoplay; encrypted-media" allowfullscreen></iframe>

###Important notes
Before using please note that:
- The weights are currently trained on a minimal dataset (200 images) with only 1 class for orange cones.  
- The intention is to expand to all relevent objects for FSAE driverless with a larger dataset for each class.  
- Detection and tracking should improve with a larger dataset.  
- I assume tracking would work better with a natively high fps video.
- This software is currently not implemented in an FSAE car and is used mainly as a "playground" for testing at the moment. 

###Regarding the cfg file
- While training the resolution was set to 608x608.
- While in use the resoution was set to 416x416.
- Increasing the resolution will improve accuracy but reduce speed,

###Performance
- CPU: ~18 fps (Intel i7-4710HQ).
- GPU: ~80 (!) fps (Nvidia Geforce 970M).

##Setup and prerequisites
Please read the code and make sure you understand it before building and using!

** Setup **
- The darknet shared library was compiled with the use of CUDA, so if no gpu is used please [recompile darknet](https://github.com/AlexeyAB/darknet#how-to-compile-on-linux)  
and replace libdarknet.so (re-name the resulting darknet.so file and simply replace the one used here).

- If you do have an nvidia gpu install the [CUDA toolkit](https://developer.nvidia.com/cuda-downloads) and [cuDNN](https://developer.nvidia.com/cudnn).

- Make sure you have [OpenCV](https://docs.opencv.org/master/df/d65/tutorial_table_of_content_introduction.html) installed and properly linked in your build environment.

- Clone this repostiory `git clone --recursive https://github.com/Galzai/ORM`
- Add libdarknet.so and the cuda libraries to your library path:  
 `export LD_LIBRARY_PATH=/usr/local/cuda-10.0/lib64:$LD_LIBRARY_PATH`  
 `export LD_LIBRARY_PATH=/home/"your-username"/ORM/Debug:$LD_LIBRARY_PATH `
- Before building make sure the path to the video file in the code is correct (Can easily be modified to use a camera if you know some basic OpenCV usage).  
- Build:  
`cd /home/"your-username"/ORM/Debug`   
`make`

** Usage **

- Enter `./ORM`


