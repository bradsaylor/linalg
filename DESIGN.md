# Adding and removing names from registry
ADDING BINDING
1. Hash the name string
2. find index by % TABLE_SIZE (prime)
3. check if name exists already
  3a. If yes:
    - check if passed obj* == found obj* at that LL element
      - If yes do nothing return success
      - If no proceed
    - decrement num_refs for current element->object
      - if ref_count == 0 (after dec) trigger destruction of current element->object 
    - replace LL element->object with passed object
    - increment element->object->ref_count
  3b. If no:
    - Allocate name string for new LL element
    - add LL element to the LL at that index
    - increment new_element->object->ref_count

REMOVING BINDING

1. Hash the name string
2. Find index via hash % TABLE_SIZE
3. Search the linked list at that index for the name
  - If name not found → return “not found” / error (no side effects)
4. Capture the wrapper pointer from the node before modifying anything
5. Decrement the wrapper’s ref_count
6. If ref_count == 0 after decrement call destroy_obj(wrapper)
7. Remove the linked-list node
  - unlink it from the list
  - free the node’s name string
  - free the node struct itself

Need
- hash function (will use K&R string hash)
- table initialization
- methods for add, replace, remove LL element
- find_node() to return node and prev_node for name string input
- name_copy() for copying and allocating new node->name's
- destroy_registry() for program exit
- list_bindings() (diagnostic)

Conventions
- add/remove_binding returns:
  - 0 on success
  - 1 on name not found (remove only)
  - 2 on failed allocation (add only)
  - 3 on invalid input
    - add: we check name != NULL, name[0]!= "\0", obj* != NULL
    - remove: we check name != NULL, name[0] != "\0"
  - 4 on registry internal error
- If name already exists on LL we overwrite it

# Adding and removing objects from object index
ADDING OBJECT



