#!/usr/bin/env python3
from __future__ import division, print_function

from argparse import ArgumentParser
import pickle
import sys
import os
from time import time as get_current_time

from common import ENV_AGENT_NAMES
from evaluation import PigChaseEvaluator
from environment import PigChaseSymbolicStateBuilder
from malmopy.agent import RandomAgent, BaseAgent

import tsearch

class BayesAgent(BaseAgent):
    PRIORS = np.array([0.75, 0.25])
    def __init__(self, name, n_actions, visualizer=None):
        super(BaseAgent, self).__init__(name, n_actions, visualizer)
        self.bp = tsearch.BayesianPlanner([tsearch.focused, tsearch.random])

        self._prev_state = None
        self.bp.reset(self.PRIORS)

    def act(self, symbolic_state, reward, done, is_training=False):
        cur_state = [None]*8
        for entity in symbolic_state[1]:
            if entity['name'] == 'Pig':
                cur_state[0] = int(entity['x'])
                cur_state[1] = int(entity['z'])
            else:
                i = (int(entity['name'][-1])-1)*3
                cur_state[i+2] = int(entity['x'])
                cur_state[i+3] = int(entity['z'])
                # Minecraft yaw to 0=north, 1=east.. taken from agent.py
                cur_state[i+4] = ((((int(entity['yaw']) - 45) % 360) // 90) - 1) % 4

        if self._prev_state is not None:
            self.bp.infer_strategy_proba(self._prev_state, cur_state)
        if done:
            self._prev_state = None
            self.bp.reset(self.PRIORS)
        else:
            self._prev_state = cur_state
        return self.bp.plan_best_action(cur_state)

    def log_dir(args, date):
        return os.path.join(args.log_dir, date)


if __name__ == '__main__':
    arg_parser = ArgumentParser('Bayesian inference and planning agent evaluation')
    arg_parser.add_argument('-a', '--agent', type=str, default='BayesAgent',
                            help='Agent class to use')
    arg_parser.add_argument('-l', '--log-dir', type=str, default='./logs/bp',
                            help='Directory to store logs in')
    args = arg_parser.parse_args()

    clients = [('127.0.0.1', 10000), ('127.0.0.1', 10001)]
    AgentClass = locals()[args.agent]
    logdir = AgentClass.log_dir(args, datetime.utcnow().isoformat())
    if 'malmopy.visualization.tensorboard' in sys.modules:
        visualizer = TensorboardVisualizer()
        visualizer.initialize(logdir, None)
    else:
        visualizer = ConsoleVisualizer()
    agent = AgentClass(ENV_AGENT_NAMES[1], 3, visualizer=visualizer)
    eval = PigChaseEvaluator(agent, agent, PigChaseSymbolicStateBuilder())
    eval.run()
