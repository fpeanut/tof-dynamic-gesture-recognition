from tensorflow.keras.models import load_model
from sklearn.preprocessing import LabelEncoder
from tensorflow.keras.utils import to_categorical
import tensorflow as tf

import numpy as np
import os

# set GPU 按需增长
tf.debugging.set_log_device_placement(True)
gpus = tf.config.experimental.list_physical_devices('GPU') #列举gpu
tf.config.experimental.set_visible_devices(gpus[0], 'GPU')#用于设置对于本进程可见的设备。比如一个机器上有4个GPU，只设置1个对本进程可见，故本进程就不能使用其他设备。
for gpu in gpus:
    tf.config.experimental.set_memory_growth(gpu, True)#设置内存自增长，内存用多少就占多少，而不是一下子把内存都占满。
# print(len(gpus))
logical_gpus = tf.config.experimental.list_logical_devices('GPU')#获取所有的逻辑设备。物理设备即一整块儿物理磁盘，逻辑设备即物理磁盘上的分区。
#时间序列长度
timesetp=18
#加载模型文件
model=load_model("./model/_220927_lstm-model.h5")
model.summary()
#构建测试集
testData=[]
labels=[]
def getFlist(path):#txt文件路径
    temp=[]
    temp_dir = []
    for root, dirs, files in os.walk(path):
        #print('root_dir:', root)  #当前路径
        # print('sub_dirs:', dirs)   #子文件夹
        # print('files:', files)     #文件名称，返回list类型
        for i in files:
            temp.append(root+"\\"+i)#txt文件路径
    dnames = os.listdir(path)
    for dname in dnames:

        temp_dir.append(dname)#子文件夹路径
    return temp,temp_dir
testTxt,testDir=getFlist("F:\\15_HandRecognition\\3_EXE\\2_getFeature\DataSet\\train\\noraml")
for i in testTxt:
    testData.append(np.loadtxt(i,delimiter=","))
testData=np.array(testData)
testData=testData.reshape(int(testData.shape[0]/timesetp),timesetp,-1)
for j in testDir:
    tpstr=np.loadtxt("F:\\15_HandRecognition\\3_EXE\\2_getFeature\DataSet\\train\\label"+"\\"+j+".txt",dtype='str')
    labels.append(tpstr)

# 设置普通标签
label_encoder = LabelEncoder()
labels = label_encoder.fit_transform(labels)
labels=to_categorical(labels).astype(int)#独热编码方法二
print("独热编码标签："+labels)

# #预测
pre_y=model.predict(testData)
print("预测值："+pre_y)
#获取预测值最大值索引
for m in range(pre_y.shape[0]):
    for n in range(pre_y.shape[1]):
        if(pre_y[m][n]>0.5):
            pre_y[m][n]=1
        else:
            pre_y[m][n]=0
pre_label=np.argmax(pre_y, axis=-1)
rational_label=np.argmax(labels,axis=-1)
print("预测标签:"+pre_label)
print("真实标签:"+rational_label)
#计算准确度
cout_num=0
for i in range(len(pre_label)):
    if(pre_label[i]==rational_label[i]):
        cout_num=cout_num+1
accuarcy=cout_num/len(pre_label)
print('准确度:'+str(accuarcy))