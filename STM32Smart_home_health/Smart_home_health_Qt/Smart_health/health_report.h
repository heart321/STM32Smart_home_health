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
#include <QStandardPaths>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QFile>

#include <QUrl>
#include <QUrlQuery>
#include <QBuffer>
#include <QTimer>
#include <QString>

#include <QProcess>

#include "health_home.h"

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
    explicit health_report(health_home *home,QWidget *parent = nullptr);
    ~health_report();


    // 请求类型枚举
    enum RequestType {
        ChatRequest,      // 普通聊天请求
        HealthReportRequest // 健康报告请求
    };
    /*获取百度chat 和 语音识别的 token*/
    void baidu_http_get_token(void);

    void writeWavHeader(QFile &file, const QAudioFormat &format, qint64 dataLength);

public slots:
    /*请求结束 返回结果解析*/
    void http_finished(QNetworkReply *Reply);

    /*将音频上传到百度云识别*/
    void baidu_Audio_Send(void);

    /*将语音识别完成的文字发送给Chat*/
    void baidu_Chat_Send(QString result,RequestType type = ChatRequest);

    /*将百度Chat的回答上传 进行语音合成*/
    void baidu_AudioOut_Send(QString text);

    /*暂停录音*/
    void on_autoStopAudio(void);

    /*开始对话按钮*/
    void on_pushButton_AiChat_clicked();
    /*停止对话按钮*/
    void on_pushButton_stopChat_clicked();
    /*健康建议按钮*/
    void on_pushButton_AiReport_clicked();

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

    /*home界面*/
    health_home *home;

    /*输出文件地址*/
    QString File_PATH = "/home/xchen/Desktop/Qt_Project/Smart_health/Audio/audio_file.wav";
    QString outputFilePath = File_PATH + ".converted.wav";
};

Q_DECLARE_METATYPE(health_report::RequestType)

#endif // HEALTH_REPORT_H
