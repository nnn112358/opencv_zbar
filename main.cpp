#include <iomanip>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <zbar.h>

using namespace std;
using namespace cv;
using namespace zbar;

int main(int argc, char **argv) {
    int cam_idx = 0;

    if(argc == 2) {
        cam_idx = atoi(argv[1]);
    }

    VideoCapture cap(cam_idx);
    if(!cap.isOpened()) {
        cerr << "Could not open camera." << endl;
        exit(EXIT_FAILURE);
    }
    if(!cap.set(cv::CAP_PROP_FRAME_WIDTH, 640))
        cout << "camera set width error" << endl;
    if(!cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480))
        cout << "camera set height error" << endl;

    //  zbar scannerを生成
    ImageScanner scanner;

    // zbarのconfig設定
    scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);

    while(cap.isOpened()) {
        // 撮像してグレースケール変換
        cv::Mat frame, frame_gray;
        cap >> frame;
        cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);

        // 画像をcv:matからImageクラスへ変換
        Image image(frame_gray.cols, frame_gray.rows, "Y800", (uchar *)(frame_gray.data), frame_gray.cols * frame_gray.rows);

        // 画像をzbarへ入力
        scanner.scan(image);

        int counter = 0;
        for(Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol) {
            // 読み取り結果をコンソールに表示
            cout << counter << " decoded " << symbol->get_type_name() << " symbol \"" << symbol->get_data() << '"' << endl;
            cout << "Location: (" << symbol->get_location_x(0) << "," << symbol->get_location_y(0) << ")" << endl;
            cout << "Size: " << symbol->get_location_size() << endl;

            // QRコードの位置を描画する
            if(symbol->get_location_size() == 4) {
                // rectangle(frame, Rect(symbol->get_location_x(i), symbol->get_location_y(i), 10, 10), Scalar(255, 128, 0));
                line(frame, Point(symbol->get_location_x(0), symbol->get_location_y(0)), Point(symbol->get_location_x(1), symbol->get_location_y(1)), Scalar(255, 128, 0), 2, 8, 0);
                line(frame, Point(symbol->get_location_x(1), symbol->get_location_y(1)), Point(symbol->get_location_x(2), symbol->get_location_y(2)), Scalar(255, 128, 0), 2, 8, 0);
                line(frame, Point(symbol->get_location_x(2), symbol->get_location_y(2)), Point(symbol->get_location_x(3), symbol->get_location_y(3)), Scalar(255, 128, 0), 2, 8, 0);
                line(frame, Point(symbol->get_location_x(3), symbol->get_location_y(3)), Point(symbol->get_location_x(0), symbol->get_location_y(0)), Scalar(255, 128, 0), 2, 8, 0);
            }

            counter++;
        }
        image.set_data(NULL, 0);
        
        #ifdef __x86_64__
  	imshow("Results", frame);
        waitKey(30);
        #endif
    }

    return 0;
}
