# OpenCV环境配置

## 下载openCV

下载地址：https://opencv.org/releases/

安装路径：D:\Program Files\


## 配置环境变量

| Visual Studio 版本 | Visual C++ 版本
| --- | --- |
VS 6.0	| VC 6.0
VS 2013	| VC 12
VS 2015	| VC 14
VS 2017	| VC 15
VS 2019	| VC 16

将以下路径添加的path中（注意VC版本）
D:\Program Files\opencv\build\x64\vc16\bin

## 配置VS项目属性

打开项目属性 -> VC++目录 -> 常规&包含目录 -> 将以下两个路径追加进去

D:\Program Files\opencv\build\include
D:\Program Files\opencv\build\include\opencv2


打开项目属性 -> VC++目录 -> 常规&库目录 -> 将下面这个路径追加进去
D:\Program Files\opencv\build\x64\vc16\lib


打开项目属性 -> 链接器 -> 输入 -> 附加依赖项 -> 将下面这个文件名追加进去
opencv_world470d.lib


# tesseract OCR环境配置

## 下载tesseract

下载地址：https://digi.bib.uni-mannheim.de/tesseract/

选择win64最新版本下载：https://digi.bib.uni-mannheim.de/tesseract/tesseract-ocr-w64-setup-5.3.1.20230401.exe

安装路径：D:\Program Files\tesseractOCR

## 配置环境变量

将以下路径添加到path中
D:\Program Files\tesseractOCR

## 下载中文文字库

https://github.com/tesseract-ocr/tessdata/blame/main/chi_sim.traineddata

## 下VCPKG

https://github.com/microsoft/vcpkg.git

安装路径：D:\Program Files\vcpkg

运行源码中的bootstrap-vcpkg.bat文件

## 配置环境变量
将以下路径添加到path中
D:\Program Files\vcpkg