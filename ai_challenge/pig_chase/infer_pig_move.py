from __future__ import division, print_function

from common import ENV_AGENT_NAMES
from evaluation import PigChaseEvaluator
from environment import PigChaseSymbolicStateBuilder
from malmopy.agent import RandomAgent
import pickle
import sys
from time import time as get_current_time

class RotateAgent(RandomAgent):
    _steps = 0
    def act(self, state, reward, done, is_training=False):
        if done:
            print("Final Rotate time:", self._steps)
            self._steps = 0
        self._steps += 1
        return 1

def dist(a, b):
    return ((b[0]-a[0])**2 + (b[1]-a[1])**2)**.5

class ObserveAgent(RandomAgent):
    _prev_pig_pos = None
    _steps = 0
    _pig_is_moving = False
    _pig_movement_history = []
    _pig_start_move_n = 0
    _total_ticks = 0
    _count = 0
    _speed_samples = []
    _time_samples = []
    _prev_time = None
    def act(self, state, reward, done, is_training=False):
        time = get_current_time()
        for s in state[1]:
            if s['name'] == 'Pig':
                pig_pos = (s['x'], s['z'])
        if self._prev_pig_pos is None:
            #print("Initial pig position is", pig_pos)
            pass
        elif self._prev_pig_pos != pig_pos:
            if not self._pig_is_moving:
                #print("Pig moving at time", self._steps)
                self._pig_is_moving = True
                self._pig_start_move_n += 1
            self._speed_samples.append(dist(self._prev_pig_pos, pig_pos)/(time-self._prev_time))
        else:
            if self._pig_is_moving:
                #print("Pig stopped at", pig_pos)
                self._pig_is_moving = False
                self._pig_movement_history.append(pig_pos)
        self._prev_pig_pos = pig_pos
        if self._prev_time is not None:
            self._time_samples.append(time-self._prev_time)
        self._prev_time = time

        if done:
            print("Final Observe time:", self._steps)
            self._total_ticks += self._steps

            self._steps = 0
            self._prev_time = None
            self._prev_pig_pos = None
            self._pig_is_moving = False
            self._count += 1
            print("==== COUNT ====", self._count)
            if self._count >= 3:
                with open('pig_move_%s.pkl' % self.name, 'wb') as f:
                    pickle.dump({
                        "start_move_n": self._pig_start_move_n,
                        "total_ticks": self._total_ticks,
                        "speed_samples": self._speed_samples,
                        "time_samples": self._time_samples,
                        "movement_history": self._pig_movement_history}, f)
                sys.exit(0)
        else:
            self._steps += 1
        return 1


if __name__ == '__main__':
    clients = [('127.0.0.1', 10000), ('127.0.0.1', 10001)]
    agent = ObserveAgent(ENV_AGENT_NAMES[1], 3)
    agent2 = RotateAgent(ENV_AGENT_NAMES[0], 3)

    eval = PigChaseEvaluator(clients, agent, agent, PigChaseSymbolicStateBuilder())
    eval.run(agent2)
