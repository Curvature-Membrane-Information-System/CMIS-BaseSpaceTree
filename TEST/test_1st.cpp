#include "test_1st.h"

#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

// �Ĳ������ݽṹ 
// �����洢��ͼͼƬ�����Լ���Ӧ�ĵ�ͼ��Χ
// ÿ�������СΪ 2048 * 2048
// �ܹ����ݷ�Χ��ȡ��ͼͼƬ����
// ����ͼƬ��Ϊʵ�� 2048 * 2048 ��С
// ����ʵ�ֵ�ͼ������
// ����./map/Ŀ¼�µ�����ͼƬ
// 1_1.png Ϊ(1,1)�����ͼƬ
// 1_2.png Ϊ(1,2)�����ͼƬ
// 2_1.png Ϊ(2,1)�����ͼƬ
// 2_2.png Ϊ(2,2)�����ͼƬ
// �Դ�����

class QuadTree
{
public:
    QuadTree();
    ~QuadTree();

    // ���ص�ͼͼƬ
    void loadMap();

    // ��ȡ��ͼͼƬ
    cv::Mat getMap(const cv::Rect& rect);
    cv::Mat getMap_1st(const cv::Rect& rect);

    cv::Mat getAllMap();

private:
    void getMap(const cv::Mat& mat, const cv::Rect& rect, int x, int y);
    void getMap(const cv::Mat& mat, const cv::Rect& rect, const int id);
    // �Ƿ�ݹ������������ı��
    std::vector<bool> m_flag;

private:
    // �洢��ͼͼƬ
    std::vector<cv::Mat> m_map;
    // �洢��ͼͼƬ��Ӧ�ĵ�ͼ��Χ
    std::vector<cv::Rect> m_rect;
    // �洢��ͼͼƬ��Ӧ�ĵ�ͼ����λ����vector�е�����
    std::map<std::pair<int, int>, int> m_index;
    struct Adjacent
    {
        int up = -1;
        int down = -1;
        int left = -1;
        int right = -1;
    };
    // �洢��ͼͼƬ��Ӧ���������ڹ�ϵ
    std::vector<Adjacent> m_adjacent;
    cv::Rect min_rect;
    cv::Point min_tl_point;
    cv::Point min_br_point;

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
    // ����./map/Ŀ¼�µ�����ͼƬ
    // i_j.png Ϊ(i,j)�����ͼƬ
    // �Դ����� 

    std::string path = "./map/";
    std::vector<std::string> files;

    // ����Ŀ¼�µ�����ͼƬ
    cv::glob(path, files, false);
    for (int i = 0; i < files.size(); i++)
    {
        // ��ȡͼƬ��
        std::string name = files[i].substr(files[i].find_last_of("\\") + 1);
        // ��ȡͼƬ��������
        int y = std::stoi(name.substr(0, name.find("_")));
        int x = std::stoi(name.substr(name.find("_") + 1, name.find(".")));
        // ����ͼƬ
        cv::Mat img = cv::imread(files[i]);
        // ͼƬ��С��һ�� 2048 * 2048
        if (img.cols != 2048 || img.rows != 2048)
        {
            // Ϊ�˷�ֹ��Ե���ֵ������󣬷Ŵ�ʱʹ�� INTER_NEAREST ��ֵ
			if (img.cols > 512 || img.rows > 512)
				cv::resize(img, img, cv::Size(2048, 2048), 0, 0, cv::INTER_NEAREST);
			else
				cv::resize(img, img, cv::Size(2048, 2048), 0, 0, cv::INTER_CUBIC);
        }
        // �洢ͼƬ
        m_map.push_back(img);
        // �洢ͼƬ��Ӧ�ĵ�ͼ��Χ
        int x_org = (x - 1) * 2048;
        int y_org = (y - 1) * 2048;
        m_rect.push_back(cv::Rect(x_org,y_org, 2048, 2048));
        // �洢ͼƬ��Ӧ�ĵ�ͼ����λ����vector�е�����
        m_index[std::make_pair(x, y)] = i;

        // 
        if (x_org < min_tl_point.x)
        {
            min_tl_point.x = x_org;
        }   
        if (y_org < min_tl_point.y)
        {
            min_tl_point.y = y_org;
        }
        if (x_org > min_br_point.x)
        {
            min_br_point.x = x_org;
        }
        if (y_org > min_br_point.y)
        {
            min_br_point.y = y_org;
        }

        min_rect = cv::Rect(min_tl_point,   min_br_point + cv::Point(2048,2048));
    }
    for (int i = 0; i < m_rect.size(); i++)
    {
        int x = m_rect[i].x / 2048 + 1;
        int y = m_rect[i].y / 2048 + 1;
        // �洢ͼƬ��Ӧ���������ڹ�ϵ
        /*
              1,4
          2,3 2,4 2,5
              3,4
        */
        Adjacent adjacent;
        if (m_index.find(std::make_pair(x - 1, y)) != m_index.end())
        {
            adjacent.up = m_index[std::make_pair(x - 1, y)];
        }
        if (m_index.find(std::make_pair(x + 1, y)) != m_index.end())
        {
            adjacent.down = m_index[std::make_pair(x + 1, y)];
        }
        if (m_index.find(std::make_pair(x, y - 1)) != m_index.end())
        {
            adjacent.left = m_index[std::make_pair(x, y - 1)];
        }
        if (m_index.find(std::make_pair(x, y + 1)) != m_index.end())
        {
            adjacent.right = m_index[std::make_pair(x, y + 1)];
        }

        m_adjacent.push_back(adjacent);
    }
}



cv::Mat QuadTree::getMap(const cv::Rect& rect)
{
    // ��ȡ���ĵ�
    cv::Point center = rect.tl() + cv::Point(rect.width / 2, rect.height / 2);
    // ��ȡ���ĵ����ڵ����飬��Ҫ����ȡ��
    int x = center.x / 2048 == 0 ? 1 : center.x / 2048 + 1;
    int y = center.y / 2048 == 0 ? 1 : center.y / 2048 + 1;
    int index = -1;
    // ��ȡ���ĵ����ڵ����������
    if (m_index.find(std::make_pair(x, y)) == m_index.end())
    {
        // ���ĵ����ڵ����鲻���ڣ��ͳ��Ի�ȡ�ĸ��������ڵ�����
        // ��������
        std::vector<cv::Point> points;
        points.push_back(rect.tl());
        points.push_back(rect.tl() + cv::Point(rect.width, 0));
        points.push_back(rect.tl() + cv::Point(0, rect.height));
        points.push_back(rect.tl() + cv::Point(rect.width, rect.height));
        // �������ڵ���������
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
        // ����ĸ����㶼���������У��ͷ��ؿ�
        if (indexs.size() == 0)
        {
            return cv::Mat::zeros(rect.size(), CV_8UC3);
        }
        // ����ĸ������ڴ���ĳ�����飬���Ը�����Ϊ���ݹ�
        else
        {
            index = indexs[0];
        }

    }
    else
    {
        index = m_index[std::make_pair(x, y)];
    }
    // ��ʼ����ͼͼƬ
    cv::Mat map = cv::Mat::zeros(rect.size(), CV_8UC3);

    // �������ڹ�ϵ����ȡ����
    // �����ĵ���������Ϊ��㣬���������ұ���
    // �Դ�����
    m_flag = std::vector<bool>(m_map.size(), false);
    // getMap(map, rect, x, y);
    getMap(map, rect, index);
    return map;
}


cv::Mat QuadTree::getMap_1st(const cv::Rect& rect)
{
    // ��ȡ���ĵ�
    cv::Point center = rect.tl() + cv::Point(rect.width / 2, rect.height / 2);
    // ��ȡ���ĵ����ڵ�����
    int x = center.x / 2048;
    int y = center.y / 2048;
    // ��ʼ����ͼͼƬ
    static cv::Mat map = cv::Mat::zeros(rect.size(), CV_8UC3);
    // ���ݷ�Χ����ȡ����
    for (int i = 0; i < m_rect.size(); i++)
    {
        cv::Rect r = rect & m_rect[i];
        if (r.area() > 0)
        {
            // ��ȡ����ڵ�ͼͼƬ�ķ�Χ
            cv::Rect r1 = r - m_rect[i].tl();
            // ��ȡ���������ͼƬ�ķ�Χ
            cv::Rect r2 = r - rect.tl();
            // ��ȡ����ͼƬ
            cv::Mat img = m_map[i];
            // ȡ����
            img(r1).copyTo(map(r2));
        }
    }

    return map;
}

cv::Mat QuadTree::getAllMap()
{
    return getMap(min_rect);
}

void QuadTree::getMap(const cv::Mat& mat, const cv::Rect& rect, int x, int y)
{
    // ��ȡx,y���������
    if (m_index.find(std::make_pair(x, y)) == m_index.end())
    {
        return;
    }
    int index = m_index[std::make_pair(x, y)];
    // ����������Ѿ���������ֱ�ӷ���
    if (m_flag[index])
    {
        return;
    }
    // ȡ����
    cv::Rect r = rect & m_rect[index];
    // ����������Ϊ0��ֱ�ӷ���
    if (r.area() == 0)
    {
        return;
    }
    // ��ȡ����ڵ�ͼͼƬ�ķ�Χ
    cv::Rect r1 = r - m_rect[index].tl();
    // ��ȡ���������ͼƬ�ķ�Χ
    cv::Rect r2 = r - rect.tl();
    // // ��ȡ����ͼƬ
    // cv::Mat img = m_map[index];
    // // ȡ����
    // img(r1).copyTo(mat(r2));
    // 
    //m_map[index](r1).copyTo(mat(r2));

    mat(r2) = m_map[index](r1);

    // #define SHOW
#ifdef SHOW
    cv::imshow("map", mat);
    cv::waitKey(100);
#endif
    // ��Ǹ������Ѿ�������
    m_flag[index] = true;
    // ��ȡ��������
    Adjacent adjacent = m_adjacent[index];
    // �ݹ������������
    getMap(mat, rect, x - 1, y);
    getMap(mat, rect, x + 1, y);
    getMap(mat, rect, x, y - 1);
    getMap(mat, rect, x, y + 1);
}

void QuadTree::getMap(const cv::Mat& mat, const cv::Rect& rect, const int id)
{
    // ����������Ѿ���������ֱ�ӷ���
    if (m_flag[id])
    {
        return;
    }
    // ȡ����
    cv::Rect r = rect & m_rect[id];
    // ����������Ϊ0��ֱ�ӷ���
    if (r.area() == 0)
    {
        return;
    }
    // ��ȡ����ڵ�ͼͼƬ�ķ�Χ
    cv::Rect r1 = r - m_rect[id].tl();
    // ��ȡ���������ͼƬ�ķ�Χ
    cv::Rect r2 = r - rect.tl();
    // // ��ȡ����ͼƬ
    // cv::Mat img = m_map[id];
    // // ȡ����
    // img(r1).copyTo(mat(r2));
    m_map[id](r1).copyTo(mat(r2));
    // #define SHOW
#ifdef SHOW
    cv::imshow("map", mat);
    cv::waitKey(100);
#endif
    // ��Ǹ������Ѿ�������
    m_flag[id] = true;
    // ��ȡ��������
    Adjacent adjacent = m_adjacent[id];
    // �ݹ������������
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

void test_quadTree(QuadTree& q, int x, int y, int w, int h)
{
    cv::Rect rect(x, y, w, h);
    cv::Mat map = q.getMap(rect);
    cv::imshow("map", map);
    cv::waitKey(100);
}

void save_quadTree(QuadTree& q)
{
    cv::Mat map = q.getAllMap();
    cv::imwrite("AllMap.png", map);
}

void test_1st()
{
    QuadTree quadTree;

    auto start = std::chrono::steady_clock::now();

    save_quadTree(quadTree);

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "getAllMap: " << diff.count() << " s" << std::endl;

    test_quadTree(quadTree, 0, 0, 100, 100);
    test_quadTree(quadTree, 0, 0, 100, 100);
    test_quadTree(quadTree, -1000, 0, 100, 100);
    test_quadTree(quadTree, -100, 0, 100, 100);
    test_quadTree(quadTree, -100, 0, 200, 100);
    test_quadTree(quadTree, 100, 0, 200, 200);
    test_quadTree(quadTree, 100, 0, 2000, 2000);
    test_quadTree(quadTree, 1000, 0, 2000, 2000);

    cv::waitKey(10);

    //// ��ʱ�Ƚ� getMap �� getMap_1st ��Ч��
    //// ѭ��100��

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
}