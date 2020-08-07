import json 
import networkx as nx


def get_json(path):
    file = open(path, "r")
    json_ = json.loads(file.read())
    return json_

def create_graph(path):
    json_graph = get_json(path)
    vertices = json_graph["num vertices"]
    G = nx.Graph()
    G.add_nodes_from(range(vertices))
    G.add_edges_from(json_graph["edges"])
    return G
