# tof-dynamic-gesture-recognition（tof、cnn-lstm、mediapipe-lstm、持续更新。。。）<br>
------------------------------------------------------动态手势识别流程-------------------------------------------------------------<br>
1.数据采集<br>
2.数据管理<br>
3.数据处理：时间序列原始数据生成、数据清洗<br>
4.特征数据集生成：图像数据集、关键点数据集、关键点融合特征数据集<br>
5.训练框架：时间序列特征生成、时间序列特征分类<br>
6.评价指标：测试方法、结果对比<br>
7.模型部署：不同模型转换（pt、h5->onnx、tflite）、模型检测方案<br>
详解
数据采集<br>
根据自己相机sdk的要求自行编写采集软件。采集流程如下：<br>
![Image text](https://github.com/fpeanut/tof-dynamic-gesture-recognition/blob/main/img/1668144844199.jpg)<br>
<br>
数据管理<br>
制定原始采集数据、训练数据集的存放方式。<br>
![Image text](https://github.com/fpeanut/tof-dynamic-gesture-recognition/blob/main/img/datacontrol.jpg)<br>
