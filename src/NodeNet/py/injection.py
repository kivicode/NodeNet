import keras
import nodespy


class NodeSpyInjectionSpy(keras.callbacks.Callback):

    def on_train_begin(self, logs={}):
        nodespy.start_logging()

    def on_epoch_end(self, batch, logs={}):
        nodespy.push_log('loss', float(logs.get('loss')))

_nodespy_injection = NodeSpyInjectionSpy()