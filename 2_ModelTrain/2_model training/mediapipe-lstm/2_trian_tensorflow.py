import numpy as np
import tensorflow as tf
import matplotlib.pyplot as plt
from sklearn.preprocessing import LabelEncoder
from sklearn.preprocessing import OneHotEncoder
from tensorflow.keras.models import Sequential
from sklearn.model_selection import train_test_split
from tensorflow.keras.utils import to_categorical
from tensorflow.keras.layers import LSTM, Dense,Dropout,Bidirectional
from tensorflow.keras.optimizers import Adam
from attention import  Attention

import tensorflow.keras.backend as K
from tensorflow.keras.callbacks import LearningRateScheduler

import os
import datetime  # 导入时间

# set GPU 按需增长
tf.debugging.set_log_device_placement(True)
gpus = tf.config.experimental.list_physical_devices('GPU') #列举gpu
tf.config.experimental.set_visible_devices(gpus[0], 'GPU')#用于设置对于本进程可见的设备。比如一个机器上有4个GPU，只设置1个对本进程可见，故本进程就不能使用其他设备。
for gpu in gpus:
    tf.config.experimental.set_memory_growth(gpu, True)#设置内存自增长，内存用多少就占多少，而不是一下子把内存都占满。
logical_gpus = tf.config.experimental.list_logical_devices('GPU')#获取所有的逻辑设备。物理设备即一整块儿物理磁盘，逻辑设备即物理磁盘上的分区。

#数据处理类
class setData():
    def __init__(self,datapth,labelpath,timestep):
        self.datapth=datapth
        self.labelpath = labelpath
        self.timestep=timestep
    def getDataPath(self,path):
        temp = []
        temp_dir = []
        for root, dirs, files in os.walk(path):
            # print('root_dir:', root)  #当前路径
            # print('sub_dirs:', dirs)   #子文件夹
            # print('files:', files)     #文件名称，返回list类型
            for i in files:
                temp.append(root + "\\" + i)  # txt文件路径
        dnames = os.listdir(path)
        for dname in dnames:
            temp_dir.append(dname)  # 子文件夹路径
        return temp, temp_dir
    def getDataAndLabel(self):
        df = []
        labels = []
        # 定义数据集
        trian_txt, train_dir = self.getDataPath(self.datapth)
        for i in trian_txt:
            df.append(np.loadtxt(i, delimiter=","))
        df = np.array(df)
        Data = df.reshape(int(df.shape[0] / self.timestep), self.timestep, -1)
        #数据集标签
        for j in train_dir:
            tpstr = np.loadtxt(self.labelpath + "\\" + j + ".txt", dtype='str')
            labels.append(tpstr)
        labels = np.array(labels)
        print(Data.shape,labels.shape)
        return Data,labels
    def splitDateAndLabel(self):
        #获取数据集，标签信息
        Data,labels=self.getDataAndLabel()
        # 设置普通标签
        label_encoder = LabelEncoder()
        labels = label_encoder.fit_transform(labels)
        # #训练集验证集划分
        labels = to_categorical(labels).astype(int)  # 独热编码方法二
        X_train, X_test, y_train, y_test = train_test_split(Data, labels, test_size=0.2, random_state=2)
        print(X_train.shape,y_train.shape)
        return X_train, X_test, y_train, y_test
#模型搭建
class HandModel(tf.keras.Model):
    def __init__(self,timestep,features,batchSize,m_epochs,X_train, X_test, y_train, y_test):
        super(HandModel, self).__init__()
        self.timestep=timestep
        self.features=features
        self.batchSize=batchSize
        self.m_epochs=m_epochs
        self.X_train=X_train
        self.X_test=X_test
        self.y_train=y_train
        self.y_test=y_test
        # 学习率退火

    def LSTM(self):
        # 学习率退火
        lr_save = []
        def scheduler(epoch):
            # 每隔n个epoch，学习率减小为原来的1/10
            if epoch % 50 == 0 and epoch != 0:
                lr = K.get_value(model.optimizer.lr)
                K.set_value(model.optimizer.lr, lr * 0.1)
                lr_save.append(lr * 0.1)
            return K.get_value(model.optimizer.lr)
        reduce_lr = LearningRateScheduler(scheduler)
        #模型搭建
        model = Sequential()
        model.add(LSTM(64, return_sequences=False, activation='relu', input_shape=(self.timestep, self.features)))
        model.add(Dropout(0.3))
        # Attention(name='attention_weight')
        model.add(Dense(self.y_train.shape[1], activation='softmax'))
        # 编译模型
        model.compile(metrics=['accuracy'], loss='categorical_crossentropy', optimizer='Adam')
        # 训练模型
        history = model.fit(self.X_train, self.y_train, epochs=self.m_epochs, verbose=1, batch_size=self.batchSize,
                            validation_data=(self.X_test, self.y_test), callbacks=[reduce_lr])
        # 查看网络结构
        model.summary()
        save_name = "./model/" + '_LSTM_' + datetime.datetime.now().strftime('%Y%m%d') + '_epochs_' + str(self.m_epochs) + "+batch_size_" + str(self.batchSize)  # 当前时间：年月日
        model.save("./model/_220927_lstm-model.h5")
        return history
    def BIT_LSTM(self):
        # 学习率退火
        lr_save = []

        def scheduler(epoch):
            # 每隔n个epoch，学习率减小为原来的1/10
            if epoch % 50 == 0 and epoch != 0:
                lr = K.get_value(model.optimizer.lr)
                K.set_value(model.optimizer.lr, lr * 0.1)
                lr_save.append(lr * 0.1)
            return K.get_value(model.optimizer.lr)

        reduce_lr = LearningRateScheduler(scheduler)
        # 模型搭建
        model = Sequential()
        model.add(Bidirectional(LSTM(64, return_sequences=False, activation='relu', input_shape=(self.timestep, self.features))))  # 双向LSTM
        # model.add(Dropout(0.3))
        # Attention(name='attention_weight')
        model.add(Dense(self.y_train.shape[1], activation='softmax'))
        # 编译模型
        model.compile(metrics=['accuracy'], loss='categorical_crossentropy', optimizer='Adam')
        # 训练模型
        history = model.fit(self.X_train, self.y_train, epochs=self.m_epochs, verbose=1, batch_size=self.batchSize,
                            validation_data=(self.X_test, self.y_test), callbacks=[reduce_lr])
        # 查看网络结构
        model.summary()
        save_name = "./model/" + '_BitLSTM_' + datetime.datetime.now().strftime('%Y%m%d') + '_epochs_' + str(
            self.m_epochs) + "+batch_size_" + str(self.batchSize)  # 当前时间：年月日
        model.save("./model/_220927_Bitlstm-model.h5")
        return history
if __name__ == '__main__':
    #数据集分配
    file_dir_data = 'F:\\15_HandRecognition\\2_DATA\\trainDataNoNoise\\points\\3dNormal'  # 你的文件路径
    file_dir_label = 'F:\\15_HandRecognition\\2_DATA\\trainDataNoNoise\\label'  # 你的文件路径
    newDataBase=setData(file_dir_data,file_dir_label,30)
    newData, newlabels=newDataBase.getDataAndLabel()
    newX_train, newX_test, newy_train, newy_test=newDataBase.splitDateAndLabel()
    #模型训练
    newModel=HandModel(timestep=18,features=63,batchSize=20,m_epochs=200,X_train=newX_train,X_test=newX_test,y_train=newy_train,y_test=newy_test)
    history=newModel.LSTM()
    # plot loss learning curves
    plt.figure(figsize=(15, 15))
    plt.subplot(2, 2, 1)
    plt.plot(history.history['loss'])
    plt.plot(history.history['val_loss'])
    plt.title('300-samples 4-gestures')
    plt.ylabel('loss')
    plt.xlabel('epoch_' + str(100) + " batch_size_" + str(36))
    plt.ylim(ymax=1.0, ymin=0.0)
    plt.legend(['train', 'validation'], loc='upper right')
    # plot accuracy learning curves
    plt.subplot(2, 2, 2)
    plt.title('Accuracy', pad=-40)
    plt.plot(history.history['accuracy'], label='train')
    plt.plot(history.history['val_accuracy'], label='test')
    plt.legend()
    plt.show()