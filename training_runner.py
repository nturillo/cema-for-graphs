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
                     num_generations=1000,
                     act_rndness_max=0.05)
    reward_library.delete_compositions(n)

    

    # r, A = train(compute_reward=brouwer,
    #              n=29,
    #              percent_learn=75,
    #              percent_survive=97.5,
    #              # act_rndness_wait=5,
    #              # act_rndness_max=0.05,
    #              num_generations=10000)

#    r, _ = train(compute_reward=triangle_conflict,
#                 num_generations=10000)

# r, A = train(compute_reward=ramsey_5_6,
#              n=58,
#              batch_size=200,
#              num_generations=10000,
#              percent_learn=92,
#              percent_survive=94,
#              learning_rate=0.0015,
#              neurons=[192,16],
#              act_rndness_init=0.0005,
#              act_rndness_max=0.0015)

# r, A = train(compute_reward=lambda1_nonregular,
#              n=40,
#              batch_size=400,
#              num_generations=5000,
#              percent_learn=95,
#              percent_survive=97.5,
#              neurons=[100,15])

# r, A = train(compute_reward=ti_graphs)
# r, A = train(compute_reward=iti_graphs, n=23)
# r, A = train(compute_reward=soltes_sum)
