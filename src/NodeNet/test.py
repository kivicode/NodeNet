from keras.models import Model
from keras.layers import *


def generate_model():
	inp = Input(shape=(2,))
	hid = Dense(2, activation="relu")(inp)
	outp = Dense(1, activation="softmax")(hid)
	model = Model(inputs=inp, outputs=outp)

	return model

def load_dataset():
	return np.array([[0,0], [0,1], [1,1]]), np.array([0, 1, 0]), [], []

if __name__ == '__main__':
	model = generate_model()
	x_train, y_train, x_test, y_test = load_dataset()

	model.compile(optimizer='sgd', loss='mse')

	print('[INFO ] Training started')
	model.fit(x_train, y_train, batch_size=32, epochs=10)
	print('[INFO ] Training finished')

	print(model.predict(np.array([[1, 0]]))[0])
