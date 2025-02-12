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
#include <QJsonObject>
#include <QJsonArray>

#include <QFile>

#include <QUrl>
#include <QUrlQuery>
#include <QBuffer>
#include <QTimer>


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

/*录音文件地址*/
#define File_PATH "D:\\All_Project\\Graduation_project\\STM32Smart_home_health\\Smart_home_health_Qt\\Smart_health\\Audio\\audio_file.pcm"

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

public slots:
    /*请求结束 返回结果解析*/
    void http_finished(QNetworkReply *Reply);

    /*将音频上传到百度云识别*/
    void baidu_Audio_Send(void);

    /*将语音识别完成的文字发送给Chat*/
    void baidu_Chat_Send(QString result);

    /*将百度Chat的回答上传 进行语音合成*/
    void baidu_AudioOut_Send(QString text);

    /*暂停录音*/
    void on_autoStopAudio(void);

private slots:
    void on_pushButton_AiChat_clicked();

    void on_pushButton_stopChat_clicked();

private:
    Ui::health_report *ui;

    // get请求返回的token
    QString Audio_access_token = "";
    QString Chat_access_token = "";

    QNetworkAccessManager* baidu_Audio = nullptr;
    QNetworkAccessManager* baidu_Chat = nullptr;

    // 音频格式
    QAudioFormat Audio_Format;
    QAudioInput *Audio_in = nullptr;

    // 音频文件处理
    QFile destinationFile;
    QFile *sendFile;

    // 结束聊天的标志变量
    bool shouldStopChat = false;
};

#endif // HEALTH_REPORT_H
