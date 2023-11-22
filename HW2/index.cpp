#include "./index.h"

Index::Index(int num_rows, std::vector<int> &key, std::vector<int> &value)
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
}
void Index::key_query(std::vector<int> &query_keys)
{
}
void Index::range_query(std::vector<std::pair<int, int>> &query_pairs)
{
}
void Index::clear_index()
{
}
// insert a key-value pair into the index
// @param key: the key to insert
// @param value: the value to insert
// @return: true if the key-value pair is successfully inserted, false otherwise
bool Index::insert(int key, int _position)
{
    // if root is null, create a new root
    if (this->root == nullptr)
    {
        this->root = new Node(this->degree, true);
        this->root->keys[0] = key;
        this->root->values[0] = _position;
        this->root->num_keys = 1;
        return true;
    }
    // if root is full, split it
    if (this->root->num_keys == 2 * this->degree - 1)
    {
        Node *new_root = new Node(this->degree, false);
        new_root->children[0] = this->root;
        new_root->split_child(0, this->root);
        int i = 0;
        if (new_root->keys[0] < key)
        {
            i++;
        }
        new_root->children[i]->insert_non_full(key, _position);
        this->root = new_root;
    }
    else
    {
        this->root->insert_non_full(key, _position);
    }
    return true;
}
// search a key in Index
// @param key: the key to search
// @return: the pointer to the leaf node that contains the key
Node *Index::bplus_tree_search(int key)
{
}