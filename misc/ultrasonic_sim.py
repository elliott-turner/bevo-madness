# simulate data for ultrasonic measurements taken at 1 deg increments across 360 deg
# from random position and orientation in starting zone

import random
import math
import matplotlib.pyplot as plt

# noise (inches)
noise = 0.8
linearity = 0.08 # increase in noise per unit distance

# court information
court_width = 72.0
court_length = 72.0

# starting zone information
start_width = 36.0
start_length = 18.0

# pick a random starting coordinate and orientation
x = random.random() * start_width + court_width / 2 - start_width / 2
y = random.random() * start_length
theta = random.randint(-180, 179)

measurements = []

# generate distance measurements
t = math.radians(theta)
for i in range(360):
    if math.atan2(court_length - y, 0 - x) >= t >= math.atan2(court_length - y, court_width - x): # facing front wall
        measurements.append((court_length - y) / math.sin(t))
    elif math.atan2(court_length - y, court_width - x) >= t >= math.atan2(0 - y, court_width - x): # facing right wall
        measurements.append((court_width - x) / math.cos(t))
    elif math.atan2(0 - y, court_width - x) >= t >= math.atan2(0 - y, 0 - x): # facing back wall
        measurements.append((0 - y) / math.sin(t))
    else: # facing left wall
        measurements.append((0 - x) / math.cos(t))

    t += math.radians(1)
    if math.degrees(t) > 179: t = math.radians(-180)

# add noise
measurements = [d + random.uniform(-noise, noise) + linearity * d * random.uniform(-1.0, 1.0) for d in measurements]

fig = plt.figure()
ax1 = plt.subplot(121)
ax2 = plt.subplot(122, projection='polar')
ax1.scatter([t for t in range(360)], measurements)
ax2.scatter([math.radians(t) for t in range(360)], measurements)
plt.show()
