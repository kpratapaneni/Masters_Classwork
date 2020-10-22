# -*- coding: utf-8 -*-
"""
Created on Mon Nov 25 08:54:40 2019

@author: 91903
"""
import matplotlib.pyplot as plt
import numpy as np

x=['1', '2', '4', '8']
f = open("D:/Masters/CS550/Project/data.txt", "r")
data=f.readlines()
markers=["*", "o", "^", ".", "x"]
count=0
namesOfPlots=["ZHT", "Memcached-Paper", "Memcached", "MongoDB", "Riak"]
for yRowsData in data:
    yAxisValues=[float(i) for i in (yRowsData.replace("\n", "").split(" "))]
    print(yAxisValues)
    plt.plot(x, yAxisValues, label=namesOfPlots[count], marker=markers[count])
    count+=1
plt.xlabel('(Number of Nodes)')
# Set the y axis label of the current axis.
plt.ylabel('(Throughput(OPs/s))')
# Set a title of the current axes.
plt.title('Overall Throughput for different systems')
plt.xticks(np.arange(5), ('1', '2', '4', '8'))
#plt.yticks(np.arange(0, 45001, step=3000))
# show a legend on the plot
plt.legend()
# Display a figure.
plt.show()

