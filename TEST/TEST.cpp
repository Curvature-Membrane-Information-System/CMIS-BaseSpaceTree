#include <iostream>
#include <vector>
#include <opencv2/opencv.h>

// 四叉树数据结构 
// 用来存储地图图片区块以及对应的地图范围
// 能够根据范围获取地图图片

class QuadTree
{
public:
    QuadTree(int x, int y, int width, int height)
    {
        m_x = x;
        m_y = y;
        m_width = width;
        m_height = height;
    }

    void AddChild(QuadTree* child)
    {
        m_children.push_back(child);
    }

    QuadTree* GetChild(int index)
    {
        if (index >= m_children.size()) return nullptr;
        return m_children[index];
    }

    int GetX()
    {
        return m_x;
    }

    int GetY()
    {
        return m_y;
    }

    int GetWidth()
    {
        return m_width;
    }

    int GetHeight()
    {
        return m_height;
    }

private:
    int m_x;
    int m_y;
    int m_width;
    int m_height;
    std::vector<QuadTree*> m_children;
};

// 地图图片区块

class MapBlock
{
public:
    MapBlock(int x, int y, int width, int height)
    {
        m_x = x;
        m_y = y;
        m_width = width;
        m_height = height;
    }

    int GetX()
    {
        return m_x;
    }

    int GetY()
    {
        return m_y;
    }

    int GetWidth()
    {
        return m_width;
    }

    int GetHeight()
    {
        return m_height;
    }

private:
    int m_x;
    int m_y;
    int m_width;
    int m_height;
};

// 地图图片区块管理器

class MapBlockManager
{
public:
    MapBlockManager()
    {
        m_mapBlockTree = new QuadTree(0, 0, 1000, 1000);
    }

    ~MapBlockManager()
    {
        delete m_mapBlockTree;
    }

    void AddMapBlock(MapBlock* mapBlock)
    {
        m_mapBlocks.push_back(mapBlock);
    }

    void BuildMapBlockTree()
    {
        for (int i = 0; i < m_mapBlocks.size(); i++)
        {
            MapBlock* mapBlock = m_mapBlocks[i];
            QuadTree* quadTree = m_mapBlockTree;
            while (true)
            {
                int x = quadTree->GetX();
                int y = quadTree->GetY();
                int width = quadTree->GetWidth();
                int height = quadTree->GetHeight();
                int mapBlockX = mapBlock->GetX();
                int mapBlockY = mapBlock->GetY();
                int mapBlockWidth = mapBlock->GetWidth();
                int mapBlockHeight = mapBlock->GetHeight();
                if (mapBlockX >= x && mapBlockX + mapBlockWidth <= x + width && mapBlockY >= y && mapBlockY + mapBlockHeight <= y + height)
                {
                    if (quadTree->GetChild(0) == NULL)
                    {
                        quadTree->AddChild(new QuadTree(x, y, width / 2, height / 2));
                        quadTree->AddChild(new QuadTree(x + width / 2, y, width / 2, height / 2));
                        quadTree->AddChild(new QuadTree(x, y + height / 2, width / 2, height / 2));
                        quadTree->AddChild(new QuadTree(x + width / 2, y + height / 2, width / 2, height / 2));
                    }
                    quadTree = quadTree->GetChild(0);
                }
                else if (mapBlockX >= x + width / 2 && mapBlockX + mapBlockWidth <= x + width && mapBlockY >= y && mapBlockY + mapBlockHeight <= y + height)
                {
                    if (quadTree->GetChild(1) == NULL)
                    {
                        quadTree->AddChild(new QuadTree(x, y, width / 2, height / 2));
                        quadTree->AddChild(new QuadTree(x + width / 2, y, width / 2, height / 2));
                        quadTree->AddChild(new QuadTree(x, y + height / 2, width / 2, height / 2));
                        quadTree->AddChild(new QuadTree(x + width / 2, y + height / 2, width / 2, height / 2));
                    }
                    quadTree = quadTree->GetChild(1);
                }
                else if (mapBlockX >= x && mapBlockX + mapBlockWidth <= x + width && mapBlockY >= y + height / 2 && mapBlockY + mapBlockHeight <= y + height)
                {
                    if (quadTree->GetChild(2) == NULL)
                    {
                        quadTree->AddChild(new QuadTree(x, y, width / 2, height / 2));
                        quadTree->AddChild(new QuadTree(x + width / 2, y, width / 2, height / 2));
                        quadTree->AddChild(new QuadTree(x, y + height / 2, width / 2, height / 2));
                        quadTree->AddChild(new QuadTree(x + width / 2, y + height / 2, width / 2, height / 2));
                    }
                    quadTree = quadTree->GetChild(2);
                }
                else if (mapBlockX >= x + width / 2 && mapBlockX + mapBlockWidth <= x + width && mapBlockY >= y + height / 2 && mapBlockY + mapBlockHeight <= y + height)
                {
                    if (quadTree->GetChild(3) == NULL)
                    {
                        quadTree->AddChild(new QuadTree(x, y, width / 2, height / 2));
                        quadTree->AddChild(new QuadTree(x + width / 2, y, width / 2, height / 2));
                        quadTree->AddChild(new QuadTree(x, y + height / 2, width / 2, height / 2));
                        quadTree->AddChild(new QuadTree(x + width / 2, y + height / 2, width / 2, height / 2));
                    }
                    quadTree = quadTree->GetChild(3);
                }
                else
                {
                    break;
                }
            }
        }
    }

private:
    std::vector<MapBlock*> m_mapBlocks;
    QuadTree* m_mapBlockTree;
};

int main()
{
    MapBlockManager mapBlockManager;
    mapBlockManager.AddMapBlock(new MapBlock(0, 0, 100, 100));
    mapBlockManager.AddMapBlock(new MapBlock(0, 100, 100, 100));
    mapBlockManager.AddMapBlock(new MapBlock(100, 0, 100, 100));
    mapBlockManager.AddMapBlock(new MapBlock(100, 100, 100, 100));
    mapBlockManager.BuildMapBlockTree();
    return 0;
}

