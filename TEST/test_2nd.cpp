#include "test_2nd.h"

#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

#include "2nd.box.h"


#include <cassert>
#include <chrono>
#include <iostream>
#include <random>

#include <cassert>
#include <algorithm>
#include <array>
#include <memory>
#include <type_traits>
#include <vector>

namespace t2nd
{

    template<typename T, typename GetBox, typename Equal = std::equal_to<T>, typename Float = float>
    class QuadTree
    {
        static_assert(std::is_convertible_v<std::invoke_result_t<GetBox, const T&>, Box<Float>>,
            "GetBox must be a callable of signature Box<Float>(const T&)");
        static_assert(std::is_convertible_v<std::invoke_result_t<Equal, const T&, const T&>, bool>,
            "Equal must be a callable of signature bool(const T&, const T&)");
        static_assert(std::is_arithmetic_v<Float>);

    public:
        QuadTree(const Box<Float>& box, const GetBox& getBox = GetBox(),
            const Equal& equal = Equal()) :
            mBox(box), mRoot(std::make_unique<Node>()), mGetBox(getBox), mEqual(equal)
        {

        }

        void add(const T& value)
        {
            add(mRoot.get(), 0, mBox, value);
        }

        void remove(const T& value)
        {
            remove(mRoot.get(), mBox, value);
        }

        std::vector<T> query(const Box<Float>& box) const
        {
            auto values = std::vector<T>();
            query(mRoot.get(), mBox, box, values);
            return values;
        }

        std::vector<std::pair<T, T>> findAllIntersections() const
        {
            auto intersections = std::vector<std::pair<T, T>>();
            findAllIntersections(mRoot.get(), intersections);
            return intersections;
        }

    private:
        static constexpr auto Threshold = std::size_t(16);
        static constexpr auto MaxDepth = std::size_t(8);

        struct Node
        {
            std::array<std::unique_ptr<Node>, 4> children;
            std::vector<T> values;
        };

        Box<Float> mBox;
        std::unique_ptr<Node> mRoot;
        GetBox mGetBox;
        Equal mEqual;

        bool isLeaf(const Node* node) const
        {
            return !static_cast<bool>(node->children[0]);
        }

        Box<Float> computeBox(const Box<Float>& box, int i) const
        {
            auto origin = box.getTopLeft();
            auto childSize = box.getSize() / static_cast<Float>(2);
            switch (i)
            {
                // North West
            case 0:
                return Box<Float>(origin, childSize);
                // Norst East
            case 1:
                return Box<Float>(Point<Float>(origin.x + childSize.x, origin.y), childSize);
                // South West
            case 2:
                return Box<Float>(Point<Float>(origin.x, origin.y + childSize.y), childSize);
                // South East
            case 3:
                return Box<Float>(origin + childSize, childSize);
            default:
                assert(false && "Invalid child index");
                return Box<Float>();
            }
        }

        int getQuadrant(const Box<Float>& nodeBox, const Box<Float>& valueBox) const
        {
            auto center = nodeBox.getCenter();
            // West
            if (valueBox.getRight() < center.x)
            {
                // North West
                if (valueBox.getBottom() < center.y)
                    return 0;
                // South West
                else if (valueBox.top >= center.y)
                    return 2;
                // Not contained in any quadrant
                else
                    return -1;
            }
            // East
            else if (valueBox.left >= center.x)
            {
                // North East
                if (valueBox.getBottom() < center.y)
                    return 1;
                // South East
                else if (valueBox.top >= center.y)
                    return 3;
                // Not contained in any quadrant
                else
                    return -1;
            }
            // Not contained in any quadrant
            else
                return -1;
        }

        void add(Node* node, std::size_t depth, const Box<Float>& box, const T& value)
        {
            assert(node != nullptr);
            assert(box.contains(mGetBox(value)));
            if (isLeaf(node))
            {
                // Insert the value in this node if possible
                if (depth >= MaxDepth || node->values.size() < Threshold)
                    node->values.push_back(value);
                // Otherwise, we split and we try again
                else
                {
                    split(node, box);
                    add(node, depth, box, value);
                }
            }
            else
            {
                auto i = getQuadrant(box, mGetBox(value));
                // Add the value in a child if the value is entirely contained in it
                if (i != -1)
                    add(node->children[static_cast<std::size_t>(i)].get(), depth + 1, computeBox(box, i), value);
                // Otherwise, we add the value in the current node
                else
                    node->values.push_back(value);
            }
        }

        void split(Node* node, const Box<Float>& box)
        {
            assert(node != nullptr);
            assert(isLeaf(node) && "Only leaves can be split");
            // Create children
            for (auto& child : node->children)
                child = std::make_unique<Node>();
            // Assign values to children
            auto newValues = std::vector<T>(); // New values for this node
            for (const auto& value : node->values)
            {
                auto i = getQuadrant(box, mGetBox(value));
                if (i != -1)
                    node->children[static_cast<std::size_t>(i)]->values.push_back(value);
                else
                    newValues.push_back(value);
            }
            node->values = std::move(newValues);
        }

        bool remove(Node* node, const Box<Float>& box, const T& value)
        {
            assert(node != nullptr);
            assert(box.contains(mGetBox(value)));
            if (isLeaf(node))
            {
                // Remove the value from node
                removeValue(node, value);
                return true;
            }
            else
            {
                // Remove the value in a child if the value is entirely contained in it
                auto i = getQuadrant(box, mGetBox(value));
                if (i != -1)
                {
                    if (remove(node->children[static_cast<std::size_t>(i)].get(), computeBox(box, i), value))
                        return tryMerge(node);
                }
                // Otherwise, we remove the value from the current node
                else
                    removeValue(node, value);
                return false;
            }
        }

        void removeValue(Node* node, const T& value)
        {
            // Find the value in node->values
            auto it = std::find_if(std::begin(node->values), std::end(node->values),
                [this, &value](const auto& rhs) { return mEqual(value, rhs); });
            assert(it != std::end(node->values) && "Trying to remove a value that is not present in the node");
            // Swap with the last element and pop back
            *it = std::move(node->values.back());
            node->values.pop_back();
        }

        bool tryMerge(Node* node)
        {
            assert(node != nullptr);
            assert(!isLeaf(node) && "Only interior nodes can be merged");
            auto nbValues = node->values.size();
            for (const auto& child : node->children)
            {
                if (!isLeaf(child.get()))
                    return false;
                nbValues += child->values.size();
            }
            if (nbValues <= Threshold)
            {
                node->values.reserve(nbValues);
                // Merge the values of all the children
                for (const auto& child : node->children)
                {
                    for (const auto& value : child->values)
                        node->values.push_back(value);
                }
                // Remove the children
                for (auto& child : node->children)
                    child.reset();
                return true;
            }
            else
                return false;
        }

        void query(Node* node, const Box<Float>& box, const Box<Float>& queryBox, std::vector<T>& values) const
        {
            assert(node != nullptr);
            assert(queryBox.intersects(box));
            for (const auto& value : node->values)
            {
                if (queryBox.intersects(mGetBox(value)))
                    values.push_back(value);
            }
            if (!isLeaf(node))
            {
                for (auto i = std::size_t(0); i < node->children.size(); ++i)
                {
                    auto childBox = computeBox(box, static_cast<int>(i));
                    if (queryBox.intersects(childBox))
                        query(node->children[i].get(), childBox, queryBox, values);
                }
            }
        }

        void findAllIntersections(Node* node, std::vector<std::pair<T, T>>& intersections) const
        {
            // Find intersections between values stored in this node
            // Make sure to not report the same intersection twice
            for (auto i = std::size_t(0); i < node->values.size(); ++i)
            {
                for (auto j = std::size_t(0); j < i; ++j)
                {
                    if (mGetBox(node->values[i]).intersects(mGetBox(node->values[j])))
                        intersections.emplace_back(node->values[i], node->values[j]);
                }
            }
            if (!isLeaf(node))
            {
                // Values in this node can intersect values in descendants
                for (const auto& child : node->children)
                {
                    for (const auto& value : node->values)
                        findIntersectionsInDescendants(child.get(), value, intersections);
                }
                // Find intersections in children
                for (const auto& child : node->children)
                    findAllIntersections(child.get(), intersections);
            }
        }

        void findIntersectionsInDescendants(Node* node, const T& value, std::vector<std::pair<T, T>>& intersections) const
        {
            // Test against the values stored in this node
            for (const auto& other : node->values)
            {
                if (mGetBox(value).intersects(mGetBox(other)))
                    intersections.emplace_back(value, other);
            }
            // Test against values stored into descendants of this node
            if (!isLeaf(node))
            {
                for (const auto& child : node->children)
                    findIntersectionsInDescendants(child.get(), value, intersections);
            }
        }
    };

}

using namespace t2nd;

struct Node
{
    Box<float> box;
    std::size_t id;
};

std::vector<Node> generateRandomNodes(std::size_t n)
{
    auto generator = std::default_random_engine();
    auto originDistribution = std::uniform_real_distribution(0.0f, 1.0f);
    auto sizeDistribution = std::uniform_real_distribution(0.0f, 0.01f);
    auto nodes = std::vector<Node>(n);
    for (auto i = std::size_t(0); i < n; ++i)
    {
        nodes[i].box.left = originDistribution(generator);
        nodes[i].box.top = originDistribution(generator);
        nodes[i].box.width = std::min(1.0f - nodes[i].box.left, sizeDistribution(generator));
        nodes[i].box.height = std::min(1.0f - nodes[i].box.top, sizeDistribution(generator));
        nodes[i].id = i;
    }
    return nodes;
}

std::vector<std::pair<Node*, Node*>> computeIntersections(std::vector<Node>& nodes, const std::vector<bool>& removed)
{
    auto intersections = std::vector<std::pair<Node*, Node*>>();
    for (auto i = std::size_t(0); i < nodes.size(); ++i)
    {
        if (removed.size() == 0 || !removed[i])
        {
            for (auto j = std::size_t(0); j < i; ++j)
            {
                if (removed.size() == 0 || !removed[j])
                {
                    if (nodes[i].box.intersects(nodes[j].box))
                        intersections.emplace_back(&nodes[i], &nodes[j]);
                }
            }
        }
    }
    return intersections;
}

void checkIntersections(std::vector<Node*> nodes1, std::vector<Node*> nodes2)
{
    assert(nodes1.size() == nodes2.size());
    std::sort(std::begin(nodes1), std::end(nodes1));
    std::sort(std::begin(nodes2), std::end(nodes2));
    for (auto i = std::size_t(0); i < nodes1.size(); ++i)
        assert(nodes1[i] == nodes2[i]);
}

void test_2nd()
{
    auto n = std::size_t(1000);
    auto getBox = [](Node* node)
    {
        return node->box;
    };
    auto box = Box(0.0f, 0.0f, 1.0f, 1.0f);
    auto nodes = generateRandomNodes(n);
    // Add nodes to quadtree
    auto quadtree = QuadTree<Node*, decltype(getBox)>(box, getBox);
    auto start1 = std::chrono::steady_clock::now();
    for (auto& node : nodes)
        quadtree.add(&node);
    // Randomly remove some nodes
    auto generator = std::default_random_engine();
    auto deathDistribution = std::uniform_int_distribution(0, 1);
    auto removed = std::vector<bool>(nodes.size(), false);
    std::generate(std::begin(removed), std::end(removed),
        [&generator, &deathDistribution]() { return deathDistribution(generator); });
    for (auto& node : nodes)
    {
        if (removed[node.id])
            quadtree.remove(&node);
    }
    // QuadTree
    auto intersections1 = std::vector<std::vector<Node*>>(nodes.size());
    auto start2 = std::chrono::steady_clock::now();
    for (const auto& node : nodes)
    {
        if (!removed[node.id])
            intersections1[node.id] = quadtree.query(node.box);
    }
    auto duration2 = std::chrono::steady_clock::now() - start2;
    auto duration1 = std::chrono::steady_clock::now() - start1;
    std::cout << "quadtree: " << std::chrono::duration_cast<std::chrono::microseconds>(duration2).count() << "us" << '\n';
    std::cout << "quadtree with creation: " << std::chrono::duration_cast<std::chrono::microseconds>(duration1).count() << "us" << '\n';
    // Brute force
    auto intersections2 = computeIntersections(nodes, removed);
    // Check
    //checkIntersections(intersections1[node.id], intersections2[node.id]);
    // Find all intersections
    auto intersections3 = quadtree.findAllIntersections();
    std::cout << intersections3.size() << '\n';
    std::cout << intersections2.size() << '\n';
	
}