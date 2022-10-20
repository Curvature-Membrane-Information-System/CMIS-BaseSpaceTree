#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

// 四叉树数据结构 
// 用来存储地图图片区块以及对应的地图范围
// 每个区块大小为 256 * 256
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
    QuadTree() {}
    QuadTree(int x, int y, int width, int height, int level, int maxLevel)
    {
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
        this->level = level;
        this->maxLevel = maxLevel;
    }

    // 加载图片
    void load(std::string path)
    {
        // 遍历目录下的所有图片
        cv::glob(path, files, false);
        for (int i = 0; i < files.size(); i++)
        {
            // 获取图片名
            std::string name = files[i].substr(files[i].find_last_of("\\") + 1);
            // 获取图片区块坐标
            int x = std::stoi(name.substr(0, name.find("_")));
            int y = std::stoi(name.substr(name.find("_") + 1, name.find(".")));
            // 加载图片
            cv::Mat img = cv::imread(files[i]);
            // 存储图片
            images.push_back(img);
            // 存储图片区块坐标
            positions.push_back(cv::Point(x, y));
        }
    }

    // 获取图片
    cv::Mat get(int x, int y, int width, int height)
    {
        // 获取图片区块坐标
        int x1 = x / 256;
        int y1 = y / 256;
        int x2 = (x + width) / 256;
        int y2 = (y + height) / 256;
        // 获取图片区块
        cv::Mat img1 = get(x1, y1);
        cv::Mat img2 = get(x2, y1);
        cv::Mat img3 = get(x1, y2);
        cv::Mat img4 = get(x2, y2);
        // 拼接图片
        cv::Mat img;
        cv::hconcat(img1, img2, img);
        cv::Mat img_;
        cv::hconcat(img3, img4, img_);
        cv::vconcat(img, img_, img);
        // 返回图片
        return img;
    }

    // 获取图片
    cv::Mat get(int x, int y)
    {
        // 获取图片区块
        cv::Mat img = cv::Mat;
        for (int i = 0; i < positions.size(); i++)
        {
            if (positions[i].x == x && positions[i].y == y)
            {
                img = images[i];
                break;
            }
        }
        // 返回图片
        return img;
    }

private:
    // 图片区块坐标
    int x;
    int y;
    // 图片区块大小
    int width;
    int height;
    // 图片区块层级
    int level;
    // 图片区块最大层级
    int maxLevel;
    // 图片区块
    std::vector<cv::Mat> images;
    // 图片区块坐标
    std::vector<cv::Point> positions;
    // 图片路径
    std::vector<std::string> files;
};

int main()
{
    // 加载图片
    QuadTree tree;
    tree.load("./map/*.png");
    // 获取图片
    cv::Mat img = tree.get(0, 0, 2048, 2048);
    // 显示图片
    cv::imshow("img", img);
    cv::waitKey(0);
    return 0;
}
