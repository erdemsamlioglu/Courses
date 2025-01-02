
def make_tree_birliler(part_list):
    handled_parts_list = []
    new_part_list = []
    for i in range(len(part_list)):
        if len(part_list[i]) == 2 and type(part_list[i][1]) == float and part_list[i][0] not in handled_parts_list:
            # poplanan =  part_list.pop(i)
            # print ("poplanan: ", part_list[i])
            part = part_list[i][0]
            handled_parts_list.append(part)
            price = part_list[i][1]
            for j in range(len(part_list)):
                found = False
                if i != j and type(part_list[j][1]) == tuple:
                    for m in range(1, len(part_list[j])):
                        # print("part_list[j][m]", part_list[j][m])
                        if part_list[j][m][1] == part:
                            location_row = j
                            location_inside = m
                            found = True
                            break
                if found == True:
                    break
            count = part_list[location_row].pop(location_inside)[0]
            part_list[location_row].append([count, part, price])

    for k in part_list:
        if len(k) != 2 or type(k[1]) != float:
            new_part_list.append(k)
    # print(handled_parts_list)
    return new_part_list


def make_tree_coklular(part_list):
    all_set = False
    if not part_list:
        return []
    elif len(part_list)==1:
        return part_list.pop()
    else:
        last_part = -5
        while (all_set == False):
            # print("while'a girdi")
            for i in range(len(part_list)):
                all_known = True
                for m in range(1, len(part_list[i])):
                    if len(part_list[i][m]) < 3:
                        all_known = False
                        break
                if all_known == True:
                    part = part_list[i][0]

                    # print(part)
                    for j in range(len(part_list)):
                        if i != j:
                            for it in range(1, len(part_list[j])):
                                if type(part_list[j][it]) == tuple and part_list[j][it][1] == part:
                                    count = part_list[j].pop(it)[0]
                                    part_list[j].append([count])
                                    last_part = j
                                    for element in part_list[i]:
                                        part_list[j][-1].append(element)

            all_non_tuple = True
            for p in range(len(part_list)):
                for s in range(1, len(part_list[p])):
                    if type(part_list[p][s]) == tuple:
                        all_non_tuple = False
                        break
            if (all_non_tuple == True):
                all_set = True

        return part_list[last_part]


def make_tree(part_list):
    copy = []
    for b in range(len(part_list)):
        c = []
        for a in range(len(part_list[b])):
            c.append(part_list[b][a])
        copy.append(c)
    if len(copy) == 1:
        return copy.pop()
    elif not part_list:
        return []
    part_list_copy = make_tree_birliler(copy)
    part_list_copy1 = make_tree_coklular(part_list_copy)
    return part_list_copy1


def datum(t):
    return t[0]


def children(t):
    return t[1:]


def is_leaf(t):
    return len(children(t)) == 0


def is_empty(t):
    return t == []


def print_tree(tree, level=0):
    if not tree:
        return
    print(" " * 4 * level + str(datum(tree)))
    for child in children(tree):
        print_tree(child, level + 1)


def sum_tree(tree):
    if len(tree) == 3 and type(tree[2]) == float:
        # print("isleafte ", tree)
        return tree[0]*tree[2]

    elif type(tree[0]) == str:
        # print ("elifte ", tree)
        return sum_forest(tree[1:])


    else:
        # print("elsete ", tree)
        return tree[0]*sum_forest(tree[2:])


def sum_forest(forest):
    if len(forest) == 0:
        # print ("return 0")
        return 0
    else:
        # print(forest[0])
        # print(forest[2:])
        return (sum_tree(forest[0]) + sum_forest(forest[1:]))


def calculate_price(part_list):
    copy = []
    for b in range(len(part_list)):
        c = []
        for a in range(len(part_list[b])):
            c.append(part_list[b][a])
        copy.append(c)
    if len(copy) == 1:
        return copy[0][1]
    elif not part_list:
        return 0
    tree = make_tree(copy)
    price = sum_tree(tree)
    return price


def required_parts_tree(tree, count, liste):
    if len(tree) == 3 and type(tree[2]) == float:
        # print("isleafte ", tree)
        liste.append((count, tree[1]))
        return 1

    elif type(tree[0]) == str:
        # print ("elifte ", tree)
        return required_parts_helper_forest(tree[1:], 1, liste)


    else:
        # print("elsete ", tree)
        return required_parts_helper_forest(tree[2:], count, liste)


def required_parts_helper_forest(forest, count, liste):
    if len(forest) == 0:
        # print ("return 0")
        return 1
    else:
        return (required_parts_tree(forest[0], count * forest[0][0], liste) + required_parts_helper_forest(forest[1:],
                                                                                                           count,
                                                                                                           liste))


def required_parts(part_list):
    copy = []
    for b in range(len(part_list)):
        c = []
        for a in range(len(part_list[b])):
            c.append(part_list[b][a])
        copy.append(c)
    liste = []
    if len(copy) == 1:
        return [(1, copy[0][0])]
    elif not part_list:
        return []
    tree = make_tree(copy)
    required_parts_tree(tree, 1, liste)
    return liste


def find_count(tree, part):
    for p in tree:
        if p[1] == part:
            return p[0]
    return 0


def stock_check(part_list, stock_list):
    copy = []
    for b in range(len(part_list)):
        c = []
        for a in range(len(part_list[b])):
            c.append(part_list[b][a])
        copy.append(c)

    if not part_list:
        return []
    tree = make_tree(copy)
    liste = required_parts(part_list)

    listt = []
    if stock_list==[]:
        yeni_liste= []
        for lst in liste:
            yeni_liste.append((lst[1], lst[0]))
        return yeni_liste

    elif len(liste) == 1:
        if liste[0][0] > stock_list[0][0]:
            return [(liste[0][1], liste[0][0] - stock_list[0][0])]
        else:
            return []
    else:
        for part in liste:
            count_stock = find_count(stock_list, part[1])
            if count_stock < part[0]:
                fark = part[0] - count_stock
                listt.append((part[1], fark))
        return listt