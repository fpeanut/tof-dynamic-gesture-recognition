from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import LSTM, Dense,Dropout,Bidirectional,Flatten,TimeDistributed,Conv2D,MaxPooling2D
from tensorflow.keras.applications import ResNet50
class HandModel():
    def __init__(self,class_num):
        super(HandModel,self).__init__()
        self.class_num=class_num
    def model(self):
        model=Sequential()
        # model.add(TimeDistributed(ResNet50(include_top=False,weights="imagenet",pooling='avg'),input_shape=(None,50,50,3)))
        model.add(TimeDistributed(Conv2D(2, (2, 2), activation='relu'), input_shape=(None, 50, 50, 3)))
        model.add(TimeDistributed(MaxPooling2D(pool_size=(2, 2))))
        model.add(TimeDistributed(Flatten()))
        model.add(LSTM(64, return_sequences=False, activation='relu'))
        model.add(Dense(self.class_num, activation='softmax'))
        model.compile(metrics=['accuracy'], loss='categorical_crossentropy',optimizer='Adam')
        model.summary()
        return model

