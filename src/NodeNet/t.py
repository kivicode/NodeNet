

def generate_model():


def load_dataset():
	return np.array([i for i in range(8)]), np.array([0, 1]*4), [], []

if __name__ == '__main__':
	model = generate_model()
	x_train, y_train, x_test, y_test = load_dataset()

	model.compile(optimizer='SGD', loss='mse')

	print('[INFO ] Training started')
	model.fit(x_train, y_train, batch_size=32, epochs=10)
	print('[INFO ] Training finished')

	print(model.predict(np.array([[1, 0]]))[0])
