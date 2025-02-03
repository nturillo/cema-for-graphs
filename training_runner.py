import numpy as np
import ctypes

reward_library = ctypes.CDLL('./reward.so')

reward_library.populate_compositions.argtypes = [ctypes.c_int]
reward_library.populate_compositions.restype = None
reward_library.delete_compositions.argtypes = [ctypes.c_int]
reward_library.delete_compositions.restype = None

reward_library.genus_minus_gonality_reward.argtypes = [ctypes.c_int, ctypes.POINTER(ctypes.c_int)]
reward_library.genus_minus_gonality_reward.restype = ctypes.c_double

def reward(n, A):
    return reward_library.genus_minus_gonality_reward(n, A.ctypes.data_as(ctypes.POINTER(ctypes.c_int)))

############################################################
# Training the cross entropy method agent on above rewards #
############################################################
if __name__=="__main__":
    from cema_train_simple_graph import train

    n = 10
    reward_library.populate_compositions(n)
    r, A = train(compute_reward=reward,
                     n=n,
                     num_generations=10000,
                     act_rndness_max=0.2)
    reward_library.delete_compositions(n)
