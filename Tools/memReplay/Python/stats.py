import math

def arithmetic_mean(a):
	return math.fsum(a) / float(len(a))

def covariance(a, b):
	invN = 1.0 / float(len(a));

	xyMn = 0.0
	xMn = 0.0
	yMn = 0.0

	for i in xrange(0, len(a)):
		dx = float(a[i])
		dy = float(b[i])

		xMn += dx * invN
		yMn += dy * invN
		xyMn += (dx * dy) * invN

	return xyMn - xMn * yMn

def standard_deviation(a):
	mn = arithmetic_mean(a);
	invN = 1.0 / float(len(a));

	sdMn = 0.0

	for i in xrange(0, len(a)):
		dv = float(a[i]) - mn
		dvSq = dv * dv
		sdMn += dvSq * invN

	return math.sqrt(sdMn)

def lls(seq):
	seqLen = len(seq)
	if seqLen > 0:
		invN = 1.0 / float(seqLen)

		xyMean = 0.0
		xMean = 0.0
		yMean = 0.0
		xSqMean = 0.0

		for xi in xrange(0, len(seq)):
			x = float(xi)
			y = float(seq[xi])

			xy = x * y
			xSq = x * x

			xyMean += xy * invN
			xMean += x * invN
			yMean += y * invN
			xSqMean += xSq * invN

		betaEst = (xyMean - xMean * yMean) / (xSqMean - xMean * xMean)
		alphaEst = yMean - betaEst * xMean

		result = [alphaEst + betaEst * x for x in xrange(0, len(seq))]

		cov = covariance(seq, result)
		sdPlot = standard_deviation(seq)
		sdLLS = standard_deviation(result)

		sdPlotLLS = sdPlot * sdLLS
		if abs(sdPlotLLS) > 0.0000001:
			correlationCoeff = cov / sdPlotLLS
		else:
			correlationCoeff = 0
		determinationCoeff = correlationCoeff * correlationCoeff

		return (betaEst, determinationCoeff)
	else:
		return (0, 0)

def gaussian(x, sigma):
	return math.exp(-(x*x) / (2.0 * sigma * sigma))

def gaussian_kernel(sigma):
	sigma = int(sigma)
	width = sigma * 6 - 1

	kernel = [gaussian(float(x), sigma) for x in range(-width, width + 1)]
	kernelSum = sum(kernel)
	invSum = 1.0 / kernelSum

	normalizedKernel = [x * invSum for x in kernel]
	return normalizedKernel

def convolve(v, kernel):
	vLen = len(v)
	kernelLen = len(kernel)
	def cv(base, kernel):
		left = base - kernelLen / 2
		right = left + kernelLen

		r = 0.0
		if left >= 0 and right <= vLen:
			for x in range(0, kernelLen):
				r += kernel[x] * v[left + x]
		else:
			for x in range(0, kernelLen):
				if left + x < 0:
					r += kernel[x] * v[0]
				elif left + x >= vLen:
					r += kernel[x] * v[vLen - 1]
				else:
					r += kernel[x] * v[left + x]

		return r

	return [cv(i, kernel) for i in xrange(0, len(v))]

