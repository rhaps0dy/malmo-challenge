# Copyright (c) 2017 Microsoft Corporation.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
# documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
#  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
# and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all copies or substantial portions of
# the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
# THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
#  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
# ===================================================================================================================

import os
import sys
from argparse import ArgumentParser
from datetime import datetime
from os import path
from threading import Thread, active_count
from time import sleep
import importlib

from environment import PigChaseSymbolicStateBuilder

try:
    from malmopy.visualization.tensorboard import TensorboardVisualizer
    from malmopy.visualization.tensorboard.cntk import CntkConverter
except ImportError:
    print('Cannot import tensorboard, using ConsoleVisualizer.')
    from malmopy.visualization import ConsoleVisualizer

# Enforce path
sys.path.insert(0, os.getcwd())
sys.path.insert(1, os.path.join(os.path.pardir, os.getcwd()))


def agent_factory(name, role, baseline_agent, clients, max_epochs,
                  logdir, visualizer, recording_path, epoch_size, AgentClass):

    assert len(clients) >= 2, 'Not enough clients (need at least 2)'
    clients = parse_clients_args(clients)

    if role == 0:
        env = PigChaseEnvironment(clients, PigChaseSymbolicStateBuilder(), role=role,
                                  randomize_positions=True, recording_path=None)
        agent = PigChaseChallengeAgent(name)

        if type(agent.current_agent) == RandomAgent:
            agent_type = PigChaseEnvironment.AGENT_TYPE_1
        else:
            agent_type = PigChaseEnvironment.AGENT_TYPE_2
        obs = env.reset(agent_type)
        reward = 0
        agent_done = False

        while True:
            # reset if needed
            if env.done:
                if type(agent.current_agent) == RandomAgent:
                    agent_type = PigChaseEnvironment.AGENT_TYPE_1
                else:
                    agent_type = PigChaseEnvironment.AGENT_TYPE_2
                obs = env.reset(agent_type)

            # select an action
            action = agent.act(obs, reward, agent_done, is_training=True)
            # take a step
            obs, reward, agent_done = env.do(action)


    else:
        env = PigChaseEnvironment(clients, AgentClass.StateBuilder(), role=role,
                                  randomize_positions=True,
                                  recording_path=recording_path)
        agent = AgentClass(name, env.actions, ENV_TARGET_NAMES[0], visualizer,
                           device)
        obs = env.reset()
        reward = 0
        agent_done = False
        viz_rewards = []

        max_training_steps = AgentClass.EPOCH_SIZE * max_epochs
        for step in range(1, max_training_steps+1):

            # check if env needs reset
            if env.done:

                visualize_training(visualizer, step, viz_rewards)
                viz_rewards = []
                obs = env.reset()

            # select an action
            action = agent.act(obs, reward, agent_done, is_training=True)
            # take a step
            obs, reward, agent_done = env.do(action)
            viz_rewards.append(reward)

            agent.inject_summaries(step)


def run_experiment(agents_def):
    assert len(agents_def) == 2, 'Not enough agents (required: 2, got: %d)'\
                % len(agents_def)

    processes = []
    for agent in agents_def:
        p = Thread(target=agent_factory, kwargs=agent)
        p.daemon = True
        p.start()

        # Give the server time to start
        if agent['role'] == 0:
            sleep(1)

        processes.append(p)

    try:
        # wait until only the challenge agent is left
        while active_count() > 2:
            sleep(0.1)
    except KeyboardInterrupt:
        print('Caught control-c - shutting down.')


if __name__ == '__main__':
    arg_parser = ArgumentParser('Pig Chase experiment')
    arg_parser.add_argument('-r', '--recording-path', type=str, default='',
                            help='Path to record the file on')
    arg_parser.add_argument('-a', '--agent', type=str,
                            default='run_agents.Focused',
                            help='Agent class to use')
    arg_parser.add_argument('-e', '--epochs', type=int, default=5,
                            help='Number of epochs to run.')
    arg_parser.add_argument('clients', nargs='*',
                            default=['127.0.0.1:10000', '127.0.0.1:10001'],
                            help='Minecraft clients endpoints (ip(:port)?)+')
    arg_parser.add_argument('-d', '--device', type=int, default=-1,
                            help='GPU device on which to run the experiment.')
    args = arg_parser.parse_args()

    agent_module = args.agent.split(".")
    mod = importlib.import_module(".".join(agent_module[:-1]))
    AgentClass = getattr(mod, agent_module[-1])

    logdir = AgentClass.log_dir(args, datetime.utcnow().isoformat())
    if 'malmopy.visualization.tensorboard' in sys.modules:
        visualizer = TensorboardVisualizer()
        visualizer.initialize(logdir, None)
    else:
        visualizer = ConsoleVisualizer()

    agents = [{'name': agent, 'role': role, 'AgentClass': AgentClass,
               'clients': args.clients, 'max_epochs': args.epochs,
               'logdir': logdir, 'visualizer': visualizer,
               'device': args.device}
              for role, agent in enumerate(ENV_AGENT_NAMES)]

    if args.recording_path != "":
        for a in agents:
            # Only record on the controlled agent
            if a['role'] == 1:
                a['recording_path'] = args.recording_path
    run_experiment(agents)
