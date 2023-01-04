## :star2: White Border Removal Image Cropping Tool

* Remove the unneccesary white borders of the image with the extensions of .jpg, .jpeg, .png.

## :robot: Brief Description

This tool is designed specifically to remove white borders of the lossy compressed (e.g jpeg) images. 

Since those white borders are also affected by noise, they could be hardly filtered by just using contour detection after binary threshololding. 

A k mean clustering algorithm is implemented after the above step to partition those columns and rows into two main categories, i.e. possible white borders and impossible, which then returns the rectangle edges' coordinates to crop the image, with the following result:


| Original | Threshold Filtering | Threshold Filtering with K-mean Clustering |
| ------------- | ------------- | ------------- |
| {[c]<img src="result/IMG_0001_original.jpg" width=100% height=100%>} | <img src="result/IMG_0001_no_clustering.jpg" width=100% height=100%> | <img src="result/IMG_0001_with_clustering.jpg" width=100% height=100%> |

With the extra clustering process involved, the width of the white border is significantly reduced. 

However, with careful inspection of the third image, a column of white border (one pixel wide) still exists on the left side due to the weakness of the simple k mean clustering that categorizes the similarities based on the Euclidean distance.

## 	:toolbox: Getting Started

### :bangbang: Prerequisites

* Designed for Windows OS (tested on Windows 10, x64).
* Requires OpenCV library (tested on 4.x version):
    * DLL library = opencv_worldxxxd.dll (debug), opencv_worldxxx.dll (release), which xxx = build number.
* Optional (for building purpose)
    * Requires Visual Studio (tested on 2019 version and above).

### :gear: Building (Skip this if using the releases attached)

* Open the Visual Studio Project through the .sln file.
* Specify the OpenCV libaries through configurations in Visual Studio project properties ([tutorial](https://docs.opencv.org/4.x/dd/d6e/tutorial_windows_visual_studio_opencv.html)):
    * Platform = x64
    * Additional Include Directories (in C/C++,General) = $(OPENCV_DIR)\build\include
    * Additional Library Directories (in Linker, General) = $(OPENCV_DIR)\build\x64\vc15\lib
    * Additional Dependencies (in Linker, Input) = opencv_worldxxxd.lib (debug), opencv_worldxxx.lib(release), which xxx = build number.
* Build solution in release mode (debug mode will produce test results).

### :running: Executing program

* Command Prompt
```
ImageCropping.exe -I IMG_0001.jpg -F y -P 95
```

* Contains three required parameters to run the program, i.e.
    * -I={FILENAME} , --input={FILENAME}

        The file name of input image to be cropped. (Supports **ANSI characters** only)

    * -F={y|n} , --force={y|n} 

        Forcing (y) the output image to compress to .jpg extension, else (n) will output the image with the same extension as the input image.

        **Note**: The output cropped image with .jpg or .jpeg extensions is **not** a lossless crop.

    * -P=# , --percentage=#

        The minimum percentage,# (0-100 in integer) of image to be retained by user in both x and y directions after binary threshold filtering, the recommended value = 95. 
        
        This setting is only needed to modify when excessive trimming is done after clustering process.
        
        e.g. if **percentage=75** is set by user, for the image cropped after binary threshold filtering,

        | % of image would be retained after clustering in **x-direction** | % of image would be retained after clustering in **y-direction** | Output |
        | :-------------: | :-------------: | :-------------: |
        | ≤75% | ≤75% | Image is **not** further cropped. |
        | ≤75% | >75% | Image is cropped further in **y** direction only. |
        | >75% | ≤75% | Image is cropped further in **x** direction only. |
        | >75% | >75% | Image is cropped further in **x** and **y** directions.  |


## :scroll: Version History

* 1.0.0
    * Initial Release

## :warning: License

This project is licensed under the MIT license - see the [LICENSE.md](LICENSE.md) file for details
