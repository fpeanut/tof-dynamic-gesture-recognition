import torch
import torch.nn as nn
import numpy as np
import os
from  pytoch_train import HandLstm
from sklearn.preprocessing import LabelEncoder

device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")
timestep=18
#准备数据
file_dir = 'E:\\6_HandRecognition\\2_DATA\\2_Train\\2_frame18\\WIN\\points\\3dNormal'  # 你的文件路径
file_dir1 = 'E:\\6_HandRecognition\\2_DATA\\2_Train\\2_frame18\\WIN\\label'  # 你的文件路径
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
Data = df.reshape(int(df.shape[0] / timestep), timestep, -1)

for j in train_dir:
    tpstr = np.loadtxt(file_dir1 + "\\" + j + ".txt", dtype='str')
    labels.append(tpstr)
labels = np.array(labels)
print(labels)
# 设置普通标签
label_encoder = LabelEncoder()
labels = label_encoder.fit_transform(labels)
Data = torch.Tensor(Data)
labels = torch.Tensor(labels)
# 模型测试
model=HandLstm(hidden_size=128,features=Data[0][0].size(0),num_layers=2,batch_size=labels.size(0)).to(device)
model.load_state_dict(torch.load('model/LSTM_200.pt'))
model.eval()
num = 0
input_ = torch.tensor(Data, dtype=torch.float32).to(device)
label = torch.tensor(labels, dtype=torch.long).to(device)
# print(label)
output = model(input_)
print(output.size())
max,max_dict=torch.max(output,1)
for i in range(len(label)):
    if(label[i]==max_dict[i]):
        num=num+1
print("准确率："+str(num/len(label)))
# pred = output.max(dim=-1)[1]
print(label.tolist())
print(max_dict.tolist())
