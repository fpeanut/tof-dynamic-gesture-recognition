import tensorflow as tf
model=tf.keras.models.load_model("./model/_220927_lstm-model.h5")
# Convert model.
converter = tf.lite.TFLiteConverter.from_keras_model(model)
converter.target_spec.supported_ops = [
 tf.lite.OpsSet.TFLITE_BUILTINS, # enable TensorFlow Lite ops.
 tf.lite.OpsSet.SELECT_TF_OPS # enable TensorFlow ops.
]
converter.optimizations = [tf.lite.Optimize.DEFAULT]
model_content = converter.convert()
open("./model/TF_BEST.tflite", "wb").write(model_content)