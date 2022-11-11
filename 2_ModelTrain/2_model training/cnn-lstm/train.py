import tensorflow as tf
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

#获取数据集
new_object=get_data(".\\data\\img",".\\data\\label")
data,label=new_object.load_img()
# #训练集验证集划分
X_train,X_test,y_train,y_test = train_test_split(data,label,test_size=0.2,random_state=3)
print(X_train.shape,y_train.shape)

#训练
obj=HandModel(y_train.shape[1])
Model=obj.model()
history=Model.fit(X_train, y_train, epochs=10,verbose=1,batch_size=2,validation_data=(X_test, y_test))
Model.save("cnn_lstm_"+datetime.datetime.now().strftime('%Y%m%d')+".h5")
Model.summary()
print("H.histroy keys：", history.history.keys())

#显示
plt.figure(figsize=(15,15))
plt.subplot(2,2,1)
plt.plot(history.history['loss'])
plt.plot(history.history['val_loss'])
plt.title('resnet-lstm')
plt.ylabel('loss')
plt.xlabel('epoch')
plt.ylim(ymax=1.0, ymin=0.0)
plt.legend(['train', 'validation'], loc='upper right')
# plot accuracy learning curves
plt.subplot(2,2,2)
plt.title('Accuracy', pad=-40)
plt.plot(history.history['acc'], label='train')
plt.plot(history.history['val_acc'], label='test')
plt.legend()
plt.show()
