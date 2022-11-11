import tensorflow as tf
from tensorflow.keras.models import load_model
from model import HandModel
from getData import get_data
from sklearn.model_selection import train_test_split
import matplotlib.pyplot as plt
from keras import backend as K
import datetime  # 导入时间

# set GPU 按需增长
tf.debugging.set_log_device_placement(True)
gpus = tf.config.experimental.list_physical_devices('GPU') #列举gpu
tf.config.experimental.set_visible_devices(gpus[0], 'GPU')#用于设置对于本进程可见的设备。比如一个机器上有4个GPU，只设置1个对本进程可见，故本进程就不能使用其他设备。
for gpu in gpus:
    tf.config.experimental.set_memory_growth(gpu, True)#设置内存自增长，内存用多少就占多少，而不是一下子把内存都占满。
# print(len(gpus))
logical_gpus = tf.config.experimental.list_logical_devices('GPU')#获取所有的逻辑设备。物理设备即一整块儿物理磁盘，逻辑设备即物理磁盘上的分区。
# print(len(logical_gpus))

#时间序列长度
timesetp=18
#加载模型文件
model=load_model("cnn_lstm_20221031.h5")
model.summary()
#获取测试集
new_object=get_data(".\\data\\img",".\\data\\label")
data,label=new_object.load_img()

# #预测
pre_y=model.predict(data)
print("预测值："+pre_y)
