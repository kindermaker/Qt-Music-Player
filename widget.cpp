#include "widget.h"
#include "ui_widget.h"
#include <QMouseEvent>

//调用WIN API需要用到的头文件与库
#ifdef Q_OS_WIN
#include <qt_windows.h>
#include <Windowsx.h>
// 编译需要的库
#pragma comment(lib, "user32.lib")
#endif

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    //设置窗口标志为无边框
    this->setWindowFlags(Qt::FramelessWindowHint);
    // 背景透明，防止黑影
    setAttribute(Qt::WA_TranslucentBackground);

// 开启贴靠布局
#ifdef Q_OS_WIN
    HWND hwnd = reinterpret_cast<HWND>(this->winId());
    DWORD style = GetWindowLong(hwnd, GWL_STYLE);
    SetWindowLongPtr(hwnd, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
#endif
}

/*
 * 拖动，缩放实现
*/
bool Widget::nativeEvent(const QByteArray &eventType, void *message, qintptr *result) {
    Q_UNUSED(eventType);
#ifdef Q_OS_WIN
    MSG *param = static_cast<MSG *>(message);
    switch (param->message) {
    case WM_NCCALCSIZE: {
        // 使用window默认布局，不加这一段代码，系统无法正确进行最大化
        *result = 0;
        return true;
    }
    case WM_NCHITTEST:{
        int nX = cursor().pos().x() - x();
        int nY = cursor().pos().y() - y();
        // 如果鼠标位于子控件上，则不进行处理
        if (childAt(nX, nY) != nullptr) {
            return QWidget::nativeEvent(eventType, message, result);
        }
        // 默认情况下，将鼠标视为在标题栏上
        *result = HTCAPTION;
        // 鼠标区域位于窗体边框，进行缩放
        // 左边框
        if ((nX > 0) && (nX < m_nBorderWidth))
            *result = HTLEFT;
        // 右边框
        if (nX > (this->width() - m_nBorderWidth))
            *result = HTRIGHT;
        // 左上边框
        if ((nY > 0) && (nY < m_nBorderWidth))
            *result = HTTOP;
        // 下边框
        if ((nY > this->height() - m_nBorderWidth) && (nY < this->height()))
            *result = HTBOTTOM;
        // 左上角
        if ((nX > 0) && (nX < m_nBorderWidth) && (nY > 0)
            && (nY < m_nBorderWidth))
            *result = HTTOPLEFT;
        // 右上角
        if ((nX > this->width() - m_nBorderWidth) && (nX < this->width())
            && (nY > 0) && (nY < m_nBorderWidth))
            *result = HTTOPRIGHT;
        // 左下角
        if ((nX > 0) && (nX < m_nBorderWidth)
            && (nY > this->height() - m_nBorderWidth) && (nY < this->height()))
            *result = HTBOTTOMLEFT;
        // 右下角
        if ((nX > this->width() - m_nBorderWidth) && (nX < this->width())
            && (nY > this->height() - m_nBorderWidth) && (nY < this->height()))
            *result = HTBOTTOMRIGHT;
        return true;
    }
    }
#endif

    return QWidget::nativeEvent(eventType, message, result);
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    // 窗口移动
#ifdef Q_OS_WIN
    if (ReleaseCapture()) {
        QWidget *pWindow = this->window();
        if (pWindow->isWindow()) {
            SendMessage(HWND(pWindow->winId()), WM_SYSCOMMAND, SC_MOVE + HTCAPTION, 0);
        }
    }
#endif
    QWidget::mousePressEvent(event);
}

void Widget::mouseDoubleClickEvent(QMouseEvent *event)
{
    // 双击放大或者恢复
#ifdef Q_OS_WIN
    if (event->button() == Qt::LeftButton) {
        if (ReleaseCapture()) {
            QWidget *pWindow = this->window();
            if (pWindow->isWindow() && !isMaximize) {
                isMaximize = true;
                SendMessage(HWND(pWindow->winId()), WM_SYSCOMMAND, SC_MAXIMIZE, 0);
            } else {
                // 回到最大化之前的位置
                isMaximize = false;
                SendMessage(HWND(pWindow->winId()), WM_SYSCOMMAND, SC_RESTORE, 0);
            }
        }
    }
#endif
    QWidget::mouseDoubleClickEvent(event);
};

Widget::~Widget()
{
    delete ui;
}
