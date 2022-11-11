# tof-dynamic-gesture-recognition（tof、cnn-lstm、mediapipe-lstm、持续更新。。。）<br>
------------------------------------------------------动态手势识别流程-------------------------------------------------------------<br>
1.数据采集<br>
2.数据管理<br>
3.数据处理：时间序列原始数据生成、数据清洗<br>
4.特征数据集生成：图像数据集、关键点数据集、关键点融合特征数据集<br>
5.训练框架：时间序列特征生成、时间序列特征分类<br>
6.评价指标：测试方法、结果对比<br>
7.模型部署：不同模型转换（pt、h5->onnx、tflite）、模型检测方案<br>
详解<br>
`数据采集`<br>
根据自己相机sdk的要求自行编写采集软件。采集流程如下：<br>
![Image text](https://github.com/fpeanut/tof-dynamic-gesture-recognition/blob/main/img/1668144844199.jpg)<br>
<br>
`数据管理`<br>
制定原始采集数据、训练数据集的存放方式。<br>
![Image text](https://github.com/fpeanut/tof-dynamic-gesture-recognition/blob/main/img/datacontrol.jpg)<br>
<br>
`数据处理`<br>
该步骤将前面录取的原始图像数据采用特定的方式（如滑动串口）生成用于训练集生成的图像数据集，流程如下图。<br>
![Image text](https://github.com/fpeanut/tof-dynamic-gesture-recognition/blob/main/img/dataprocess.jpg)<br>
注：滑动窗口法使用<br>
考虑到人类活动在现实中是如何进行的，我们了解到每一个活动都是几个基本的连续运动的组合。通常情况下，一个人的活动可以在练习中持续几秒钟，在一秒钟内，一些基本的动作就可以参与其中。<br>
1.数据集采集方式:指定某个手势动作在规定时间内做固定的次数手势。<br>
2.数据集制作方式：<br>
利用固定宽度滑动窗口在该视频内进行分割，设置滑动窗口步长s，s越小数据集数量越多。（借鉴UCI-HAR数据集原理<br>
![Image text](https://github.com/fpeanut/tof-dynamic-gesture-recognition/blob/main/img/slidwin.jpg)<br>
`特征数据集生成`<br>
该部分是将图像数据集处理成不同网络想要的训练集和验证集。主要分图像训练集和关键点训练集。<br>
图像训练集生成：直接将数据处理步骤的图像数据集作为训练集即可。<br>
关键点训练集：需要编写mediapipe关键点代码，将所有样图片转换成21个关键点或者其他关键点相关的融合特征。<br>
![Image text](https://github.com/fpeanut/tof-dynamic-gesture-recognition/blob/main/img/datasc.png)<br>
`注`：mediaipe c++编译，dll生成与运行下载链接：https://pan.baidu.com/s/1ditBJOPpW7ae_RDSL8GnqA  提取码：6fva<br>
<br>
`训练`<br>
1.cnn-lstm<br>
网络结构：![Image text](https://github.com/fpeanut/tof-dynamic-gesture-recognition/blob/main/img/cnn-lstm.jpg)<br>
code:https://github.com/fpeanut/tof-dynamic-gesture-recognition/tree/main/2_ModelTrain/2_model%20training/cnn-lstm
2.mediapipe-lstm<br>
code:https://github.com/fpeanut/tof-dynamic-gesture-recognition/tree/main/2_ModelTrain/2_model%20training/mediapipe-lstm
