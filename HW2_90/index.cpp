#include "./index.h"

Node::Node(std::size_t _degree, bool _is_leaf = IS_LEAF, std::size_t _size = 0)
{
    this->degree = _degree;
    this->is_leaf = _is_leaf;
    this->size = _size;

    this->key = new int[degree - 1];
    this->value = new int[degree - 1];
    this->children = new Node *[degree];
    this->parent = nullptr;
    for (int i = 0; i < degree - 1; i++)
    {
        this->key[i] = 0;
        this->value[i] = 0;
        this->children[i] = nullptr;
    }
    this->children[degree - 1] = nullptr;
};

Node::~Node()
{
    delete[] this->key;
    delete[] this->value;
    delete[] this->children;
};

int *Node::insert_key_non_full(int _key, int _value)
{
    int i = this->size;
    while (i > 0 && this->key[i - 1] > _key)
    {
        this->key[i] = this->key[i - 1];
        this->value[i] = this->value[i - 1];
        i--;
    }
    this->key[i] = _key;
    this->value[i] = _value;
    return this->key;
};
/**
 * @brief Construct a new Index object
 * @param num_rows the number of rows in the table
 * @param key the vector of keys
 * @param value the vector of values
 */
Index::Index(int num_rows, std::vector<int> &key, std::vector<int> &value)
{
    this->root = nullptr;
    this->degree = 128;
    this->num_rows = num_rows;
    for (int i = 0; i < num_rows; i++)
    {
        insert(key[i], value[i]);
    }
    // bfs();
};
// Query by key
void Index::key_query(std::vector<int> &query_keys)
{
    std::ofstream file;
    file.open("key_query_out.txt");
    for (int i = 0; i < query_keys.size(); i++)
    {
        int result_value = -1;
        Node *cursor = bplus_tree_range_search(this->root, query_keys[i]);
        // search for the key in the leaf node
        for (int j = 0; j < cursor->size; j++)
        {
            if (cursor->key[j] == query_keys[i])
            {
                result_value = cursor->value[j];
                break;
            }
        }
        // file << "Key: " << cursor->key[j] << ", Value: " << this->values[cursor->value[j]] << std::endl;
        file << result_value << std::endl;
    }
};
// Query by range of key
void Index::range_query(std::vector<std::pair<int, int>> &query_pairs)
{
    std::ofstream file;
    file.open("range_query_out.txt");

    for (int i = 0; i < query_pairs.size(); i++)
    {
        int result_value = INT32_MAX;
        // Do a range query in the leaf node
        // Get the leftmost leaf node that might contains the left bound
        Node *cursor = bplus_tree_range_search(this->root, query_pairs[i].first);
        // Search in the leftmost leaf node first.
        while(cursor != nullptr && query_pairs[i].second >= cursor->key[0])
        {
            for(int j = 0; j < cursor->size; j++)
            {
                if(cursor->key[j] >= query_pairs[i].first && cursor->key[j] <= query_pairs[i].second)
                {
                    result_value = std::min(cursor->value[j], result_value);
                }
            }
            // Keep Searching until the minimum key in this leaf node is greater than right bound
            cursor = cursor->children[cursor->size];
        }
        // file << "Key: " << cursor->key[j] << ", Value: " << this->values[cursor->value[j]] << std::endl;
        file << result_value << std::endl;
    }
};
// Free memory
void Index::clear_index()
{
    dfs(this->root);
};

void Index::dfs(Node *root)
{
    // Delete Nodes using DFS
    if (root == nullptr)
    {
        return;
    }
    if (root->is_leaf == IS_LEAF)
    {
        delete root;
        return;
    }
    for (int i = 0; i < root->size + 1; i++)
    {
        dfs(root->children[i]);
    }
    delete root;
}

void Index::bfs()
{
    Node *cursor = this->root;
    std::queue<Node *> q;
    q.push(cursor);
    while (!q.empty())
    {
        cursor = q.front();
        q.pop();
        std::cout << "Node: " << cursor << std::endl;
        std::cout << "Size: " << cursor->size << std::endl;
        std::cout << "Key: ";
        for (int i = 0; i < cursor->size; i++)
        {
            std::cout << cursor->key[i] << " ";
        }
        std::cout << std::endl;
        std::cout << "Value: ";
        for (int i = 0; i < cursor->size; i++)
        {
            std::cout << cursor->value[i] << " ";
        }
        std::cout << std::endl;
        std::cout << "Children: ";
        for (int i = 0; i < cursor->size + 1; i++)
        {
            std::cout << cursor->children[i] << " ";
            if (cursor->is_leaf == NOT_LEAF)
            {
                if (cursor->children[i] != nullptr)
                {
                    q.push(cursor->children[i]);
                }
            }
        }
        std::cout << std::endl;
        std::cout << "Parent: " << cursor->parent << std::endl;
        std::cout << std::endl;
    }
};

// I think this is OK
bool Index::insert(int _key, int _value)
{
    // if root is null, create a new root
    if (this->root == nullptr)
    {
        this->root = new Node(this->degree);
        this->root->key[0] = _key;
        this->root->value[0] = _value;
        this->root->size = 1;
        return true;
    }
    // if the tree has at least one node
    Node *cursor = this->root;
    cursor = bplus_tree_range_search(cursor, _key); // move to the target leaf node

    // If the leaf node will not be overflowed(i.e. cursor->size + 1 < this->degree)
    if (cursor->size < this->degree - 1)
    {
        // Insert the key-value pair into the leaf node
        cursor->key = cursor->insert_key_non_full(_key, _value);
        cursor->size++;
        // Edit the child pointers
        cursor->children[cursor->size] = cursor->children[cursor->size - 1];
        cursor->children[cursor->size - 1] = nullptr;
        return true;
    }
    // If the leaf node will be overflowed(i.e. cursor->size == this->degree - 1)
    // Create a new node (split the leaf node)
    Node *new_node = new Node(this->degree);
    new_node->parent = cursor->parent;

    // Create temporary key-value pairs
    int *temp_key = new int[this->degree];
    int *temp_value = new int[this->degree];
    for (int i = 0; i < cursor->size; i++)
    {
        temp_key[i] = cursor->key[i];
        temp_value[i] = cursor->value[i];
    }
    int idx = cursor->size;
    while (idx > 0 && temp_key[idx - 1] > _key)
    {
        temp_key[idx] = temp_key[idx - 1];
        temp_value[idx] = temp_value[idx - 1];
        idx--;
    }
    temp_key[idx] = _key;
    temp_value[idx] = _value;

    // Split the leaf node
    cursor->size = this->degree / 2;
    new_node->size = this->degree - cursor->size;
    for (int i = 0; i < cursor->size; i++)
    {
        cursor->key[i] = temp_key[i];
        cursor->value[i] = temp_value[i];
    }
    for (int i = 0; i < new_node->size; i++)
    {
        new_node->key[i] = temp_key[i + cursor->size];
        new_node->value[i] = temp_value[i + cursor->size];
    }
    // The new node is on the right of cursor, so these are the only pointers we need to change
    cursor->children[cursor->size] = new_node;
    new_node->children[new_node->size] = cursor->children[this->degree - 1];
    new_node->children[0] = cursor;
    cursor->children[this->degree - 1] = nullptr;

    delete[] temp_key;
    delete[] temp_value;

    // if there is no parent node initially(i.e. the root is the leaf node)
    if (cursor->parent == nullptr)
    {
        Node *new_root = new Node(this->degree, NOT_LEAF);
        // Insert the middle key-value pair into the parent node
        cursor->parent = new_root;
        new_node->parent = new_root;

        new_root->key[0] = new_node->key[0];
        new_root->size = 1;

        new_root->children[0] = cursor;
        new_root->children[1] = new_node;

        this->root = new_root;
        return true;
    }
    // if there is already a parent node
    return insert_parent(cursor->parent, new_node, new_node->key[0]);
};

int *Index::insert_only_key(int *_keys, int size, int _key)
{
    int i = size;
    while (i > 0 && _keys[i - 1] > _key)
    {
        _keys[i] = _keys[i - 1];
        i--;
    }
    _keys[i] = _key;
    return _keys;
};

bool Index::insert_parent(Node *node, Node *child, int _key)
{
    Node *cursor = node;
    if (cursor->size < this->degree - 1)
    {
        // Insert the key-value pair into the parent node
        cursor = insert_child_key(cursor, child, _key);
        cursor->size++;
        return true;
    }
    // If the parent node will be overflowed(i.e. cursor->size == this->degree - 1)
    // Split the parent node
    Node *new_node = new Node(this->degree, NOT_LEAF);
    new_node->parent = cursor->parent;

    // Create temporary keys with _key replaced
    // A new key will be stored at the new root of new parent nodes.
    // So there will be $(this->degree) keys in total.
    int *temp_key = new int[cursor->size + 1];
    for (int i = 0; i < cursor->size; i++)
    {
        temp_key[i] = cursor->key[i];
    }
    temp_key = insert_only_key(temp_key, cursor->size, _key);

    // There will be $(this->degree + 1) children in total.
    Node **temp_children = new Node *[cursor->size + 2];
    for (int i = 0; i < cursor->size + 1; i++)
    {
        temp_children[i] = cursor->children[i];
    }
    temp_children = insert_child(temp_children, child, cursor->size + 1, find_index(temp_key, cursor->size + 1, _key));

    cursor->size = this->degree / 2;
    new_node->size = this->degree - cursor->size - 1; // 3->1+1, 4(#children)->2+1(#key)
    for (int i = 0; i < cursor->size; i++)
    {
        cursor->key[i] = temp_key[i];
        cursor->children[i] = temp_children[i];
    }
    cursor->children[cursor->size] = temp_children[cursor->size];
    for (int i = 0; i < new_node->size; i++)
    {
        new_node->key[i] = temp_key[i + cursor->size + 1];
        new_node->children[i] = temp_children[i + cursor->size + 1];
        new_node->children[i]->parent = new_node;
    }
    new_node->children[new_node->size] = temp_children[this->degree];
    new_node->children[new_node->size]->parent = new_node;

    int middle_key = temp_key[this->degree / 2];

    delete[] temp_key;
    delete[] temp_children;

    if (cursor->parent == nullptr)
    {
        Node *new_parent = new Node(this->degree, NOT_LEAF);
        cursor->parent = new_parent;
        new_node->parent = new_parent;

        new_parent->key[0] = middle_key;
        new_parent->size++;

        new_parent->children[0] = cursor;
        new_parent->children[1] = new_node;

        this->root = new_parent;
        return true;
    }
    return insert_parent(cursor->parent, new_node, middle_key);
};
Node **Index::insert_child(Node **child_arr, Node *child, int size, int position)
{
    for (int i = size; i > position; i--)
    {
        child_arr[i] = child_arr[i - 1];
    }
    child_arr[position] = child;
    return child_arr;
};
int Index::find_index(int *arr, int size, int _data)
{
    for (int i = 0; i < size; i++)
    {
        if (arr[i] > _data)
        {
            return i;
        }
        if (i == size - 1)
        {
            return size;
        }
    }
    return -1;
};
Node *Index::insert_child_key(Node *node, Node *child, int _key)
{
    int key_position = 0;
    int child_position = 0;
    for (int i = 0; i < node->size; i++)
    {
        if (node->key[i] > _key)
        {
            key_position = i;
            child_position = i + 1;
            break;
        }
        if (i == node->size - 1)
        {
            key_position = node->size;
            child_position = node->size + 1;
            break;
        }
    }
    for (int i = node->size; i > key_position; i--)
    {
        node->key[i] = node->key[i - 1];
    }
    for (int i = node->size + 1; i > child_position; i--)
    {
        node->children[i] = node->children[i - 1];
    }
    node->key[key_position] = _key;
    node->children[child_position] = child;
    return node;
};

Node *Index::bplus_tree_range_search(Node *node, int _key)
{
    // if node is null return null
    if (node == nullptr)
    {
        return nullptr;
    }
    // find the leaf node that might contains the key
    Node *cursor = node;
    while (!cursor->is_leaf)
    {
        // find the first key in this node that is larger than _key
        for (int i = 0; i < cursor->size; i++)
        {
            if (_key < cursor->key[i])
            {
                cursor = cursor->children[i];
                break;
            }
            if (i == cursor->size - 1)
            {
                cursor = cursor->children[i + 1];
                break;
            }
        }
    }
    return cursor;
};