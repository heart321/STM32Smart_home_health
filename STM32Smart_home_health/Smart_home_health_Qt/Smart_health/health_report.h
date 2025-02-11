/*
 * @file      health_report.h
 *
 * @brief     主要实现AI健康报告 调用百度AI大模型
 *
 * @version   V1.0
 *
 * @date     2025/2/11 星期二
 *
 * @auther   chenxuan_123
 */
#ifndef HEALTH_REPORT_H
#define HEALTH_REPORT_H

#include <QWidget>

#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QAudioOutput>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include <QFile>

#include <QUrl>
#include <QUrlQuery>
#include <QBuffer>


//百度语音获取token_url psot_url AppID APIkey Secret_Key
#define Audio_token_url "https://aip.baidubce.com/oauth/2.0/token?grant_type=client_credentials&client_id=%1&client_secret=%2"
#define Audio_post_url "http://vop.baidu.com/server_api"
#define Audio_AppID "116906393"
#define Audio_API_Key "mNTamLIB3G7KBuyDJBjjULJN"
#define Audio_Secret_Key "6YSoSy74kdEkzxT32qBqEBmAPVmPfPoa"

//百度Chat获取token_url psot_url AppID APIkey Secret_Key
#define Chat_token_url "https://aip.baidubce.com/oauth/2.0/token?client_id=%1&client_secret=%2&grant_type=client_credentials"
#define Chat_post_url "https://aip.baidubce.com/rpc/2.0/ai_custom/v1/wenxinworkshop/chat/ernie_speed?access_token=%1"
#define Chat_AppID "116906464"
#define Chat_API_Key "SJmTWOwHO9iFS17FnLz91knb"
#define Chat_Secret_key "UWaaYoPx8RvUovhaENZWB0Y7fbZ76nYh"

namespace Ui {
class health_report;
}

class health_report : public QWidget
{
    Q_OBJECT

public:
    explicit health_report(QWidget *parent = nullptr);
    ~health_report();

    /*获取百度chat 和 语音识别的 token*/
    void baidu_http_get_token(void);

private:
    Ui::health_report *ui;
};

#endif // HEALTH_REPORT_H
