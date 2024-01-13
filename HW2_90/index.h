#ifndef BPlustTree_H
#define BPlustTree_H

#include <iostream>
#include <vector>
#include <queue>
#include <fstream>

#define IS_LEAF true
#define NOT_LEAF false

struct Node
{
    // maximum number of children
    std::size_t degree;
    // whether the node is a leaf node
    bool is_leaf;
    // current number of item
    std::size_t size;
    // Key-Value pairs in the Node
    // std::pair<int, int> *key_value;
    int *key;
    int *value;
    // Pointers to children, an internal node could have at most degree children.
    // A leaf node could have at most degree item.
    Node **children;
    // Pointers to parent
    Node *parent;

public:
    /**
     * @brief Construct a new Node object
     * @param _degree the maximum number of children
     * @param _is_leaf whether the node is a leaf node
     * @param _size the current number of item
     */
    Node(std::size_t _degree, bool _is_leaf, std::size_t _size);
    ~Node();
    /**
     * @brief insert the key into this non-full node
     * @param _key the key to insert
     * @param _value the value to insert
     * @return the pointer to the key-value pair
     */
    int *insert_key_non_full(int _key, int _value);
};

class Index
{
private:
    Node *root;
    std::size_t degree;
    std::size_t num_rows;

public:
    // Constructor
    /**
     * @brief Construct a new Index object
     * @param num_rows the number of rows in the table
     * @param key the vector of keys
     * @param value the vector of values
     */
    Index(int num_rows, std::vector<int> &key, std::vector<int> &value);
    // Query by key
    void key_query(std::vector<int> &query_keys);
    // Query by range of key
    void range_query(std::vector<std::pair<int, int>> &query_pairs);
    // Free memory
    void clear_index();
    int* insert_only_key(int* _keys, int size, int _key);
    void bfs();
    void dfs(Node *root);
    /**
     * @brief put the key-value pair into the right position of cursor
     * @param node the node to insert into
     * @param child the child node to be append
     * @param _key the key to insert
     * @return the pointer to the key-value pair
     */
    Node *insert_child_key(Node *node, Node *child, int _key);
    Node **insert_child(Node **node, Node *child, int size, int position);
    int find_index(int *arr, int size, int _data);
    /**
     * @brief insert the key-value pair into the parent node
     * @param node _key will be inseted to node (the parent node)
     * @param child the child node
     * @param _key the key to insert
     * @return true if the key-value pair is successfully inserted, false otherwise
     */
    bool insert_parent(Node *node, Node *child, int _key);
    /**
     *  @brief insert a key-value pair into the index
     *  @param key the key to insert
     *  @param value the value to insert
     *  @return true if the key-value pair is successfully inserted, false otherwise
     */
    bool insert(int _key, int _value);
    /**
     *  @brief search a key in Index
     *  @param node the node to search from
     *  @param key the key to search
     *  @return the pointer to the leaf node that contains the key
     */
    Node *bplus_tree_range_search(Node *node, int _key);
};

#endif