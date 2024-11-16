# import jpype
import networkx as nx

import numpy as np

import ctypes

reward_library = ctypes.CDLL('./reward.so')

reward_library.edge_reward.argtypes = [ctypes.c_int, ctypes.POINTER(ctypes.c_int)]
reward_library.edge_reward.restype = ctypes.c_double

def edge_reward(n, A):
    return reward_library.edge_reward(n, A.ctypes.data_as(ctypes.POINTER(ctypes.c_int)))


def num_edges(G):
    return G.m()

############################################################
# Training the cross entropy method agent on above rewards #
############################################################
if __name__=="__main__":
    from cema_train_simple_graph import train

    for n in range(5, 10):
        r, A = train(edge_reward, # reward function
                     n=n,
                     num_generations=100)
