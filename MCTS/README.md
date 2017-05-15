# MCTS to-do
 * Implement the Python glue.
 * Split node into UCT and A*, or something, so that UCT can use the copy
   pointer and A* can use unique_ptr which is quite fast. Or allocate in a
   vector and have an array of pointers.
 * Make the MCTS policy follow a few plausible A* paths with a high probability.
   This will obtain better results with less samples, unsure about speed.
 * Find some way to re-use nodes from previous real-world steps; especially if
   the children nodes have many more samples than the parents.
 * Incorporate learned pig movements, with float pig position, when the pig
   started moving, and maybe player positions.
 * If necessary: make MCTS parallel. LOW PRIORITY.
