#ifndef BPlustTree_H
#define BPlustTree_H

#include <iostream>
#include <vector>

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
    std::pair<int, int> *key_value;
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
    Node(std::size_t _degree, bool _is_leaf = IS_LEAF, std::size_t _size = 0)
    {
        this->degree = _degree;
        this->is_leaf = _is_leaf;
        this->size = _size;

        this->key_value = new std::pair<int, int>[degree - 1];
        this->children = new Node *[degree];
        this->parent = nullptr;
        for (int i = 0; i < degree - 1; i++)
        {
            this->key_value[i] = std::make_pair(0, 0);
            this->children[i] = nullptr;
        }
        this->children[degree - 1] = nullptr;
    }
    /**
     * @brief insert the key into this non-full node
     * @param _key the key to insert
     * @param _value the value to insert
     * @return the pointer to the key-value pair
     */
    std::pair<int, int> *insert_key_non_full(int _key, int _value)
    {
        this->key_value[this->size] = std::make_pair(_key, _value);
        return this->key_value;
    }
};

class Index
{
private:
    Node *root;
    std::size_t degree;
    std::size_t num_rows;
    int *values;

public:
    // Constructor
    /**
     * @brief Construct a new Index object
     * @param num_rows the number of rows in the table
     * @param key the vector of keys
     * @param value the vector of values
     */
    Index(int num_rows, std::vector<int> &key, std::vector<int> &value)
    {
        this->root = nullptr;
        this->degree = 8;
        this->num_rows = num_rows;

        this->values = new int[num_rows];
        for (int i = 0; i < num_rows; i++)
        {
            this->values[i] = value[i];
            insert(key[i], i);
        }
    };
    // Query by key
    void key_query(std::vector<int> &query_keys){};
    // Query by range of key
    void range_query(std::vector<std::pair<int, int>> &query_pairs){};
    // Free memory
    void clear_index(){};
    /**
     *  @brief insert a key-value pair into the index
     *  @param key the key to insert
     *  @param value the value to insert
     *  @return true if the key-value pair is successfully inserted, false otherwise
     */
    bool insert(int _key, int _value)
    {
        // if root is null, create a new root
        if (this->root == nullptr)
        {
            this->root = new Node(this->degree);
            this->root->key_value[0] = std::make_pair(_key, _value);
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
            cursor->key_value = cursor->insert_key_non_full(_key, _value);
            cursor->size++;
            // Edit the child pointers
            cursor->children[cursor->size] = cursor->children[cursor->size - 1];
            cursor->children[cursor->size - 1] = nullptr;
            return true;
        }

        // If the leaf node will be overflowed(i.e. cursor->size == this->degree - 1)
        // Find the insert position
        int insert_position = 0;
        while (insert_position < cursor->size && cursor->key_value[insert_position++].first < _key)
            ;

        // Check if its left sibling has space
        if (cursor->children[0] != nullptr && cursor->children[0]->size < this->degree - 1)
        {
            // New Key will not be the smallest in cursor because the one could only enter the leaf node if the Key is greater than the Key in the smallest in cursor.
            // So this case only has one condition, compared to the right sibling case.

            // Move the first key-value pair of cursor to the last position of the left sibling
            cursor->children[0]->key_value[cursor->children[0]->size] = cursor->key_value[0];
            cursor->children[0]->size++;
            // Insert the new key-value pair into cursor
            for (int i = 0; i < insert_position - 1; i++)
            {
                cursor->key_value[i] = cursor->key_value[i + 1];
            }
            cursor->key_value[insert_position - 1] = std::make_pair(_key, _value);

            // Copy the smallest key in cursor to the parent node
            for (int i = 0; i < cursor->parent->size - 1; i++)
            {
                if (cursor->parent->key_value[i + 1].first > cursor->key_value[0].first)
                {
                    cursor->parent->key_value[i] = cursor->key_value[0];
                    return true;
                }
            }
            cursor->parent->key_value[cursor->parent->size - 1] = cursor->key_value[0];
            return true;
        }
        // Check if its right sibling has space
        if (cursor->children[this->degree - 1] != nullptr && cursor->children[this->degree - 1]->size < this->degree - 1)
        {
            if (insert_position == cursor->size)
            {
                // The new key-value pair has the largest key in cursor -> Insert the new key-value pair into the first position of the right sibling
                for (int i = cursor->children[this->degree - 1]->size; i > 0; i--)
                {
                    cursor->children[this->degree - 1]->key_value[i] = cursor->children[this->degree - 1]->key_value[i - 1];
                }
                cursor->children[this->degree - 1]->key_value[0] = std::make_pair(_key, _value);
                cursor->children[this->degree - 1]->size++;
            }
            else
            {
                // Move the last key-value pair of cursor to the first position of the right sibling
                for (int i = cursor->children[this->degree - 1]->size; i > 0; i--)
                {
                    cursor->children[this->degree - 1]->key_value[i] = cursor->children[this->degree - 1]->key_value[i - 1];
                }
                cursor->children[this->degree - 1]->key_value[0] = cursor->key_value[cursor->size - 1];
                cursor->children[this->degree - 1]->size++;
                // Insert the new key-value pair into cursor
                for (int i = cursor->size - 1; i > insert_position; i--)
                {
                    cursor->key_value[i] = cursor->key_value[i - 1];
                }
                cursor->key_value[insert_position] = std::make_pair(_key, _value);
            }
            // Copy the smallest key in right sibling to the parent node
            for (int i = 0; i < cursor->parent->size - 1; i++)
            {
                if (cursor->parent->key_value[i + 1].first > cursor->children[this->degree - 1]->key_value[0].first)
                {
                    cursor->parent->key_value[i] = cursor->children[this->degree - 1]->key_value[0];
                    return true;
                }
            }
            cursor->parent->key_value[cursor->parent->size - 1] = cursor->children[this->degree - 1]->key_value[0];
            return true;
        }

        // Create a new node
        Node *new_node = new Node(this->degree);
        new_node->parent = cursor->parent;

        // Create temporary key-value pairs
        std::pair<int, int> *temp_key_value = new std::pair<int, int>[this->degree];
        int i = 0;
        while (i < cursor->size && _key > cursor->key_value[i].first)
        {
            temp_key_value[i] = cursor->key_value[i];
            i++;
        }
        temp_key_value[i] = std::make_pair(_key, _value);
        while (i < cursor->size)
        {
            temp_key_value[i + 1] = cursor->key_value[i];
            i++;
        }

        // Split the leaf node
        cursor->size = this->degree / 2;
        new_node->size = this->degree - cursor->size;
        i = 0;
        while (i < cursor->size)
        {
            cursor->key_value[i] = temp_key_value[i];
            i++;
        }
        while (i < this->degree)
        {
            new_node->key_value[i - cursor->size] = temp_key_value[i];
            i++;
        }
        // The new node is on the right of the cursor, so these are the only pointers we need to change
        cursor->children[cursor->size] = new_node;
        new_node->children[new_node->size] = cursor->children[this->degree - 1];
        cursor->children[this->degree - 1] = nullptr;

        delete[] temp_key_value;

        // if there is no parent node initially(i.e. the root is the leaf node)
        if (cursor->parent == nullptr)
        {
            Node *new_root = new Node(this->degree, NOT_LEAF);
            // Insert the middle key-value pair into the parent node
            new_root->key_value[0] = new_node->key_value[0];
            new_root->children[0] = cursor;
            new_root->children[1] = new_node;
            new_root->size = 1;
            cursor->parent = new_root;
            new_node->parent = new_root;
            this->root = new_root;
            return true;
        }
        // if there is already a parent node
    };
    /**
     *  @brief search a key in Index
     *  @param node the node to search from
     *  @param key the key to search
     *  @return the pointer to the leaf node that contains the key
     */
    Node *bplus_tree_range_search(Node *node, int _key)
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
                if (_key < cursor->key_value[i].first)
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
    /**
     * @brief search a range of keys in Index
     * @param start the start of the range (inclusive)
     * @param end the end of the range (inclusive)
     * @return the pointer to the leaf node that contains the start key
     */
    Node *range_search(int start, int end){};
};

#endif