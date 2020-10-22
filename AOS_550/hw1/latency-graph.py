import matplotlib.pyplot as plt

def read_lines(filename):
	txt_map = {};
	with open(filename) as input_text:
		for x in input_text.readlines():
			temp = x.split()
			if(len(temp) == 2):
				txt_map[temp[0]] = float(temp[1])
			else:
				print("Invalid line", x)
	return txt_map

if __name__ == "__main__":
	txt_map_obj = read_lines("network-test-latency.txt")
	print(txt_map_obj.keys(), txt_map_obj.values())

	x_axis_labels = list(txt_map_obj.keys())
	y_axis_values = list(txt_map_obj.values())
	y_position = []
	for i in range(len(x_axis_labels)):
		y_position.append(i)

	plt.bar(y_position, y_axis_values, align='center', alpha=0.5)
	plt.xticks(y_position, x_axis_labels)
	plt.xlabel('Website')
	plt.ylabel('Latency')
	plt.title('Network Average Latency')

	plt.show()