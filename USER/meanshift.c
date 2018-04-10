#include<math.h>
Point origin;
Rect selection;
Point track[36];
Mat frame, trackobject, trackhsv;
int flag = 0;
MatND dstlist;
static void onMouse(int event, int x, int y, int, void*)//鼠标事件，框选要追踪的物体，并计算目标物体HSV空间中H的直方图
{
    switch (event)
    {
    case EVENT_LBUTTONDOWN:
        origin = Point(x, y);
        selection = Rect(x, y, 0, 0);
        break;
    case EVENT_LBUTTONUP:
        selection.x = MIN(x, origin.x);
        selection.y = MIN(y, origin.y);
        selection.width = std::abs(x - origin.x);
        selection.height = std::abs(y - origin.y);
        trackobject = frame(selection).clone();
        cvtColor(trackobject, trackhsv, CV_RGB2HSV);
        int  hsvnum = 30;
        float hrange[] = { 0,179 };
        const float *range[] = { hrange };
        int channels = 0;
        int size = 256; //?
        calcHist(&trackhsv, 1, &channels, Mat(), dstlist, 1, &size, range);
        flag = 1;
        break;
    }
}
int drawrect(Mat &img, Rect t) { 
    rectangle(img, t.tl(), t.br(), Scalar(0));
    return 1;
}
double calcmp(Rect a) { //计算与目标直方图的距离
    Mat select = frame(a);
    Mat newtrack = select.clone();
    Mat newtrackhsv;
    MatND newlist;
    cvtColor(newtrack, newtrackhsv, RGB2HSV);
    int  hsvnum = 30;
    float hrange[] = { 0,179 };
    const float *range[] = { hrange };
    int channels = 0;
    int size = 256; 
    calcHist(newtrackhsv, 1, channels, Mat(), newlist, 1, &size, range);
    return compareHist(dstlist, newlist, CV_COMP_CORREL);
}

int main()
{
    Rect newtrack;
    VideoCapture cap;
    double step = 20; //追踪的步长，假如运动速度较快，就设置的大一点
    for (int i = 0; i < 4; i++) { //邻域方向，为计算速率的提升，仅仅选取了上下左右四个方向
        track[i].x = cos(i*1.0 / 2 * 3 .14 )*step;
        track[i].y = sin(i*1.0 / 2 * 3.14)*step;
    }
    if (!cap.isOpened())
    {
        return -1;
    }
    while (1) {
 
        drawrect(frame, selection);
        imshow("meanshift", frame);

        if (flag == 1) {
            double last = calcmp(selection);
            for (int i = 0; i < 4; i++) {
                if ((selection.x + track[i].x) > 0&&(selection.y + track[i].y)>0&& (selection.br().y + track[i].y)<479&& (selection.br().x + track[i].x)<639) { //防止追踪框越界
                    newtrack = selection + track[i];
                }
                while (last < calcmp(newtrack)) {
                    if ((selection.x + track[i].x) > 0 && (selection.y + track[i].y)>0 && (selection.br().y + track[i].y)<479 && (selection.br().x + track[i].x)<639) {//防止追踪框越界
                        selection = selection + track[i]; //优先计算原梯度上升方向。
                    }
                    break;
                }

            }
        }

    }

    return 0;
}