#!/usr/bin/env python3
from __future__ import division, print_function

from argparse import ArgumentParser
import pickle
import sys
import os
import datetime
import numpy as np

from common import ENV_AGENT_NAMES
from environment import PigChaseSymbolicStateBuilder
from malmopy.agent import RandomAgent, BaseAgent
try:
    from malmopy.visualization.tensorboard import TensorboardVisualizer
except ImportError:
    print('Cannot import tensorboard, using ConsoleVisualizer.')
    from malmopy.visualization import ConsoleVisualizer

import tsearch

class BayesAgent(BaseAgent):
    PRIORS = np.array([0.75, 0.25])
    def __init__(self, name, n_actions, visualizer=None):
        super(BayesAgent, self).__init__(name, n_actions, visualizer)
        self.bp = tsearch.BayesianPlanner()

        self._prev_state = None
        self.bp.reset(self.PRIORS)

        self.cumul_reward = 0
        self.n_steps = 0
        self._prev_pig_location = None
        self._done_pig_location = None

    def act(self, symbolic_state, reward, done, is_training=False):
        if symbolic_state is None:
            cur_state = self._prev_state
            self._prev_state = None
        else:
            cur_state = [None]*8
            for entity in symbolic_state[1]:
                if entity['name'] == 'Pig':
                    cur_state[0] = int(entity['z'])
                    cur_state[1] = int(entity['x'])
                    location = (entity['z'], entity['x'])
                    if location == self._prev_pig_location and location != self._done_pig_location:
                        print("Pig location:", location)
                        self._done_pig_location = location
                    self._prev_pig_location = location
                else:
                    i = (int(entity['name'][-1])-1)*3
                    cur_state[i+2] = int(entity['z'])
                    cur_state[i+3] = int(entity['x'])
                    # Minecraft yaw to 0=north, 1=east.. taken from agent.py
                    cur_state[i+4] = ((((int(entity['yaw']) - 45) % 360) // 90) - 1) % 4

            if None in cur_state:
                for i in xrange(len(cur_state)):
                    if cur_state[i] is None:
                        cur_state[i] = self._prev_state[i]
            elif self._prev_state is not None:
                self.bp.infer_strategy_proba(self._prev_state, cur_state)
            self._prev_state = cur_state

        self.cumul_reward += reward
        self.n_steps += 1
        if done:
            print("Final strats:", self.bp._strats, "Real reward:", self.cumul_reward/self.n_steps)
            self.cumul_reward = 0
            self.n_steps = 0
            self.bp.reset(self.PRIORS)
        print(self.bp._strats)
        return self.bp.plan_best_action(cur_state, budget=1000, exploration_constant=20.0)

    @staticmethod
    def log_dir(args, dtime):
        return 'results/ours/{:s}'.format(dtime)

class BayesAgentWrapper(BayesAgent):
    EPOCH_SIZE = 100
    StateBuilder = PigChaseSymbolicStateBuilder
    def __init__(self, name, actions, pig, visualizer, device):
        super(BayesAgentWrapper, self).__init__(name, actions, visualizer)

if __name__ == '__main__':
    from evaluation import PigChaseEvaluator
    arg_parser = ArgumentParser('Bayesian inference and planning agent evaluation')
    arg_parser.add_argument('-a', '--agent', type=str, default='BayesAgent',
                            help='Agent class to use')
    arg_parser.add_argument('-l', '--log-dir', type=str, default='logs/bp',
                            help='Directory to store logs in')
    args = arg_parser.parse_args()
    clients = [('127.0.0.1', 10000), ('127.0.0.1', 10001)]

    AgentClass = locals()[args.agent]
    logdir = AgentClass.log_dir(args, datetime.datetime.utcnow().isoformat())
    if 'malmopy.visualization.tensorboard' in sys.modules:
        visualizer = TensorboardVisualizer()
        visualizer.initialize(logdir, None)
    else:
        visualizer = ConsoleVisualizer()
    agent = AgentClass(ENV_AGENT_NAMES[1], 3, visualizer=visualizer)
    eval = PigChaseEvaluator(clients, agent, agent, PigChaseSymbolicStateBuilder())
    eval.run()
