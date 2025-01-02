#I added comments for whitebox testing


import re


#this function parses an expression into its predicate name and arguments.
def parse_predicate(expr):
    predicate = expr[:expr.find("(")].strip()
    arguments = expr[expr.find("(")+1:expr.find(")")].split(",")
    arguments = [a.strip() for a in arguments]
    return predicate, arguments

#this function finalizes all variable mappings in the substitution map to the final values.
#I used dict, to make sure all variables in the map are fully mapped to their final version of constants or values
def apply_substitutions(variable_map):
    changed = True
    while changed:
        changed = False
        for var, val in list(variable_map.items()):
            if val in variable_map:
                variable_map[var] = variable_map[val]
                changed = True
    return variable_map


#this function is to prove the query with backward chaining algorithm with the given knowledge base. It traverses knowledge base rules recursively to prove the query.
#currently, it doesn't produce the output sequences exactly.(substitutions that are exactly like shown in the homework), but it fits to the "slightly different than sample input/output" definition in the pdf
#it detects the cyclic dependencies for infinite recursions, and if the query can't be reached otherwise, then it stops.
def backward_chaining(kb, query):
    facts, knowledge_base = kb_parser(kb)

    def prove(goal, facts, knowledge_base, trace, visited):

        goal_predicate, goal_arguments = parse_predicate(goal)

        for fact in facts:
            fact_predicate, fact_arguments = parse_predicate(fact)
            if fact_predicate == goal_predicate:
                variable_map = unify(goal_arguments, fact_arguments)
                if variable_map is not None:
                    if goal in visited:
                        visited.remove(goal)
                    return True
        if goal in visited:
            return False
        visited.add(goal)

        for implied, implies in knowledge_base:
            head_match = [h for h in implied if h.startswith(goal_predicate + "(")]
            for head in head_match:
                head_predicate, head_arguments = parse_predicate(head)
                variable_map = unify(head_arguments, goal_arguments)
                if variable_map is None:
                    continue
                instantiated_implies = [substitute_variables(cond, variable_map) for cond in implies]
                instantiated_implied = [substitute_variables(h, variable_map) for h in implied]

                combined_sub_trace = []
                all_subgoals_proven = True
                for subgoal in instantiated_implies:
                    subgoal_trace = []
                    if not prove(subgoal, facts, knowledge_base, subgoal_trace, visited):
                        all_subgoals_proven = False
                        break
                    else:
                        combined_sub_trace.extend(subgoal_trace)

                if all_subgoals_proven:
                    
                    for h in instantiated_implied:
                        if h not in facts:
                            facts.append(h)
                    trace.extend(combined_sub_trace)
                    trace.append(f"{', '.join(instantiated_implied)} <- {', '.join(instantiated_implies)}")
                    visited.remove(goal)
                    return True

        visited.remove(goal)
        return False

    trace = []
    visited = set()
    if prove(query, facts, knowledge_base, trace, visited):
        print(trace)
    else:
        print("Query cannot be proven.")


#this function is to prove the query with forward chaining algorithm with the given knowledge base.
#it iteratively applies Horn clause rules to infer new facts until the query is reached, or if the query isnt reached and cant find anymore new facts, then it stops.
def forward_chaining(kb, q):
    facts, knowledge_base = kb_parser(kb)
    inferred = []

    while True:
        new_inference = False
        for implied, implies in knowledge_base:
            possible_matches = generate_substitutions(implies, facts)
            for substitution in possible_matches:
                instantiated_implies = [substitute_variables(cond, substitution) for cond in implies]
                instantiated_implied = [substitute_variables(head, substitution) for head in implied]

                if all(cond in facts for cond in instantiated_implies):
                    for n in instantiated_implied:
                        if n not in facts:
                            facts.append(n)
                            inferred.append(n)
                            new_inference = True
                            if n == q:
                                print(inferred)
                                return
        
        if not new_inference:
            break

    print(inferred) if q in facts else print("Query cannot be proven.")
    return

#this function parses the given knowledge base as input to forward/backward chaining, into facts and rules.
def kb_parser(kb):
    knowledge_base = []
    facts = []
    for i in kb:
        if "<-" in i:
            implied, implies = i.split("<-")
            implied = re.findall(r'[^\s,]+\(.*?\)', implied)
            implies = re.findall(r'[^\s,]+\(.*?\)', implies)
            knowledge_base.append((implied, implies))
        else:
            fact_match = re.findall(r'[^\s,]+\(.*?\)', i)
            if fact_match:
                facts.append(i.strip())
    return facts, knowledge_base


#this function generates all possible substitutions for variables in a rule's body part, based on the facts that are already given, or inferred newly.
#I used dict "current_substitution", to map variables to constants based on fact matches.
def generate_substitutions(implies, facts):
    substitutions = []
    for condition in implies:
        predicate = condition[:condition.find("(")]
        variables = condition[condition.find("(") + 1:condition.find(")")].split(",")
        for fact in facts:
            if fact.startswith(predicate):
                fact_constants = fact[fact.find("(") + 1:fact.find(")")].split(",")
                if len(variables) == len(fact_constants):
                    current_substitution = {}
                    consistent = True
                    for var, const in zip(variables, fact_constants):
                        if var[0].islower():
                            if var in current_substitution and current_substitution[var] != const:
                                consistent = False
                                break
                            current_substitution[var] = const
                        else:
                            if var != const:
                                consistent = False
                                break
                    if consistent:
                        substitutions.append(current_substitution)
    return substitutions

#this function substitutes variables in a predicate expression based on the provided substitution map, which is provided by unify/generate_substitiutions functions.
def substitute_variables(expression, variable_map):
    if "(" in expression and ")" in expression:
        predicate = expression[:expression.find("(")]
        arguments = expression[expression.find("(")+1:expression.find(")")].split(",")
        substituted_arguments = [variable_map.get(arg.strip(), arg.strip()) for arg in arguments]
        return f"{predicate}({','.join(substituted_arguments)})"
    return expression

#this function is to unify two lists of arguments.
#I used dict, to map variables to constants or other variables.
def unify(args1, args2):
    if len(args1) != len(args2):
        return None
    variable_map = {}
    for a1, a2 in zip(args1, args2):
        a1, a2 = a1.strip(), a2.strip()
        if a1[0].islower() and a2[0].islower():
            if a1 in variable_map and a2 in variable_map:
                if variable_map[a1] != variable_map[a2]:
                    return None
            elif a1 in variable_map:
                variable_map[a2] = variable_map[a1]
            elif a2 in variable_map:
                variable_map[a1] = variable_map[a2]
            else:
                variable_map[a2] = a1
        elif a1[0].islower() and not a2[0].islower():
            if a1 in variable_map and variable_map[a1] != a2:
                return None
            variable_map[a1] = a2
        elif not a1[0].islower() and a2[0].islower():
            if a2 in variable_map and variable_map[a2] != a1:
                return None
            variable_map[a2] = a1
        else:
            if a1 != a2:
                return None
    return variable_map


#this function is to merge two substitution maps.
#I used dict, to combine two dictionaries, to make sure there is no conflicting mapping.
#(I wrote this function to achieve the fully matching output, but it didn't help, but to make sure about the confliction, I didnt delete it.)
def merge_maps(map1, map2):
    if map2 is None:
        return None
    for k, v in map2.items():
        if k in map1 and map1[k] != v:
            return None
        map1[k] = v
    apply_substitutions(map1)
    return map1


#I have tried the given inputs/outputs here.
def main():

    
    backward_chaining(
        ["Q(x) <- P(x)", "P(x) <- L(x), M(x)", "M(x) <- B(x), L(x)",
         "L(x) <- A(x), P(x)", "L(x) <- A(x), B(x)", "A(John)", "B(John)"],
        "Q(John)"
    )

    backward_chaining(
        ["Criminal(x) <- American(x), Weapon(y), Sells(x,y,z), Hostile(z)",
         "Weapon(x) <- Missile(x)",
         "Missile(M)",
         "Owns(Nono,M)",
         "Sells(West,x,Nono) <- Owns(Nono,x), Missile(x)",
         "Hostile(x) <- Enemy(x,America)",
         "American(West)",
         "Enemy(Nono,America)"],
        "Criminal(West)"
    )

    forward_chaining(
        ["Q(x) <- P(x)", "P(x) <- L(x), M(x)", "M(x) <- B(x), L(x)",
         "L(x) <- A(x), P(x)", "L(x) <- A(x), B(x)", "A(John)", "B(John)"],
        "Q(John)"
    )
    
    forward_chaining(
        ["Partner(x,y) <- Loves(x,y), Loves(y,x)", "Happy(y) <- Gift(x,z), Partner(x,y)",
         "Loves(Feyza,Can)", "Loves(Can,Feyza)", "Gift(Can,z)"],
        "Happy(Feyza)"
    )
    forward_chaining(
["Q(x) <- P(x)", "P(x) <- L(x), M(x)", "M(x) <- B(x), L(x)",
"L(x) <- A(x), P(x)", "L(x) <- A(x), B(x)", "A(John)"],
"Q(John)"
)


    backward_chaining(
["Q(x) <- P(x)", "P(x) <- L(x), M(x)", "M(x) <- B(x), L(x)",
"L(x) <- A(x), P(x)", "L(x) <- A(x), B(x)", "A(John)"],
"Q(John)"
)
    

if __name__ == "__main__":
    main()



