import os
import numpy as np
import cv2
from sklearn.preprocessing import LabelEncoder
from tensorflow.keras.utils import to_categorical

class get_data():
    def __init__(self,img_path,label_path):
        self.img_path=img_path
        self.label_path=label_path
    def load_img(self):
        path_list = next(os.walk(self.img_path))[1]#获取一级目录，即样本文件名
        sequen=[]
        labels=[]
        delnum=0
        for i in range(len(path_list)):
            # print(path_list[i])
            imgs=[]
            #读取图片
            img_count=0
            for img_name in os.listdir(self.img_path+"/"+path_list[i]+"/IR/"):
                img = cv2.imread(self.img_path+"/"+path_list[i]+"/IR/"+ img_name)
                temp_img = cv2.resize(img, (50, 50))
                # cv2.imshow("temp_img",temp_img)
                imgs.append(temp_img)
                img_count=img_count+1
                cv2.waitKey(1)
            if(img_count!=18):
                delnum=delnum+1
                print(path_list[i])
            sequen.append(imgs)
            #读取label
            txtstr=np.loadtxt(self.label_path+"/"+path_list[i]+".txt",dtype='str')
            labels.append(txtstr)
        # if(delnum==0):
        #     file = open(r"D:\python\test1\已验证.txt", "w", encoding='utf-8')
        #     file.close()
        label_encoder = LabelEncoder()#str转普通数字编码
        labels = label_encoder.fit_transform(labels)
        labels = to_categorical(labels).astype(int)  # 独热编码方法二
        return np.asarray(sequen,np.float32),np.array(labels)#输出格式 (samples_num,sequen,img_w,img_h,channels)


if __name__=='__main__':
    file_img = 'E:\\6_HandRecognition\\2_DATA\\2_Train\\2_frame18\\NoWIN\\img'  # 你的文件路径
    file_label = 'E:\\6_HandRecognition\\2_DATA\\2_Train\\2_frame18\\NoWIN\\label'  # 你的文件路径
    new_object=get_data(file_img,file_label)
    data,label=new_object.load_img()
    print(data.shape)
    print(label.shape)



