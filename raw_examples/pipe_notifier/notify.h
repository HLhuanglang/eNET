#ifndef __EASYNET_NOTIFY_H
#define __EASYNET_NOTIFY_H

class notifier {
 public:
    notifier();
    void handle_read() const; // 通常情况下是封装好，在事件循环中使用

    int get_reciver() const { return m_reciver; }

 public:
    void wakeup() const;

 private:
    int m_notifier; // pipe[1]
    int m_reciver;  // pipe[0]
};

#endif // !__EASYNET_NOTIFY_H
