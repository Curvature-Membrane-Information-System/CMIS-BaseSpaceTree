﻿#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

// 四叉树数据结构 
// 用来存储地图图片区块以及对应的地图范围
// 每个区块大小为 2048 * 2048
// 能够根据范围获取地图图片区块
// 加载图片均为实际 2048 * 2048 大小
// 用于实现地图的缩放
// 遍历./map/目录下的所有图片
// 1_1.png 为(1,1)区块的图片
// 1_2.png 为(1,2)区块的图片
// 2_1.png 为(2,1)区块的图片
// 2_2.png 为(2,2)区块的图片
// 以此类推

class QuadTree
{
public:
    QuadTree();
    ~QuadTree();

    // 加载地图图片
    void loadMap();

    // 获取地图图片
    cv::Mat getMap(const cv::Rect &rect);
    cv::Mat getMap_1st(const cv::Rect &rect);

private:
    void getMap(const cv::Mat& mat, const cv::Rect &rect, int x, int y);
    void getMap(const cv::Mat& mat, const cv::Rect &rect, const int id);
    // 是否递归检索过的区块的标记
    std::vector<bool> m_flag;

private:
    // 存储地图图片
    std::vector<cv::Mat> m_map;
    // 存储地图图片对应的地图范围
    std::vector<cv::Rect> m_rect;
    // 存储地图图片对应的地图区块位置在vector中的索引
    std::map<std::pair<int, int>, int> m_index;
    struct Adjacent
    {
        int up    =-1;
        int down  =-1;
        int left  =-1;
        int right =-1;
    };
    // 存储地图图片对应的区块相邻关系
    std::vector<Adjacent> m_adjacent;
};

QuadTree::QuadTree()
{
    loadMap();
}

QuadTree::~QuadTree()
{
}

void QuadTree::loadMap()
{
    // 遍历./map/目录下的所有图片
    // i_j.png 为(i,j)区块的图片
    // 以此类推 

    std::string path = "./map/";
    std::vector<std::string> files;

    // 遍历目录下的所有图片
    cv::glob(path, files, false);
    for (int i = 0; i < files.size(); i++)
    {
        // 获取图片名
        std::string name = files[i].substr(files[i].find_last_of("\\") + 1);
        // 获取图片区块坐标
        int y = std::stoi(name.substr(0, name.find("_")));
        int x = std::stoi(name.substr(name.find("_") + 1, name.find(".")));
        // 加载图片
        cv::Mat img = cv::imread(files[i]);
        // 图片大小归一化 2048 * 2048
        if (img.cols != 2048 || img.rows != 2048)
        {
            // 为了防止边缘出现淡化现象，放大时使用 INTER_CUBIC 插值
            cv::resize(img, img, cv::Size(2048, 2048), 0, 0, cv::INTER_CUBIC);
            //TODO: 插值算法需要优化
        }
        // 存储图片
        m_map.push_back(img);
        // 存储图片对应的地图范围
        m_rect.push_back(cv::Rect((x-1) * 2048, (y-1) * 2048, 2048, 2048));
        // 存储图片对应的地图区块位置在vector中的索引
        m_index[std::make_pair(x, y)] = i;
    }
    for (int i = 0; i < m_rect.size(); i++)
    {
        int x = m_rect[i].x / 2048 + 1;
        int y = m_rect[i].y / 2048 + 1;
        // 存储图片对应的区块相邻关系
        /*
              1,4
          2,3 2,4 2,5
              3,4
        */
        Adjacent adjacent;
        if (m_index.find(std::make_pair(x-1, y )) != m_index.end())
        {
            adjacent.up = m_index[std::make_pair(x-1, y )];
        }
        if (m_index.find(std::make_pair(x+1, y )) != m_index.end())
        {
            adjacent.down = m_index[std::make_pair(x+1, y )];
        }
        if (m_index.find(std::make_pair(x, y-1 )) != m_index.end())
        {
            adjacent.left = m_index[std::make_pair(x, y-1 )];
        }
        if (m_index.find(std::make_pair(x, y+1 )) != m_index.end())
        {
            adjacent.right = m_index[std::make_pair(x, y+1 )];
        }

        m_adjacent.push_back(adjacent);
    }
}



cv::Mat QuadTree::getMap(const cv::Rect &rect)
{
    // 获取中心点
    cv::Point center = rect.tl() + cv::Point(rect.width / 2, rect.height / 2);
    // 获取中心点所在的区块，需要向上取整
    int x = center.x / 2048 == 0 ? 1 : center.x / 2048 + 1;
    int y = center.y / 2048 == 0 ? 1 : center.y / 2048 + 1;
    int index  = -1;
    // 获取中心点所在的区块的索引
    if (m_index.find(std::make_pair(x, y)) == m_index.end())
    {
        // 中心点所在的区块不存在，就尝试获取四个顶点所在的区块
        // 顶点坐标
        std::vector<cv::Point> points;
        points.push_back(rect.tl());
        points.push_back(rect.tl() + cv::Point(rect.width, 0));
        points.push_back(rect.tl() + cv::Point(0, rect.height));
        points.push_back(rect.tl() + cv::Point(rect.width, rect.height));
        // 顶点所在的区块索引
        std::vector<int> indexs;
        for (int i = 0; i < points.size(); i++)
        {
            int x = points[i].x / 2048 == 0 ? 1 : points[i].x / 2048 + 1;
            int y = points[i].y / 2048 == 0 ? 1 : points[i].y / 2048 + 1;
            if (m_index.find(std::make_pair(x, y)) != m_index.end())
            {
                indexs.push_back(m_index[std::make_pair(x, y)]);
            }
        }
        // 如果四个顶点都不在区块中，就返回空
        if (indexs.size() == 0)
        {
            return cv::Mat::zeros(rect.size(), CV_8UC3);
        }
        // 如果四个顶点于存在某个区块，就以该区块为起点递归
        else
        {
            index = indexs[0];
        }

    }
    else
    {
        index = m_index[std::make_pair(x, y)];
    }
    // 初始化地图图片
    cv::Mat map = cv::Mat::zeros(rect.size(), CV_8UC3);

    // 根据相邻关系遍历取交集
    // 以中心点所在区块为起点，向上下左右遍历
    // 以此类推
    m_flag = std::vector<bool>(m_map.size(), false);
    // getMap(map, rect, x, y);
    getMap(map, rect, index);    
    return map;
}


cv::Mat QuadTree::getMap_1st(const cv::Rect &rect)
{
    // 获取中心点
    cv::Point center = rect.tl() + cv::Point(rect.width / 2, rect.height / 2);
    // 获取中心点所在的区块
    int x = center.x / 2048;
    int y = center.y / 2048;
    // 初始化地图图片
    static cv::Mat map = cv::Mat::zeros(rect.size(), CV_8UC3);
    // 根据范围遍历取交集
    for (int i = 0; i < m_rect.size(); i++)
    {
        cv::Rect r = rect & m_rect[i];
        if (r.area() > 0)
        {
            // 获取相对于地图图片的范围
            cv::Rect r1 = r - m_rect[i].tl();
            // 获取相对于区块图片的范围
            cv::Rect r2 = r - rect.tl();
            // 获取区块图片
            cv::Mat img = m_map[i];
            // 取交集
            img(r1).copyTo(map(r2));
        }
    }

    return map;
}

void QuadTree::getMap(const cv::Mat &mat, const cv::Rect &rect, int x, int y)
{
    // 获取x,y区块的索引
    if (m_index.find(std::make_pair(x, y)) == m_index.end())
    {
        return;
    }
    int index = m_index[std::make_pair(x, y)];
    // 如果该区块已经遍历过，直接返回
    if (m_flag[index])
    {
        return;
    }
    // 取交集
    cv::Rect r = rect & m_rect[index];
    // 如果交集面积为0，直接返回
    if (r.area() == 0)
    {
        return;
    }
    // 获取相对于地图图片的范围
    cv::Rect r1 = r - m_rect[index].tl();
    // 获取相对于区块图片的范围
    cv::Rect r2 = r - rect.tl();
    // // 获取区块图片
    // cv::Mat img = m_map[index];
    // // 取交集
    // img(r1).copyTo(mat(r2));
    m_map[index](r1).copyTo(mat(r2));
    // #define SHOW
    #ifdef SHOW
    cv::imshow("map", mat);
    cv::waitKey(100);
    #endif
    // 标记该区块已经遍历过
    m_flag[index] = true;
    // 获取相邻区块
    Adjacent adjacent = m_adjacent[index];
    // 递归遍历相邻区块
    getMap(mat, rect, x-1, y);
    getMap(mat, rect, x+1, y);
    getMap(mat, rect, x, y-1);
    getMap(mat, rect, x, y+1);
}

void QuadTree::getMap(const cv::Mat &mat, const cv::Rect &rect, const int id)
{
    // 如果该区块已经遍历过，直接返回
    if (m_flag[id])
    {
        return;
    }
    // 取交集
    cv::Rect r = rect & m_rect[id];
    // 如果交集面积为0，直接返回
    if (r.area() == 0)
    {
        return;
    }
    // 获取相对于地图图片的范围
    cv::Rect r1 = r - m_rect[id].tl();
    // 获取相对于区块图片的范围
    cv::Rect r2 = r - rect.tl();
    // // 获取区块图片
    // cv::Mat img = m_map[id];
    // // 取交集
    // img(r1).copyTo(mat(r2));
    m_map[id](r1).copyTo(mat(r2));
    // #define SHOW
    #ifdef SHOW
    cv::imshow("map", mat);
    cv::waitKey(100);
    #endif
    // 标记该区块已经遍历过
    m_flag[id] = true;
    // 获取相邻区块
    Adjacent adjacent = m_adjacent[id];
    // 递归遍历相邻区块
    if (adjacent.left != -1)
    {
        getMap(mat, rect, adjacent.left);
    }
    if (adjacent.right != -1)
    {
        getMap(mat, rect, adjacent.right);
    }
    if (adjacent.up != -1)
    {
        getMap(mat, rect, adjacent.up);
    }
    if (adjacent.down != -1)
    {
        getMap(mat, rect, adjacent.down);
    }
}

void test(QuadTree &q, int x,int y, int w, int h)
{
    cv::Rect rect(x, y, w, h);
    cv::Mat map = q.getMap(rect);
    cv::imshow("map", map);
    cv::waitKey(1000);
}

int main()
{
    QuadTree quadTree;
    
    test(quadTree, 0, 0, 100, 100);
    test(quadTree, 0, 0, 100, 100);
    test(quadTree, -1000, 0, 100, 100);
    test(quadTree, -100, 0, 100, 100);
    test(quadTree, -100, 0, 200, 100);
    test(quadTree, 100, 0, 200, 200);
    test(quadTree, 100, 0, 2000, 2000);
    test(quadTree, 1000, 0, 2000, 2000);
	
    cv::waitKey(10);

    //// 计时比较 getMap 和 getMap_1st 的效率
    //// 循环100次

    //// getMap
    //auto start = std::chrono::steady_clock::now();
    //for(int i = 0; i < 10; i++)
    //{
    //    mat = quadTree.getMap(cv::Rect(5000+i, 3000+i, 200, 200));
    //}
    //auto end = std::chrono::steady_clock::now();
    //std::chrono::duration<double> diff = end - start;
    //std::cout << "getMap: " << diff.count() << " s" << std::endl;

    //// getMap_1st
    //start = std::chrono::steady_clock::now();
    //for(int i = 0; i < 10; i++)
    //{
    //    mat = quadTree.getMap_1st(cv::Rect(5000+i, 3000+i, 200, 200));
    //}
    //end = std::chrono::steady_clock::now();
    //diff = end - start;
    //std::cout << "getMap_1st: " << diff.count() << " s" << std::endl;


    return 0;
}