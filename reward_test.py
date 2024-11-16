import ctypes
import numpy as np
import networkx as nx
import matplotlib.pyplot as plt

reward_library = ctypes.CDLL('./reward.so')

reward_library.choose.argtypes = [ctypes.c_int, ctypes.c_int]
reward_library.choose.restype = ctypes.c_int

# print(reward_library.choose(20, 9))

reward_library.populate_compositions.argtypes = [ctypes.c_int]
reward_library.populate_compositions.restype = None

reward_library.delete_compositions.argtypes = [ctypes.c_int]
reward_library.delete_compositions.restype = None


reward_library.print_compositions.argtypes = [ctypes.c_int]
reward_library.print_compositions.restype = None

# reward_library.print_compositions(5)

reward_library.gonality.argtypes = [ctypes.c_int, ctypes.POINTER(ctypes.c_int)]
reward_library.gonality.restype = ctypes.c_int

reward_library.genus.argtypes = [ctypes.c_int, ctypes.POINTER(ctypes.c_int)]
reward_library.genus.restype = ctypes.c_int

reward_library.genus_minus_gonality_reward.argtypes = [ctypes.c_int, ctypes.POINTER(ctypes.c_int)]
reward_library.genus_minus_gonality_reward.restype = ctypes.c_double


for n in range(2, 11):
    reward_library.populate_compositions(n)
    g = nx.cycle_graph(n)
    A = np.array(nx.adjacency_matrix(g).todense(), dtype=np.int8)
    # print("A: {")
    # for row in A:
    #     for elem in row:
    #         print(elem, end=", ")
    # print("}")
    genus_minus_gonality = reward_library.genus_minus_gonality_reward(n, A.ctypes.data_as(ctypes.POINTER(ctypes.c_int)))
    
    print(A)
    print(f"n: {n}")
    print(f"Genus minus gonality of n={n}: {genus_minus_gonality}")
    print(f"Gonality: {reward_library.gonality(n, A.ctypes.data_as(ctypes.POINTER(ctypes.c_int)))}")
    print(f"Genus: {reward_library.genus(n, A.ctypes.data_as(ctypes.POINTER(ctypes.c_int)))}")
    print(f"Connected: ", nx.is_connected(g))
    print()

    reward_library.delete_compositions(n)