import keras
import nodespy


class NodeSpyInjectionSpy(keras.callbacks.Callback):

    def on_train_begin(self, logs={}):
        nodespy.start_logging()

    def on_epoch_end(self, batch, logs={}):
        nodespy.push_log('loss', float(logs.get('loss')))

_nodespy_injection = NodeSpyInjectionSpy()


import keras
from keras.layers import Dense, Dropout
import numpy as np

def load_dataset():
    global train_X, train_Y, test
    train_X = np.array([[0, 0], [0, 1], [1, 1]])
    train_Y = np.array([0, 1, 0])
    test = np.array([[1, 0]])


def train():
    model = keras.models.Sequential()
    model.add(Dense(2, activation='relu', input_shape=(2,)))
    model.add(Dense(2, activation='relu'))
    model.add(Dense(1, activation='softmax'))
    model.compile('rmsprop', 'mse')

    model.fit(train_X, train_Y, epochs=10, callbacks=[_nodespy_injection])
    print("NET!!!")

    return model.predict(test)[0]


if __name__ == '__main__':
    load_dataset()
    train()
