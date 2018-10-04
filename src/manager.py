# Import
#-------------------------------------------#
import sys
import csv
import numpy as np
from collections import deque


# Functions
#-------------------------------------------#

def get_path(n_inputs):
	try:
		input_path = str(sys.argv[1])
		result_path = str(sys.argv[2])
		return input_path, result_path
	except:
		input_path = '../public_cases/'
		input_path_list = []
		result_path_list = []
		for i in range(n_inputs):
			input_path_list.append(input_path + 'input_' + str(i+1) +'.txt')
			result_path_list.append(input_path + 'output' + str(i+1) +'.txt')
		return input_path_list, result_path_list


def read_file(path, delimiter=' ', quotechar='\r'):
	with open(path, 'r', encoding='utf-8') as f:
		file = csv.reader(f, delimiter=delimiter, quotechar=quotechar)
		input_table = []
		for row in file:
			new_dir = []
			for i, item in enumerate(row):
				if item != '' and i != 0:
					new_dir.append(item)
			new_dir = deque(new_dir)
			while new_dir[-1] == '00': # discard all trailing 00's
				new_dir.pop() 
				if len(new_dir) < 1: break 
			input_table.append(new_dir) # use deque for popleft(): higher performance popping from left end of the list
		assert len(input_table) == 4
	return input_table


class Manager(object):

	def __init__(self, input_table=None):
		self.input_table = input_table
		self.rank_table = self.get_rank_table()
		self.conf_table = self.get_conflict_table()
		self.queue = [[], [], [], []]
		self.result = [[], [], [], []]
		self.delay = 0


	def get_rank_table(self):
		"""
		ranking: 3 > 2 > 1
		"""
		rank_table_N = {
			'1W' : 1,
			'1S' : 2,
			'1E' : 3   }
		rank_table_E = {
			'1N' : 1,
			'1W' : 2,
			'1S' : 3   }
		rank_table_S = {
			'1E' : 1,
			'1N' : 2,
			'1W' : 3   }
		rank_table_W = {
			'1S' : 1,
			'1E' : 2,
			'1N' : 3   }
		rank_table = [rank_table_N, rank_table_E, rank_table_S, rank_table_W]
		return rank_table


	def get_conflict_table(self):
		"""
			|	|		
		-   1   2  -
		-   3   4  -  map to: [1, 2, 3, 4]
			|	|
		"""
		conf_table_N = {
			'1W' : np.asarray([1, 0, 0, 0]),
			'1S' : np.asarray([1, 0, 1, 0]),
			'1E' : np.asarray([1, 0, 1, 1]),
			'00' : np.zeros((4)) }
		conf_table_E = {
			'1N' : np.asarray([0, 1, 0, 0]),
			'1W' : np.asarray([1, 1, 0, 0]),
			'1S' : np.asarray([1, 1, 1, 0]),
			'00' : np.zeros((4)) }
		conf_table_S = {
			'1E' : np.asarray([0, 0, 0, 1]),
			'1N' : np.asarray([0, 1, 0, 1]),
			'1W' : np.asarray([1, 1, 0, 1]),
			'00' : np.zeros((4)) }
		conf_table_W = {
			'1S' : np.asarray([0, 0, 1, 0]),
			'1E' : np.asarray([0, 0, 1, 1]),
			'1N' : np.asarray([0, 1, 1, 1]),
			'00' : np.zeros((4)) }
		conf_table = [conf_table_N, conf_table_E, conf_table_S, conf_table_W]
		return conf_table


	def check_queue(self):
		is_empty = [False, False, False, False]
		for i in range(len(self.queue)):
			if len(self.queue[i]) == 0:
				is_empty[i] = True
		return is_empty


	def get_next(self, is_empty):
		if is_empty[0] and len(self.input_table[0]) > 0: self.queue[0].append(self.input_table[0].popleft())
		elif not is_empty[0]: 
			self.delay = self.delay + 1
			if len(self.input_table[0]) > 0: # prevent indexing empty table
				try: self.input_table[0].remove('00')  # if the previous car is delayed, remove the first encounter trailing 00
				except ValueError: pass

		if is_empty[1] and len(self.input_table[1]) > 0: self.queue[1].append(self.input_table[1].popleft())
		elif not is_empty[1]:
			self.delay = self.delay + 1
			if len(self.input_table[1]) > 0:
				try: self.input_table[1].remove('00') 
				except ValueError: pass

		if is_empty[2] and len(self.input_table[2]) > 0: self.queue[2].append(self.input_table[2].popleft())
		elif not is_empty[2]: 
			self.delay = self.delay + 1
			if len(self.input_table[2]) > 0:
				try: self.input_table[2].remove('00') 
				except ValueError: pass

		if is_empty[3] and len(self.input_table[3]) > 0: self.queue[3].append(self.input_table[3].popleft())
		elif not is_empty[3]: 
			self.delay = self.delay + 1
			if len(self.input_table[3]) > 0:
				try: self.input_table[3].remove('00') 
				except ValueError: pass

		assert len(self.queue[0]) <= 1 and len(self.queue[1]) <= 1 and len(self.queue[2]) <=  1 and len(self.queue[3]) <= 1


	def reset(self, input_table):
		self.input_table = input_table
		self.queue = [[], [], [], []]
		self.result = [[], [], [], []]
		self.delay = 0


	def input_and_queue_not_empty(self):
		for i in range(len(self.input_table)):
			if len(self.input_table[i]) > 0 or len(self.queue[i]) > 0:
				return True
		return False


	def check_conflict(self):
		is_conf = [False, False, False, False]
		for i in range(len(self.queue)):
			if len(self.queue[i]) > 0: # if queue not empty, check for conflict
				check = self.conf_table[i][self.queue[i][0]]
				for j in range(len(self.queue)):
					if i != j and len(self.queue[j]) > 0:  # avoid checking itself and empty queue
						check = check + self.conf_table[j][self.queue[j][0]]
						for k in range(len(check)):
							if check[k] > 1:
								is_conf[i] = True
						if is_conf[i] == True: break
						check = self.conf_table[i][self.queue[i][0]] # reset, check next direction
		return is_conf


	def decide(self, rank_list):
		if np.sum(rank_list) == 0: return False, None # when no conflict ones
		decision = np.argmax(rank_list) # Returns the indices of the maximum values, in case of multiple occurrences of the maximum values, the indices corresponding to the first occurrence are returned
		for i in range(4):
			if i != decision and rank_list[i] == rank_list[decision]: # check other idxs incase of same rank
				if len(self.input_table[i]) > len(self.input_table[decision]): # update decision if: 1. same rank and 2. longer queue
					decision = i
		assert 0 <= decision <= 3
		return True, decision


	def process(self, message='Processing input file...'):
		print(), print(message)
		count = 0
		while self.input_and_queue_not_empty():
			# initiate
			count = count + 1
			print('Processing, iteration: %i' % count, end='\r')
			output_list = ['00', '00', '00', '00'] # initiate output list for this time step
			rank_list = [0, 0, 0, 0] # initiate rank list

			# process queue
			self.get_next(self.check_queue()) # fill the queue
			
			# check conflict
			is_conf = self.check_conflict()
			for i in range(4):
				if is_conf[i] == True:
					if len(self.input_table[i]) == 0: rank_list[i] = 1
					elif self.input_table[i][0] == '00': rank_list[i] = 1
					else: rank_list[i] = self.rank_table[i][self.queue[i][0]] # gather the conflict ones
				elif len(self.queue[i]) > 0: # if queue not empty
					output_list[i] = self.queue[i][0] # store to output list
					self.queue[i] = [] # empty that direction's queue
			
			# resolve conflict
			conf_exist, decision = self.decide(rank_list) # decide which one to output
			if conf_exist:
				output_list[decision] = self.queue[decision][0] # store to output list
				self.queue[decision] = [] # empty that direction's queue

			# save output to result queue
			for i in range(4):
				self.result[i].append(output_list[i])
		print()


	def display_result(self, message='[Results]'):
		print(message)
		print('Total delay: ', self.delay)
		directions = ['N', 'E', 'S', 'W']
		for i, row in enumerate(self.result):
			print(directions[i], row)
		print('=========================================')
		return self.result


def save_result(result, result_path):
	with open(result_path, 'w', encoding='big5') as f:
		file = csv.writer(f, delimiter=' ', quotechar='\r')
		directions = ['N:', 'E:', 'S:', 'W:']
		for i, row in enumerate(result):
			row.insert(0, directions[i])
			file.writerow(row)
	print('Results have been successfully saved to: %s' % (result_path))



# 2, 25, 37, 16, 39
# 2, 28, 25, 16, 39
#-------------------------------------------#

def main():
	input_path_list, result_path_list = get_path(n_inputs=5)

	manager = Manager()

	SELECT = bool(1) # set to True if runing single file only => bool(1)
	select_file = 2 # file index to run individually
	assert 0 <= select_file <= 4

	for i in range(len(input_path_list)):

		if SELECT: i = select_file
		#----------------------------------------------------------------------------------#
		input_table = read_file(input_path_list[i])
		manager.reset(input_table)

		manager.process(message='Processing file: ' + input_path_list[i])
		result = manager.display_result(message='[Results] for file: ' + input_path_list[i])

		save_result(result, result_path_list[i])
		#----------------------------------------------------------------------------------#
		if SELECT: break

	
#-------------------------------------------#

if __name__ == "__main__":
	main()

	