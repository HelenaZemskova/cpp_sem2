// g++ -std=c++23 -Wall -Wextra -o 09_02 09_02.cpp && ./09_02


#include <cassert>
#include <functional>
#include <iostream>
#include <memory>
#include <queue>
#include <vector>

class Tree
{
public :

    struct Node
    {
        Node(int value) : m_value(value) {}

       ~Node()
        {
            std::cout << "Node::~Node : value = " << m_value << "\n";
        }

        int m_value = 0;

        std::shared_ptr < Node > m_left;
        std::shared_ptr < Node > m_right;

        std::weak_ptr   < Node > m_parent;
    };

    void traverse_v1() const
    {
        if (!m_root) return;

        std::cout << "traverse_v1 (BFS) : ";

        std::queue < std::shared_ptr < Node > > queue;

        queue.push(m_root);

        while (!queue.empty())
        {
            auto node = queue.front();
            queue.pop();

            std::cout << node->m_value << " ";

            if (node->m_left)  queue.push(node->m_left);
            if (node->m_right) queue.push(node->m_right);
        }

        std::cout << "\n";
    }

    void traverse_v2() const
    {
        std::cout << "traverse_v2 (DFS) : ";

        dfs(m_root);

        std::cout << "\n";
    }

    std::shared_ptr < Node > m_root;

private :

    void dfs(std::shared_ptr < Node > const & node) const
    {
        if (!node) return;

        std::cout << node->m_value << " ";

        dfs(node->m_left);
        dfs(node->m_right);
    }
};

int main()
{
    Tree tree;

    tree.m_root = std::make_shared < Tree::Node > (1);

    tree.m_root->m_left  = std::make_shared < Tree::Node > (2);
    tree.m_root->m_right = std::make_shared < Tree::Node > (3);

    tree.m_root->m_left ->m_parent = tree.m_root;
    tree.m_root->m_right->m_parent = tree.m_root;

    auto & left  = tree.m_root->m_left;
    auto & right = tree.m_root->m_right;

    left->m_left  = std::make_shared < Tree::Node > (4);
    left->m_right = std::make_shared < Tree::Node > (5);

    left->m_left ->m_parent = left;
    left->m_right->m_parent = left;

    right->m_left  = std::make_shared < Tree::Node > (6);
    right->m_right = std::make_shared < Tree::Node > (7);

    right->m_left ->m_parent = right;
    right->m_right->m_parent = right;

    assert(tree.m_root->m_value == 1);

    assert(tree.m_root->m_left ->m_value == 2);
    assert(tree.m_root->m_right->m_value == 3);

    assert(left->m_left ->m_value == 4);
    assert(left->m_right->m_value == 5);

    assert(right->m_left ->m_value == 6);
    assert(right->m_right->m_value == 7);

    assert(tree.m_root->m_left ->m_parent.lock() == tree.m_root);
    assert(tree.m_root->m_right->m_parent.lock() == tree.m_root);
    assert(left->m_left ->m_parent.lock() == left);
    assert(left->m_right->m_parent.lock() == left);
    assert(right->m_left ->m_parent.lock() == right);
    assert(right->m_right->m_parent.lock() == right);

    {
        std::vector < int > bfs;

        std::queue < std::shared_ptr < Tree::Node > > q;
        q.push(tree.m_root);

        while (!q.empty())
        {
            auto node = q.front(); q.pop();
            bfs.push_back(node->m_value);
            if (node->m_left)  q.push(node->m_left);
            if (node->m_right) q.push(node->m_right);
        }

        assert((bfs == std::vector < int > { 1, 2, 3, 4, 5, 6, 7 }));
    }

    {
        std::vector < int > dfs;

        std::function < void(std::shared_ptr < Tree::Node > const &) > collect;

        collect = [&](std::shared_ptr < Tree::Node > const & node)
        {
            if (!node) return;
            dfs.push_back(node->m_value);
            collect(node->m_left);
            collect(node->m_right);
        };

        collect(tree.m_root);

        assert((dfs == std::vector < int > { 1, 2, 4, 5, 3, 6, 7 }));
    }

    tree.traverse_v1();
    tree.traverse_v2();

    std::cout << "main : end of scope\n";
    std::cout << "All tests passed successfully.\n";
}
