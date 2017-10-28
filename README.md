## Tesseract OCR Sample (Visual Studio) with Leptonica Preprocessing

This guides you through including the Tesseract OCR 3.02 API and Leptonica dll into a Visual Studio C++ Project, and  provides a sample file which takes an image path to preprocess and OCR. The preprocessing script in Leptonica converts the input image into black and white book-like text and substantially improves accuracy.

### Setup

To include this in your own projects, you will need to reference the header files and lib and copy the tessdata folders and dlls.

Copy the tesseract-include folder to the root folder of your project. Now Click on your project in Visual Studio Solution Explorer, and go to Project>Properties. 

VC++ Directories>Include Directories:
```
..\tesseract-include\tesseract;..\tesseract-include\leptonica;$(IncludePath)
```

C/C++>Preprocessor>Preprocessor Definitions:
```
 _CRT_SECURE_NO_WARNINGS;%(PreprocessorDefinitions)
```

C/C++>Linker>Input>Additional Dependencies:
```
..\tesseract-include\libtesseract302.lib;..\tesseract-include\liblept168.lib;%(AdditionalDependencies)
```

Now you can include headers in your project's file:

```#include <baseapi.h>```

```#include <allheaders.h>```


Now copy the two dll files in tesseract-include and the tessdata folder in Debug to the Output Directory of your project.

When you initialize tesseract, you need to specify the location of the parent folder (!important) of the tessdata folder if it is not already the current directory of your executable file. You can copy my script, which assumes tessdata  is installed in the executable's folder.

```
tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
api->Init("D:\\tessdataParentFolder\\", ...
```


## Sample

You can compile the provided sample, which takes one command line argument of the image path to use. The preprocess()  function uses Leptonica to create a black and white book-like copy of the image which makes tesseract work with 90% accuracy. The ocr() function shows the  functionality of the Tesseract API to return a string output. The toClipboard() can be used to save text to clipboard on Windows. You can copy these into your own projects.
