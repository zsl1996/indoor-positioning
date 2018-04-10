#include<math.h>
Point origin;
Rect selection;
Point track[36];
Mat frame, trackobject, trackhsv;
int flag = 0;
MatND dstlist;
static void onMouse(int event, int x, int y, int, void*)//����¼�����ѡҪ׷�ٵ����壬������Ŀ������HSV�ռ���H��ֱ��ͼ
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
double calcmp(Rect a) { //������Ŀ��ֱ��ͼ�ľ���
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
    double step = 20; //׷�ٵĲ����������˶��ٶȽϿ죬�����õĴ�һ��
    for (int i = 0; i < 4; i++) { //������Ϊ�������ʵ�����������ѡȡ�����������ĸ�����
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
                if ((selection.x + track[i].x) > 0&&(selection.y + track[i].y)>0&& (selection.br().y + track[i].y)<479&& (selection.br().x + track[i].x)<639) { //��ֹ׷�ٿ�Խ��
                    newtrack = selection + track[i];
                }
                while (last < calcmp(newtrack)) {
                    if ((selection.x + track[i].x) > 0 && (selection.y + track[i].y)>0 && (selection.br().y + track[i].y)<479 && (selection.br().x + track[i].x)<639) {//��ֹ׷�ٿ�Խ��
                        selection = selection + track[i]; //���ȼ���ԭ�ݶ���������
                    }
                    break;
                }

            }
        }

    }

    return 0;
}