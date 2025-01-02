from copy import deepcopy

def parse_input():
    algorithm = input().strip()
    cost_iterations = int(input().strip())

    initial_config = [tuple(input().strip().split()) for _ in range(4)]
    goal_config = [tuple(input().strip().split()) for _ in range(4)]

    return algorithm, cost_iterations, tuple(initial_config), tuple(goal_config)



def expand_node(initial, blank):
    
    children= []
    i, j =blank

    moves = [
        (i - 1, j),
        (i + 1, j),
        (i, j - 1),
        (i, j + 1),
        (i - 2, j),
        (i + 2, j),
        (i, j - 2),
        (i, j + 2)
    ]

    for new_i, new_j, in moves:
        if 0 <= new_i < 4 and 0 <= new_j < 4:
            changed= change(initial, blank, (new_i, new_j))
            children.append((changed, (new_i, new_j)))

    return children


def expand_node_withcost(initial, blank):
    children = []
    i, j = blank

    moves = [
        (i - 1, j, 2),
        (i + 1, j, 2),
        (i, j - 1, 2),
        (i, j + 1, 2),
        (i - 2, j, 3),
        (i + 2, j, 3),
        (i, j - 2, 3),
        (i, j + 2, 3)
    ]

    for new_i, new_j, cost in moves:
        if 0 <= new_i < 4 and 0 <= new_j < 4:
            changed = change(initial, blank, (new_i, new_j))
            children.append((changed, (new_i, new_j), cost))

    return children


def change(initial, blank, changed):
    initial = deepcopy([list(row) for row in initial])
    i1, j1= blank
    i2, j2= changed
    initial[i1][j1], initial[i2][j2] = initial[i2][j2], initial[i1][j1]
    return tuple(tuple(i) for i in initial)
    

    
def manhattan(initial, goal):
    distance=0
    for i in range(4):
        for j in range(4):
            if initial[i][j]!="_" and initial[i][j]!=goal[i][j]:
                goal_i, goal_j = next(
                    (x, y) for x in range(4) for y in range(4) if goal[x][y] == initial[i][j]
                )
                distance += abs(i - goal_i) + abs(j - goal_j)
    return distance





def blank_location(initial):
    for row in range(4):
        for col in range(4):
            if initial[row][col] == "_":
                return (row, col)


def iterative_deepening(initial, goal, iterations):
    blank = blank_location(initial)

    for limit in range(iterations+1):
        result = limited_depth(initial, goal, limit, blank, [], set())
        if result != "Cutoff":
            print("SUCCESS\n")
            for i, state in enumerate(result):
                print_tree(state)
                if i < len(result)-1:
                    print()
            return

    print("FAILURE")


def limited_depth(initial, goal, limit, blank, path, visited):
    path.append(initial)

    if initial == goal:
        return path

    if limit == 0:
        path.pop()
        return "Cutoff"

    visited.add(initial)
    cutoff_occurred = False

    for child, new_blank in expand_node(initial, blank):
        if child not in visited:
            result = limited_depth(child, goal, limit - 1, new_blank, path, visited)
            if result == "Cutoff":
                cutoff_occurred = True
            elif result != "Cutoff":
                return result

    visited.remove(initial)
    path.pop()

    if cutoff_occurred:
        return "Cutoff"
    else:
        return "Failure"
    

def algorithma(initial, goal, cost_iterations):
    blank = blank_location(initial)
    visited = {}


    tobevisited = [(0, 0, initial, blank, [])] #distance, cost

    while tobevisited:
        tobevisited.sort(key=lambda x: x[0])  
        f, g, current, blank, path = tobevisited.pop(0)

        if f > cost_iterations:
            continue

        if current in visited and visited[current] <= g:
            continue

        path = path + [current]
        visited[current] = g 

        if current == goal:
            print("SUCCESS\n")
            for i, state in enumerate(path):
                print_tree(state)
                if i < len(path)-1:
                    print()
            return

        for child, new_blank, move_cost in expand_node_withcost(current, blank):
            if child not in visited:
                new_g = g + move_cost
                new_f = new_g + manhattan(child, goal)

                if new_f <= cost_iterations:
                    tobevisited.append((new_f, new_g, child, new_blank, path))

    print("FAILURE")



                
def print_tree(board):
    for row in board:
        print(" ".join(str(j) for j in row))


    
    
        
    


def main():
    algorithm, cost_iterations, initial_config, goal_config = parse_input()

    if algorithm == "DIFDS":
        iterative_deepening(initial_config, goal_config, cost_iterations)

    if algorithm == "A*":
        algorithma(initial_config, goal_config, cost_iterations)

if __name__ == "__main__":
    main()