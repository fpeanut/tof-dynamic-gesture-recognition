import torch
import matplotlib.pyplot as plt
from sklearn.preprocessing import LabelEncoder
from sklearn.model_selection import train_test_split
import torch.utils.data as Data
import torch.nn as nn
import torch.optim as optim
import numpy as np
# import pandas as pd
import os
# import onnx


timesetp=30
features=63
label_num=5
batchSize=40
m_epochs=400

device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")

def get_data():
    # 获取train label文件路径
    file_dir = 'E:\\6_HandRecognition\\2_DATA\\train'  # 你的文件路径
    file_dir1 = 'E:\\6_HandRecognition\\2_DATA\\label'  # 你的文件路径
    def getFlist(path):  # txt文件路径
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

    # 定义数据集,数据集标签
    trian_txt, train_dir = getFlist(file_dir)
    df = []
    labels = []
    for i in trian_txt:
        df.append(np.loadtxt(i, delimiter=","))
    df = np.array(df)
    Data = df.reshape(int(df.shape[0] / timesetp), timesetp, -1)

    for j in train_dir:
        tpstr = np.loadtxt(file_dir1 + "\\" + j + ".txt", dtype='str')
        labels.append(tpstr)
    labels = np.array(labels)
    print(labels)
    # 设置普通标签
    label_encoder = LabelEncoder()
    labels = label_encoder.fit_transform(labels)
    X_train, X_test, y_train, y_test = train_test_split(Data, labels, test_size=0.8, random_state=1)
    #numpy转torch
    X_train=torch.Tensor(X_train)
    X_test = torch.Tensor(X_test)
    y_train=torch.tensor(y_train).long()
    y_test = torch.tensor(y_test).long()
    return X_train,y_train,X_test,y_test
class HandLstm(nn.Module):
    def __init__(self, features,hidden_size,num_layers,batch_size):
        super(HandLstm, self).__init__()
        self.features=features
        self.hidden_size=hidden_size
        self.num_layers=num_layers
        self.batch_size=batch_size
        self.lstm=nn.LSTM(input_size=self.features,hidden_size=self.hidden_size, num_layers= self.num_layers,
                            batch_first=True)
        self.fc=nn.Sequential(nn.Linear(hidden_size, label_num),
                              nn.ReLU())

    def forward(self,input_seq):
        batch_size, seq_len, _ = input_seq.size()
        h_0 = torch.randn(self.num_layers, self.batch_size, self.hidden_size).to(device)
        c_0 = torch.randn(self.num_layers, self.batch_size, self.hidden_size).to(device)
        output, _ = self.lstm(input_seq, (h_0, c_0))
        pred = self.fc(output)
        pred = pred[:, -1, :]
        return pred
if __name__ == '__main__':
    trainX, trainY ,testX,testY= get_data()
    print(trainX.size(),trainY.size(),testX.size(),testY.size(0))
    train_loader = Data.DataLoader(
        dataset=Data.TensorDataset(trainX, trainY),  # 封装进Data.TensorDataset()类的数据，可以为任意维度
        batch_size=batchSize,  # 每块的大小
        shuffle=True,  # 要不要打乱数据 (打乱比较好)
        num_workers=0,  # 多进程（multiprocess）来读数据
    )
    test_loader=Data.DataLoader(
        dataset=Data.TensorDataset(testX, testY),  # 封装进Data.TensorDataset()类的数据，可以为任意维度
        batch_size=batchSize,  # 每块的大小
        shuffle=False,  # 要不要打乱数据 (打乱比较好)
        num_workers=0,  # 多进程（multiprocess）来读数据
    )
    #损失、优化
    model=HandLstm(features=features,hidden_size=128,num_layers=2,batch_size=batchSize).to(device)
    optimizer = optim.Adam(model.parameters(), lr=0.0001)
    criterion = nn.CrossEntropyLoss().to(device)
    scheduler = optim.lr_scheduler.StepLR(optimizer,50,gamma=0.1,last_epoch=-1)#学习率衰减

    # 开始训练
    trian_loss = 0
    val_loss=0
    best_model = None
    min_val_loss = 5
    trian_loss_all = []
    val_loss_all=[]
    accuracy_all=[]
    for i in range(m_epochs):
        accuracy = 0
        for seq, labels in train_loader:
            seq=seq.to(device)
            labels=labels.to(device)
            y_pre=model(seq).to(device)
            optimizer.zero_grad()
            trian_loss=criterion(y_pre,labels)
            trian_loss.backward()
            optimizer.step()
        scheduler.step()
        trian_loss_all.append(trian_loss.item())
        print('epoch:%d loss:%.5f' % (i, trian_loss.item()))
    #开始验证
        with torch.no_grad():
            for seq, labels in test_loader:
                seq = seq.to(device)
                labels = labels.to(device)
                y_pre=model(seq).to(device)
                val_loss = criterion(y_pre,labels)
                accuracy+=(y_pre.argmax(1)==labels).sum()
        accuracy_all.append(accuracy/testY.size(0))
        val_loss_all.append(val_loss.item())
        print("---在测试集上准确率为：{}---".format(accuracy/testY.size(0)))
    # state = {'model': model.state_dict(), 'optimizer': optimizer.state_dict()}
    torch.save(model.state_dict(), './model/LSTM_'+str(m_epochs)+'_class'+str(label_num)+'.pt')
    # torch.save(model,'./model/_1010_LSTM.pt')
    # self.fc=nn.Sequential(
    #     nn.Linear(hidden_size,64),
    #     nn.ReLU(),
    #     nn.Linear(64,label_num),
    #     nn.ReLU()
    # )
    plt.figure(figsize=(12, 4))
    plt.subplot(1,2,1)
    plt.plot(trian_loss_all, label='loss', color='b')
    plt.plot(val_loss_all, label='loss', color='r')
    plt.xlabel('X-axis')
    plt.ylabel('Y-axis')
    plt.title('loss Plot')
    plt.legend(['train', 'validation','accuary'], loc='upper right')
    plt.subplot(1, 2, 2)
    plt.plot(np.array(torch.tensor(accuracy_all, device='cpu')),label='accuracy')
    plt.title('accuracy Plot')
    plt.show()
   # # 模型测试
   #  model=HandLstm(hidden_size=128,features=features,num_layers=2,batch_size=testY.size(0)).to(device)
   #  model.load_state_dict(torch.load('model/LSTM_200.pt'))
   #  model.eval()
   #  num = 0
   #  input_ = torch.tensor(testX, dtype=torch.float32).to(device)
   #  label = torch.tensor(testY, dtype=torch.long).to(device)
   #  # print(label)
   #  output = model(input_)
   #  print(output)
   #  max,max_dict=torch.max(output,1)
   #  for i in range(len(label)):
   #      if(label[i]==max_dict[i]):
   #          num=num+1
   #  print("准确率："+str(num/len(label)))
   #  # pred = output.max(dim=-1)[1]
   #  print(label.tolist())
   #  print(max_dict.tolist())





