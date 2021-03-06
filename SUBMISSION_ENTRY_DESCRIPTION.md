# Bacon Gulch
## Approach Summary

We consider two possible strategies that the other agent (agent1) can follow: “focused” and “random.” The latter simply chooses a uniformly random action, while the former chooses the optimal path towards the pig. We use _bayesian inference_ to determine which is the strategy used by the other agent in the current round. Then, we perform planning on the induced Markov Decision Process (MDP) to choose the optimal action. We have chosen a planning-based approach over a learning-based approach since we consider that the latter is unnecessarily complex for the rather simple environment we have. The planning algorithm we used is a domain-adapted version of UCT [Kocsis & Szepesvári, 2006]. 

**Bayesian inference.** In a given state, each of the 2 possible strategies _t_ induces a probability distribution over actions _a_, P(a | t). We maintain a belief probability distribution over strategies, and update using Bayes’ rule. Our belief is seeded with the (known) prior probability of the agent being focused. If this were unknown, it could be estimated from the average posterior likelihood after many training episodes, starting from a uniform prior. This approach also leaves room for additional strategies that could be included if our algorithm were to face other agents with more complex behaviour. See [code](https://github.com/rhaps0dy/malmo-challenge/blob/high_score/MCTS/tsearch/__init__.py#L62).

**UCT.** The algorithm works by simulating stochastic trajectories from the initial state, using first the _tree_ and then the _default_ exploration policies. We adapt this multi-agent problem to single-agent UCT by considering the other agent as part of the environment. The other agent is simulated by using the current belief distribution over strategies. The pig behaviour could be learned and simulated too, but we have omitted this step due to time restrictions.

Since there are only 4ms between two game ticks, is essential that this part be very fast. Thus, we have implemented it in C++, and embedded it in Python module `tsearch`, which is in directory `MCTS`. We simulate 1000 full game instances in 1ms to 8ms. This is possible because of the small size of the field, which enables us to cache the shortest paths to the exits, the pig, and the optimal way to corner the pig assuming that the other agent will take the shortest path to it. In order to deal with set-ups where the pig cannot be cornered, we have explicitly encoded a waiting behaviour in the agent in order to let the pig move towards a more suitable position.

## Installation
In your normal malmo-challenge environment, and from the repository root. Run `pip install --upgrade ./MCTS` to install our C++ module. From the `pig_chase` directory, run `python bayes_agent.py -b 1000 -c 10.0` 

## Video Link
Click in this [link](https://www.dropbox.com/sh/ngm5fnsx04gwoa5/AABgrn8_W3PY-9g2gziqypRza?dl=0) to download the video. 
