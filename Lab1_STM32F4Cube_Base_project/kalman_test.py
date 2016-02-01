import numpy as np
class KalmanFilter(object):
	q = np.float32(0.0)
	r = np.float32(0.0)
	x = np.float32(0.0)
	p = np.float32(0.0)
	k = np.float32(0.0)

	def __init__(self, q, r, p=np.float32(0.0), k=np.float32(0.0), initial_value=np.float32(0.0)):
		self.q = q
		self.r = r
		self.p = p
		self.x = initial_value

	def update(self, measurement):
		self.p = np.float32(self.p +self.q)
		self.k = np.float32(self.p / (self.p + self.r))
		self.x = np.float32(self.x + self.k * (measurement - self.x))
		self.p = np.float32((1-self.k)* self.p)

		return "p: " +str(self.p) + ", k: " +str(self.k) + ", output x: "+ str(self.x)

kalman1 = KalmanFilter(q=0.1, r=0.1);

inputArray =np.array([0.1,0.1,0.2,0.3,0.4], dtype='f')
outputArray = []


for i in range(0,len(inputArray)):
	print kalman1.update (inputArray[i])
