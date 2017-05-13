from __future__ import division, print_function

from common import ENV_AGENT_NAMES
from evaluation import PigChaseEvaluator
from environment import PigChaseSymbolicStateBuilder
from malmopy.agent import RandomAgent
import pickle
import sys

class RotateAgent(RandomAgent):
    _time = 0
    def act(self, state, reward, done, is_training=False):
        if done:
            print("Final Rotate time:", self._time)
            self._time = 0
        self._time += 1
        return 1

class ObserveAgent(RandomAgent):
    _prev_pig_pos = None
    _time = 0
    _pig_is_moving = False
    _pig_movement_history = []
    _pig_start_move_n = 0
    _total_ticks = 0
    _count = 0
    def act(self, state, reward, done, is_training=False):
        for s in state[1]:
            if s['name'] == 'Pig':
                pig_pos = (s['x'], s['z'])
        if self._prev_pig_pos is None:
            print("Initial pig position is", pig_pos)
        elif not self._pig_is_moving and self._prev_pig_pos != pig_pos:
            print("Pig moving at time", self._time)
            self._pig_is_moving = True
            self._prev_sm_pig_pos = pig_pos
            self._pig_start_move_n += 1
        elif self._pig_is_moving and self._prev_pig_pos == pig_pos:
            self._pig_is_moving = False
            print("Pig stopped at", pig_pos)
            self._pig_movement_history.append(pig_pos)
                #(pig_pos[0] - self._prev_sm_pig_pos[0],
                # pig_pos[1] - self._prev_sm_pig_pos[1]))
        self._prev_pig_pos = pig_pos

        if done:
            print("Final Observe time:", self._time)
            self._total_ticks += self._time

            self._time = 0
            self._prev_pig_pos = None
            self._pig_is_moving = False
            self._count += 1
            print("==== COUNT ====", self._count)
            if self._count >= 3:
                with open('pig_move_%s.pkl' % self.name, 'wb') as f:
                    pickle.dump((self._pig_start_move_n, self._total_ticks, self._pig_movement_history), f)
                sys.exit(0)
        else:
            self._time += 1
        return 1


if __name__ == '__main__':
    clients = [('127.0.0.1', 10000), ('127.0.0.1', 10001)]
    agent = ObserveAgent(ENV_AGENT_NAMES[1], 3)
    agent2 = RotateAgent(ENV_AGENT_NAMES[0], 3)

    eval = PigChaseEvaluator(clients, agent, agent, PigChaseSymbolicStateBuilder())
    eval.run(agent2)
