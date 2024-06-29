# Copyright (c) 2019 Pierre Vigier

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

# Source: https://github.com/pvigier/dependency-graph



from cProfile import label
import os
import re
import argparse
import codecs
from collections import defaultdict
from graphviz import Digraph

include_regex = re.compile('#include\s+["<"](.*)[">]')
valid_headers = [['.h', '.hpp'], 'black']
valid_sources = [['.c', '.cc', '.cpp'], 'black']
valid_extensions = valid_headers[0] + valid_sources[0]


def normalize(path):
	""" Return the name of the node that will represent the file at path. """
	filename = os.path.basename(path)
	end = filename.rfind('.')
	end = end if end != -1 else len(filename)
	return filename[:end]


def get_extension(path):
	""" Return the extension of the file targeted by path. """
	return path[path.rfind('.'):]


def find_all_files(path, recursive=True):
	""" 
	Return a list of all the files in the folder.
	If recursive is True, the function will search recursively.
	"""
	files = []
	for entry in os.scandir(path):
		if entry.is_dir() and recursive:
			files += find_all_files(entry.path)
		elif get_extension(entry.path) in valid_extensions:
			files.append(entry.path)
	return files


def find_neighbors(path):
	""" Find all the other nodes included by the file targeted by path. """
	f = codecs.open(path, 'r', "utf-8", "ignore")
	code = f.read()
	f.close()
	return [normalize(include) for include in include_regex.findall(code)]


def create_graph(folder, create_cluster, label_cluster, strict):
	""" Create a graph from a folder. """
	# Find nodes and clusters
	files = find_all_files(folder)
	folder_to_files = defaultdict(list)
	for path in files:
		folder_to_files[os.path.dirname(path)].append(path)
	nodes = {normalize(path) for path in files}
	# Create graph
	graph = Digraph(strict=strict, graph_attr={'ratio':'1.8'})
	# Find edges and create clusters
	for folder in folder_to_files:
		with graph.subgraph(name='cluster_{}'.format(folder)) as cluster:
			for path in folder_to_files[folder]:
				color = 'black'
				node = normalize(path)
				ext = get_extension(path)
				neighbors = find_neighbors(path)

				if ext in valid_headers[0]:
					color = valid_headers[1]
				if ext in valid_sources[0]:
					color = valid_sources[1]
					
				if create_cluster:
					cluster.node(node)
				else:
					graph.node(node)
					
				for neighbor in neighbors:
					if neighbor != node and neighbor in nodes:
						graph.edge(node, neighbor, color=color)
						
			if create_cluster and label_cluster:
				cluster.attr(label=folder)
	return graph



graph = create_graph(folder='../Chemgine', create_cluster=False, label_cluster=False, strict=True)
graph.format = 'png' # 'bmp', 'gif', 'jpg', 'png', 'pdf', 'svg'
graph.render('../dependency', cleanup=True, view=True)