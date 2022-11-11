import torch
import onnx
import onnxruntime as ort
import pytoch_train
from pytoch_train import HandLstm
import numpy as np
timesetp=30

device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")
model=HandLstm(hidden_size=128,features=63,num_layers=2,batch_size=1).to(device)
model.load_state_dict(torch.load('model/LSTM_400_class5.pt'))
model.eval()
x = torch.randn(1, 30, 63).to(device)
t = model(x)
torch.onnx.export(model, x, "model/LSTM_400_class5.onnx"
                  ,verbose=True,input_names=['input'],output_names=['output'])


#模型推理
#1.准备测试数据
trainX, trainY ,testX,testY= pytoch_train.get_data()
print(trainX.size(),trainY.size(),testX.size(),testY.size(0))
testdata=testX.detach().numpy()
testdata=testdata.astype(np.float32)
print(type(testdata))
data=np.random.rand(1,30,63)
data=data.astype(np.float32)
#2.加载模型
onnx_model_path = "model/LSTM_400_class5.onnx"
ort_session = ort.InferenceSession(onnx_model_path,providers=['TensorrtExecutionProvider', 'CUDAExecutionProvider', 'CPUExecutionProvider'])
#3.获取模型信息（输入输出的名字，维度）
onnx_input_name = ort_session.get_inputs()[0].name#输入输出层名字(固定写法)
onnx_outputs_names = ort_session.get_outputs()#输出层名字,可能有多个
output_names = []
for o in onnx_outputs_names:
    output_names.append(o.name)
#4.模型推理
onnx_result = ort_session.run(output_names, input_feed={onnx_input_name: data})
#5.输出结果
onnx_result = onnx_result[0]  #这里只有一个输出
print(onnx_result)
pre_result=np.argmax(onnx_result, axis=-1)
print(pre_result)





