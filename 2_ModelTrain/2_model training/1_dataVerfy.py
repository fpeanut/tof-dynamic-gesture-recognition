import numpy as np
import os
import matplotlib.pyplot as plt

file_dir = 'E:\\6_HandRecognition\\2_DATA\\2_Train\\2_frame18\\WIN\\points\\3dNormal'  # 你的文件路径
file_dir1 = 'E:\\6_HandRecognition\\2_DATA\\2_Train\\2_frame18\\WIN\\label'  # 你的文件路径
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

#定义数据集,数据集标签
trian_txt,train_dir= getFlist(file_dir)
df=[]
labels=[]
for i in trian_txt:
    df.append(np.loadtxt(i,delimiter=","))
df=np.array(df)

for j in train_dir:
    tpstr=np.loadtxt(file_dir1+"\\"+j+".txt",dtype='str')
    labels.append(tpstr)
labels=np.array(labels)
a1=a2=a3=a4=a5=a6=a7=a8=a9=a10=a11=a12=0
for num in labels:
    if(num=="FingerCircleClockwise"):
        a1+=1
    elif(num=="FingerCircleCounterClockwise"):
        a2+=1
    elif (num == "FistToPalm"):
        a3 += 1
    elif (num == "HandCircleClockwise"):
        a4 += 1
    elif (num == "HandCircleCounterClockwise"):
        a5 += 1
    elif (num == "HandFlickDown"):
        a6 += 1
    elif (num == "HandFlickLeft"):
        a7 += 1
    elif (num == "HandFlickRight"):
        a8 += 1
    elif (num == "HandFlickUp"):
        a9 += 1
    elif (num == "other"):
        a10 += 1
    elif (num == "PalmToFist"):
        a11 += 1
    elif (num == "Push"):
        a12 += 1
historm=[]
histormx=["FingerCircleClockwise","FingerCircleCounterClockwise","FistToPalm","HandCircleClockwise","HandCircleCounterClockwise","HandFlickDown","HandFlickLeft","HandFlickRight","HandFlickUp"
          ,"other","PalmToFist","Push"]
historm.append(a1)
historm.append(a2)
historm.append(a3)
historm.append(a4)
historm.append(a5)
historm.append(a6)
historm.append(a7)
historm.append(a8)
historm.append(a9)
historm.append(a10)
historm.append(a11)
historm.append(a12)
figure=plt.figure(figsize=(24, 6))
plt.xticks(rotation = 350)
plt.bar(histormx, historm, color = 'blue')
plt.show()