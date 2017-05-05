import malmopy
from malmopy.agent import LinearEpsilonGreedyExplorer, RandomAgent, TemporalMemory
from malmopy.model.chainer import QNeuralNetwork, DQNChain

import agent
from environment import PigChaseSymbolicStateBuilder, PigChaseTopDownStateBuilder


class Random(malmopy.agent.RandomAgent):
    EPOCH_SIZE = 100
    StateBuilder = PigChaseSymbolicStateBuilder
    def __init__(self, name, actions, pig, visualizer, device):
        super(Random, self).__init__(name, actions)
    @staticmethod
    def log_dir(args, dtime):
        return 'results/baselines/random/%s'.format(dtime)

class Focused(agent.FocusedAgent):
    EPOCH_SIZE = 100
    StateBuilder = PigChaseSymbolicStateBuilder
    def __init__(self, name, actions, pig, visualizer, device):
        super(Focused, self).__init__(name, pig)
    @staticmethod
    def log_dir(args, dtime):
        return 'results/baselines/focused/%s'.format(dtime)

class TopDownDQN(agent.PigChaseQLearnerAgent):
    EPOCH_SIZE = 100000
    StateBuilder = lambda: PigChaseTopDownStateBuilder(True)
    def __init__(self, name, actions, pig, visualizer, device):
        memory = TemporalMemory(100000, (18, 18))
        chain = DQNChain((memory.history_length, 18, 18), actions)
        target_chain = DQNChain((memory.history_length, 18, 18), actions)
        model = QNeuralNetwork(chain, target_chain, device)
        explorer = LinearEpsilonGreedyExplorer(1, 0.1, 1000000)
        super(TopDownDQN, self).__init__(name, actions, model, memory,
                                         0.99, 32, 50000,
                                         explorer=explorer,
                                         visualizer=visualizer)
    @staticmethod
    def log_dir(args, dtime):
        return 'results/baselines/topdownDQN/%s'.format(dtime)


class DQN(agent.PigChaseQLearnerAgent):
    EPOCH_SIZE = 100000
    StateBuilder = malmopy.environment.malmo.MalmoALEStateBuilder
    def __init__(self, name, actions, pig, visualizer, device):
        memory = TemporalMemory(100000, (84, 84))
        chain = DQNChain((memory.history_length, 84, 84), actions)
        target_chain = DQNChain((memory.history_length, 84, 84), actions)
        model = QNeuralNetwork(chain, target_chain, device)
        explorer = LinearEpsilonGreedyExplorer(1, 0.1, 1000000)
        super(TopDownDQN, self).__init__(name, actions, model, memory,
                                         0.99, 32, 50000,
                                         explorer=explorer,
                                         visualizer=visualizer)
    @staticmethod
    def log_dir(args, dtime):
        return 'results/baselines/DQN/%s'.format(dtime)
