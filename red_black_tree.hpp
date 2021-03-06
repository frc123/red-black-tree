#ifndef _RED_BLACK_TREE_HPP
#define _RED_BLACK_TREE_HPP

#include <utility>
#include <iterator>
#include <stdexcept>

// ---------- declaration ----------

template <class Key, class T>
class RedBlackTree
{
public:
    typedef std::pair<const Key, T> ValueType;

#ifdef RBT_TESTING
protected:
#else
private:
#endif
    struct Node
    {
        Node* parent;
        Node* left;
        Node* right;
        ValueType value;
        enum { BLACK, RED } color;
        Node();
        Node(const ValueType& value);
    };

public:
    class Iterator : public std::iterator<std::bidirectional_iterator_tag, ValueType>
    {
    public:
        Iterator& operator++();
        Iterator& operator--();
        ValueType& operator*() const;
        ValueType* operator->() const;
        bool operator==(const Iterator& other) const;
        bool operator!=(const Iterator& other) const;
        Iterator();
    private:
        friend class RedBlackTree<Key, T>;
        Iterator(Node* node, RedBlackTree<Key, T>* tree);
        Node* node_;
        RedBlackTree<Key, T>* tree_;
    };

    RedBlackTree();
    ~RedBlackTree();
    std::pair<Iterator, bool> Insert(const ValueType& value);
    void Delete(Iterator pos);
    void Clear();
    T& At(const Key& key);
    Iterator Find(const Key& key);
    Iterator Begin();
    Iterator End();
    T& operator[](Key&& key);

#ifdef RBT_TESTING
protected:
#else
private:
#endif
    void LeftRotate(Node* subtree_root_node);
    void RightRotate(Node* subtree_root_node);
    void InsertFixup(Node* node);
    void Transplant(Node* old_node, Node* new_node);
    Node* TreeMinimum(Node* sub_tree_root);
    Node* TreeMaximum(Node* sub_tree_root);
    Node* TreeSuccessor(Node* node);
    Node* TreePredecessor(Node* node);
    void DeleteFixup(Node* node);
    
    Node* root_;
    Node* nil_;

};

// ---------- definition ----------

template <class Key, class T>
RedBlackTree<Key, T>::Node::Node() {}

template <class Key, class T>
RedBlackTree<Key, T>::Node::Node(const ValueType& value) : value(value) {}

template <class Key, class T>
RedBlackTree<Key, T>::Iterator::Iterator() : node_(nullptr), tree_(nullptr) {}

template <class Key, class T>
RedBlackTree<Key, T>::Iterator::Iterator(Node* node, RedBlackTree<Key, T>* tree) : node_(node), tree_(tree) {}

template <class Key, class T>
bool RedBlackTree<Key, T>::Iterator::operator==(const Iterator& other) const
{
    return node_ == other.node_;
}

template <class Key, class T>
bool RedBlackTree<Key, T>::Iterator::operator!=(const Iterator& other) const
{
    return node_ != other.node_;
}

template <class Key, class T>
typename RedBlackTree<Key, T>::ValueType& RedBlackTree<Key, T>::Iterator::operator*() const
{
    return node_->value;
}

template <class Key, class T>
typename RedBlackTree<Key, T>::ValueType* RedBlackTree<Key, T>::Iterator::operator->() const
{
    return &(node_->value);
}

template <class Key, class T>
typename RedBlackTree<Key, T>::Iterator& RedBlackTree<Key, T>::Iterator::operator++()
{
    node_ = tree_->TreeSuccessor(node_);
    return *this;
}

template <class Key, class T>
typename RedBlackTree<Key, T>::Iterator& RedBlackTree<Key, T>::Iterator::operator--()
{
    node_ = tree_->TreePredecessor(node_);
    return *this;
}

template <class Key, class T>
RedBlackTree<Key, T>::RedBlackTree()
{
    nil_ = new Node();
    nil_->color = Node::BLACK;
    root_ = nil_;
}

template <class Key, class T>
RedBlackTree<Key, T>::~RedBlackTree()
{
    Clear();
    delete nil_;
}

template <class Key, class T>
void RedBlackTree<Key, T>::LeftRotate(Node* subtree_root_node) 
{
    Node* new_root;
    new_root = subtree_root_node->right;
    subtree_root_node->right = new_root->left;
    if (subtree_root_node->right != nil_)
        subtree_root_node->right->parent = subtree_root_node;
    new_root->parent = subtree_root_node->parent;
    if (subtree_root_node->parent == nil_)
        root_ = new_root;
    else if (subtree_root_node->parent->left == subtree_root_node)
        subtree_root_node->parent->left = new_root;
    else
        subtree_root_node->parent->right = new_root;
    new_root->left = subtree_root_node;
    subtree_root_node->parent = new_root;
}

template <class Key, class T>
void RedBlackTree<Key, T>::RightRotate(Node* subtree_root_node) 
{
    Node* new_root;
    new_root = subtree_root_node->left;
    subtree_root_node->left = new_root->right;
    if (subtree_root_node->left != nil_)
        subtree_root_node->left->parent = subtree_root_node;
    new_root->parent = subtree_root_node->parent;
    if (subtree_root_node->parent == nil_)
        root_ = new_root;
    else if (subtree_root_node->parent->left == subtree_root_node)
        subtree_root_node->parent->left = new_root;
    else
        subtree_root_node->parent->right = new_root;
    new_root->right = subtree_root_node;
    subtree_root_node->parent = new_root;
}

template <class Key, class T>
typename RedBlackTree<Key, T>::Iterator RedBlackTree<Key, T>::Find(const Key& key)
{
    Node* now;
    now = root_;
    while (now != nil_)
    {
        if (now->value.first == key)
            break;
        else if (now->value.first < key)
            now = now->right;
        else
            now = now->left;
    }
    return Iterator(now, this);
}
 
template <class Key, class T>
T& RedBlackTree<Key, T>::At(const Key& key)
{
    Node* now;
    now = root_;
    while (now != nil_)
    {
        if (now->value.first == key)
            return now->value.second;
        else if (now->value.first < key)
            now = now->right;
        else
            now = now->left;
    }
    throw std::out_of_range("the container does not have an element with the specified key");
}

template <class Key, class T>
T& RedBlackTree<Key, T>::operator[](Key&& key)
{
    Node *node, **now_ptr, *parent;
    parent = nil_;
    now_ptr = &root_;
    while (*now_ptr != nil_)
    {
        parent = *now_ptr;
        if (key == (*now_ptr)->value.first)
            return (*now_ptr)->value.second;
        else if (key < (*now_ptr)->value.first)
            now_ptr = &((*now_ptr)->left);
        else
            now_ptr = &((*now_ptr)->right);
    }
    node = new Node(std::make_pair(key, T()));
    *now_ptr = node;
    node->parent = parent;
    node->right = node->left = nil_;
    node->color = Node::RED;
    InsertFixup(node);
    return node->value.second;
}

template <class Key, class T>
std::pair<typename RedBlackTree<Key, T>::Iterator, bool> RedBlackTree<Key, T>::Insert(const ValueType& value)
{
    Node *node, **now_ptr, *parent;
    parent = nil_;
    now_ptr = &root_;
    while (*now_ptr != nil_)
    {
        parent = *now_ptr;
        if (value.first == (*now_ptr)->value.first)
            return std::make_pair(Iterator(*now_ptr, this), false);
        else if (value.first < (*now_ptr)->value.first)
            now_ptr = &((*now_ptr)->left);
        else
            now_ptr = &((*now_ptr)->right);
    }
    node = new Node(value);
    *now_ptr = node;
    node->parent = parent;
    node->right = node->left = nil_;
    node->color = Node::RED;
    InsertFixup(node);
    return std::make_pair(Iterator(node, this), true);
}

template <class Key, class T>
void RedBlackTree<Key, T>::InsertFixup(Node* node)
{
    Node *uncle, *grandparent;
    while (node->parent->color == Node::RED)
    {
        grandparent = node->parent->parent;
        if (node->parent == grandparent->left)
        {
            uncle = grandparent->right;
            if (uncle->color == Node::RED)
            {
                uncle->color = node->parent->color = Node::BLACK;
                grandparent->color = Node::RED;
                node = grandparent;
            }
            else
            {
                if (node == node->parent->right)
                {
                    node = node->parent;
                    LeftRotate(node);
                    // notice that varible grandparent do not need to update here,
                    // since it is still the grandparent of varible node
                }
                node->parent->color = Node::BLACK;
                grandparent->color = Node::RED;
                RightRotate(grandparent);
            }
        }
        else
        {
            uncle = grandparent->left;
            if (uncle->color == Node::RED)
            {
                uncle->color = node->parent->color = Node::BLACK;
                grandparent->color = Node::RED;
                node = grandparent;
            }
            else
            {
                if (node == node->parent->left)
                {
                    node = node->parent;
                    RightRotate(node);
                    // notice that varible grandparent do not need to update here,
                    // since it is still the grandparent of varible node
                }
                node->parent->color = Node::BLACK;
                grandparent->color = Node::RED;
                LeftRotate(grandparent);
            }
        }
    }
    root_->color = Node::BLACK;
}

template <class Key, class T>
void RedBlackTree<Key, T>::Transplant(Node* old_node, Node* new_node)
{
    if (old_node->parent == nil_)
        root_ = new_node;
    else if (old_node == old_node->parent->left)
        old_node->parent->left = new_node;
    else
        old_node->parent->right = new_node;
    new_node->parent = old_node->parent;
}

template <class Key, class T>
typename RedBlackTree<Key, T>::Node* RedBlackTree<Key, T>::TreeMinimum(Node* sub_tree_root)
{
    while (sub_tree_root->left != nil_)
        sub_tree_root = sub_tree_root->left;
    return sub_tree_root;
}

template <class Key, class T>
typename RedBlackTree<Key, T>::Node* RedBlackTree<Key, T>::TreeMaximum(Node* sub_tree_root)
{
    while (sub_tree_root->right != nil_)
        sub_tree_root = sub_tree_root->right;
    return sub_tree_root;
}

template <class Key, class T>
typename RedBlackTree<Key, T>::Node* RedBlackTree<Key, T>::TreeSuccessor(Node* node)
{
    Node* parent;
    if (node->right != this->nil_)
        return this->TreeMinimum(node->right);
    parent = node->parent;
    while (parent != this->nil_ && parent->right == node)
    {
        node = parent;
        parent = node->parent;
    }
    return parent;
}

template <class Key, class T>
typename RedBlackTree<Key, T>::Node* RedBlackTree<Key, T>::TreePredecessor(Node* node)
{
    Node* parent;
    if (node->left)
        return TreeMaximum(node->left);
    parent = node->parent;
    while (parent && parent->left == node)
    {
        node = parent;
        parent = node->parent;
    }
    return parent;
}

template <class Key, class T>
void RedBlackTree<Key, T>::Delete(Iterator pos)
{
    Node *old, *replaced, *replaced_replaced;
    bool is_black_deleted;
    old = pos.node_;
    replaced = old;
    is_black_deleted = replaced->color == Node::BLACK;
    if (replaced->left == nil_)
    {
        // do NOT need two "replaced" to store moved node
        replaced_replaced = replaced->right;
        Transplant(replaced, replaced_replaced);
    }
    else if (replaced->right == nil_)
    {
        // do NOT need two "replaced" to store moved node
        replaced_replaced = replaced->left;
        Transplant(replaced, replaced_replaced);
    }
    else
    {
        // DO need two "replaced" to store moved node
        // re-assign varible "replaced" and "is_black_deleted"
        replaced = TreeMinimum(old->right);
        is_black_deleted = replaced->color == Node::BLACK;
        replaced_replaced = replaced->right;
        if (replaced->parent == old)
        {
            // neccessary when "replaced_replaced" is nil_
            replaced_replaced->parent = replaced;
        }
        else
        {
            Transplant(replaced, replaced_replaced);
            replaced->right = old->right;
            replaced->right->parent = replaced;
        }
        Transplant(old, replaced);
        replaced->left = old->left;
        replaced->left->parent = replaced;
        replaced->color = old->color;// this is why "is_black_deleted" need to re-assign in this case
    }
    if (is_black_deleted)
        // In order to maintain property 5,
        // "replaced_replaced" node has extra black (either "doubly black" or "red-and-black", contributes either 2 or 1)
        DeleteFixup(replaced_replaced);
    delete old;
}

template <class Key, class T>
void RedBlackTree<Key, T>::DeleteFixup(Node* node)
{
    Node* sibling;
    while (node != root_ && node->color == Node::BLACK)
    {
        if (node == node->parent->left)
        {
            sibling = node->parent->right;
            if (sibling->color == Node::RED)
            {
                sibling->color = Node::BLACK;
                node->parent->color = Node::RED;
                LeftRotate(node->parent);
                sibling = node->parent->right;
            }
            if (sibling->left->color == Node::BLACK && sibling->right->color == Node::BLACK)
            {
                sibling->color = Node::RED;
                node = node->parent;
            }
            else
            {
                if (sibling->right->color == Node::BLACK)
                {
                    sibling->left->color = Node::BLACK;
                    sibling->color = Node::RED;
                    RightRotate(sibling);
                    sibling = node->parent->right;
                }
                sibling->color = node->parent->color;
                node->parent->color = Node::BLACK;
                sibling->right->color = Node::BLACK;
                LeftRotate(node->parent);
                node = root_;
            }
        }
        else
        {
            sibling = node->parent->left;
            if (sibling->color == Node::RED)
            {
                sibling->color = Node::BLACK;
                node->parent->color = Node::RED;
                RightRotate(node->parent);
                sibling = node->parent->left;
            }
            if (sibling->left->color == Node::BLACK && sibling->right->color == Node::BLACK)
            {
                sibling->color = Node::RED;
                node = node->parent;
            }
            else
            {
                if (sibling->left->color == Node::BLACK)
                {
                    sibling->right->color = Node::BLACK;
                    sibling->color = Node::RED;
                    LeftRotate(sibling);
                    sibling = node->parent->left;
                }
                sibling->color = node->parent->color;
                node->parent->color = Node::BLACK;
                sibling->left->color = Node::BLACK;
                RightRotate(node->parent);
                node = root_;
            }
        }
    }
    node->color = Node::BLACK;
}

template <class Key, class T>
void RedBlackTree<Key, T>::Clear()
{
    Node *now, *parent;
    if (root_ == nil_) return;
    now = TreeMinimum(root_);
    while (now != nil_)
    {
        while (now->right != nil_)
        {
            now = TreeMinimum(now->right);
        }
        parent = now->parent;
        while (parent != nil_ && parent->right == now)
        {
            delete now;
            now = parent;
            parent = now->parent;
        }
        delete now;
        now = parent;
    }
    root_ = nil_;
}

template <class Key, class T>
typename RedBlackTree<Key, T>::Iterator RedBlackTree<Key, T>::Begin()
{
    return Iterator(TreeMinimum(root_), this);
}

template <class Key, class T>
typename RedBlackTree<Key, T>::Iterator RedBlackTree<Key, T>::End()
{
    return Iterator(nil_, this);
}

#endif
