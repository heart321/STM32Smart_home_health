/*
 * @file      health_report.c
 *
 * @brief     主要实现AI健康报告 调用百度AI大模型
 *
 * @version   V1.0
 *
 * @date     2025/2/11 星期二
 *
 * @auther   chenxuan_123
 */

#include "health_report.h"
#include "ui_health_report.h"

health_report::health_report(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::health_report)
{
    ui->setupUi(this);


    ui->pushButton_AiChat->setBackgroundColor("#55557f");
    ui->pushButton_stopChat->setBackgroundColor("#55557f");
    ui->pushButton_AiReport->setBackgroundColor("#55557f");

    /*获取token*/
    baidu_http_get_token();
}

health_report::~health_report()
{
    delete ui;
}

/*获取语音识别和Chat的token*/
void health_report::baidu_http_get_token()
{
    baidu_Audio = new QNetworkAccessManager(this);
    baidu_Chat = new QNetworkAccessManager(this);

    /*连接http_finished*/
    connect(baidu_Audio,SIGNAL(finished(QNetworkReply*)),this,SLOT(http_finished(QNetworkReply*)));
    connect(baidu_Chat,SIGNAL(finished(QNetworkReply*)),this,SLOT(http_finished(QNetworkReply*)));

    /*语音识别 和 Chat的URL*/
    QString Audio_url = QString(Audio_token_url).arg(Audio_API_Key,Audio_Secret_Key);
    QString Chat_url = QString(Chat_token_url).arg(Chat_API_Key,Chat_Secret_key);


    /*发送get 请求获取 token*/
    baidu_Audio->get(QNetworkRequest(QUrl(Audio_url)));
    baidu_Chat->get(QNetworkRequest(QUrl(Chat_url)));


}


void health_report::http_finished(QNetworkReply *Reply)
{
    int state = Reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QByteArray Reply_data = Reply->readAll();

    if (state == 200)
    {
        QByteArray byteArray = QString(Reply_data).toUtf8();
        QJsonDocument jsonDocument = QJsonDocument::fromJson(byteArray);

        if (jsonDocument.isObject())
        {
            QJsonObject jsonObject = jsonDocument.object();

            if (jsonObject.find("access_token") != jsonObject.end())
            {
                if (Reply->manager() == baidu_Audio)
                {
                    Audio_access_token = jsonDocument["access_token"].toString();
                    qDebug() << "Audio_access_token:" << Audio_access_token << Qt::endl;
                }
                if (Reply->manager() == baidu_Chat)
                {
                    Chat_access_token = jsonDocument["access_token"].toString();
                    qDebug() << "Chat_access_token:" << Chat_access_token << Qt::endl;
                }
            }
            if (jsonObject.find("result") != jsonObject.end())
            {
                if (Reply->manager() == baidu_Audio)
                {
                    QJsonArray jsonArray = jsonDocument["result"].toArray();
                    QString result_Audio = jsonArray[0].toString();
                    qDebug() << "result_Audio" << result_Audio << Qt::endl;
                    result_Audio = result_Audio.trimmed();
                    baidu_Chat_Send(result_Audio); // 默认是聊天请求
                }
                if (Reply->manager() == baidu_Chat)
                {
                    QString result_Chat = jsonObject["result"].toString();
                    qDebug() << "result_chat" << result_Chat << Qt::endl;

                    // 获取请求类型
                    RequestType type = Reply->request().attribute(QNetworkRequest::User).value<RequestType>();

                    if (type == ChatRequest)
                    {
                        // 普通聊天结果显示在textBrowser_AiChat
                        ui->textBrowser_AiChat->setText(result_Chat);
                        baidu_AudioOut_Send(result_Chat);
                    }
                    else if (type == HealthReportRequest)
                    {
                        // 健康报告结果显示在textBrowser_AiReport
                        ui->textBrowser_AiReport->setText(result_Chat);
                    }
                }
            }
        }
    }
}

/* 点击AI 聊天开始录音 */
void health_report::on_pushButton_AiChat_clicked()
{
    shouldStopChat = false;

    if (ui->pushButton_AiChat->text() == QStringLiteral("开始聊天"))
    {
        // **确保文件已关闭**
        if (destinationFile.isOpen()) {
            destinationFile.close();
        }

        // **设置录音格式**
        Audio_Format.setSampleRate(16000);
        Audio_Format.setChannelCount(1);
        Audio_Format.setSampleSize(16);
        Audio_Format.setCodec("audio/wav");
        Audio_Format.setByteOrder(QAudioFormat::LittleEndian);
        Audio_Format.setSampleType(QAudioFormat::UnSignedInt);


        //选择默认设备为输入源
        QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();

        //判断输入的格式是否支持，如果不支持就使用系统支持的默认格式
        if(!info.isFormatSupported(Audio_Format))
        {
            Audio_Format = info.nearestFormat(Audio_Format);
        }


        // **释放旧的 QAudioInput**
        if (Audio_in) {
            delete Audio_in;
            Audio_in = nullptr;
        }

        // **创建新的 QAudioInput**
        Audio_in = new QAudioInput(info, Audio_Format, this);

        // **打开音频文件**
        destinationFile.setFileName(File_PATH);
        if (!destinationFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            qWarning() << "Failed to open audio file";
            return;
        }

        // **开始录音**
        Audio_in->start(&destinationFile);
        ui->pushButton_AiChat->setText("请说话....");

        // **5 秒后自动停止录音**
        QTimer::singleShot(5000, this, &health_report::on_autoStopAudio);
    }
}




/* 停止录音 */
void health_report::on_autoStopAudio()
{
    if (Audio_in) {
        Audio_in->stop();
        delete Audio_in;  // **释放音频输入对象**
        Audio_in = nullptr;
    }

    if (destinationFile.isOpen()) {
        destinationFile.close();  // **确保文件关闭**
    }


    ui->pushButton_AiChat->setText("开始聊天");

    // 转换音频格式
    QString filePath = File_PATH;  // 原始文件路径

    // 构建参数列表
    QStringList arguments;
    arguments << "-i" << filePath          // 输入文件
              << "-ar" << "16000"          // 采样率 16kHz
              << "-ac" << "1"              // 单声道
              << "-acodec" << "pcm_s16le"  // 音频编码
              << "-f" << "wav"             // 输出格式
              << filePath                  // 输出文件（覆盖原文件）
              << "-y";                     // 自动覆盖

    QProcess process;
    process.start("D:\\All_Project\\Graduation_project\\STM32Smart_home_health\\Smart_home_health_Qt\\Smart_health\\ffmpeg\\bin\\ffmpeg.exe", arguments);    // 使用新的 start 方法
    if (process.waitForFinished(-1)) {
        // 转换成功后上传
        baidu_Audio_Send();  // 使用原地址上传，因为已经被替换
        qDebug() << "Audio conversion Success!!" << Qt::endl;
    } else {
        // 处理转换失败的情况
        qDebug() << "Audio conversion failed:" << process.errorString();
    }
    // 上传音频文件进行识别
    //baidu_Audio_Send();
}



/*将音频上传百度云识别*/
void health_report::baidu_Audio_Send()
{
    // 读取音频文件
    sendFile = new QFile(File_PATH);
    sendFile->open(QIODevice::ReadWrite);

    // 文件打开识别
    if(sendFile->isOpen() == false)
    {
        qDebug() << "file open false" << Qt::endl;
        delete sendFile;
        return ;
    }

    // 读取文件中的内容
    QByteArray requestData = sendFile->readAll();
    // 读取完成后关闭文件
    sendFile->close();
    // 将文件格式转为base64编码
    QString buf = requestData.toBase64();

    // 组装JSON数据
    QJsonObject jsonObject;
    jsonObject["format"] = "wav";
    jsonObject["rate"] = 16000;
    jsonObject["dev_pid"] = 1537;
    jsonObject["channel"] = 1;
    jsonObject["token"] = Audio_access_token;
    jsonObject["cuid"] = "cx";
    jsonObject["len"] = requestData.size();
    jsonObject["speech"] = buf;

    // 设置POST请求头
    QNetworkRequest Request(QUrl(Audio_post_url));
    Request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");

    // POST请求
    QByteArray send_json = QJsonDocument(jsonObject).toJson();
    baidu_Audio->post(Request,send_json);
}

/*将识别完成的文字发送给Chat*/
void health_report::baidu_Chat_Send(QString result, RequestType type)
{
    // Chat POST url
    QString Chat_Post_url = QString(Chat_post_url).arg(Chat_access_token);

    // 组装JSON
    QJsonObject json;
    QJsonArray messageArray;
    QJsonObject messageObject;
    messageObject.insert("role", QJsonValue("user"));
    messageObject.insert("content", QJsonValue(result));
    messageArray.append(messageObject);
    json.insert("messages", messageArray);

    // 设置头部
    QNetworkRequest Request(Chat_Post_url);
    Request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // 将请求类型存入请求属性，用于返回时区分
    Request.setAttribute(QNetworkRequest::User, QVariant::fromValue(type));

    QString send_json = QJsonDocument(json).toJson();
    QByteArray jsonByte = send_json.toUtf8();
    baidu_Chat->post(Request, jsonByte);
}

void health_report::baidu_AudioOut_Send(QString text)
{
    if (shouldStopChat) {
        return;  // 如果标志为 true，则不再继续录音
    }

    QUrl AudioOut_url("http://tsn.baidu.com/text2audio");

    //组装JSON 音色
    const int per = 4100;
    const int spd = 6;
    const int pit = 5;
    const int vol = 5;
    const int aue = 6;

    QUrlQuery query;
    query.addQueryItem("tok",Audio_access_token);
    query.addQueryItem("tex",text);
    query.addQueryItem("cuid","cx");
    query.addQueryItem("per",QString::number(per));
    query.addQueryItem("spd",QString::number(spd));
    query.addQueryItem("pit",QString::number(pit));
    query.addQueryItem("vol",QString::number(vol));
    query.addQueryItem("aue",QString::number(aue));
    query.addQueryItem("lan","zh");
    query.addQueryItem("ctp","1");

    AudioOut_url.setQuery(query);

    //http请求创建
    QNetworkAccessManager manager;
    QNetworkRequest request(AudioOut_url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");

    //http请求
    QNetworkReply *reply = manager.get(request);
    QEventLoop loop;
    QObject::connect(reply,&QNetworkReply::finished,&loop,&QEventLoop::quit);
    loop.exec();

    // 请求完成
    if(reply->error() == QNetworkReply::NoError)
    {

        QString contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
        qDebug() << "contentType:" << contentType << Qt::endl;


        if(contentType.startsWith("audio"))
        {
            // 获取返回的音频数据流
            QByteArray audioData = reply->readAll();
            //qDebug() << "audioData:" << audioData << Qt::endl;
            QBuffer *buffer = new QBuffer(); // 使用指针以便管理生命周期
            buffer->setData(audioData);
            buffer->open(QIODevice::ReadOnly);

            // 音频输出
            QAudioFormat format;
            format.setSampleRate(16000);
            format.setChannelCount(1);
            format.setSampleSize(16);
            format.setCodec("audio/pcm");
            format.setByteOrder(QAudioFormat::LittleEndian);
            format.setSampleType(QAudioFormat::SignedInt);

            QAudioOutput *audioOutput = new QAudioOutput(format); // 同样使用指针
            audioOutput->start(buffer);

            // 事件循环处理播放状态
            QEventLoop audioLoop;
            QObject::connect(audioOutput, &QAudioOutput::stateChanged, [&](QAudio::State state) {
                if (state == QAudio::IdleState || state == QAudio::StoppedState) {
                    audioLoop.quit();
                }
            });

            audioLoop.exec(); // 等待音频播放完成

            // 播放完成进行录音
            // 清理资源
            audioOutput->stop();
            delete audioOutput;
            buffer->close();
            delete buffer;

            // 播放完成后开始新的录音
            if (!shouldStopChat) {
                on_pushButton_AiChat_clicked();// 触发录音按钮点击事件，开始新的录制
            }
        }
    }
    reply->deleteLater();
}


void health_report::on_pushButton_stopChat_clicked()
{
    shouldStopChat = true;
    ui->pushButton_AiChat->setText("开始聊天");
}

/*生成AI建议报告*/
void health_report::on_pushButton_AiReport_clicked()
{
    // 假设这是你的健康数据（实际数据应从传感器或其他模块获取）
    QString healthData = "心率: 75 bpm, 血氧: 98%, 体温: 36.5°C, 室内PM2.5: 30 μg/m³ 请给出三条建议";
    baidu_Chat_Send(healthData, HealthReportRequest); // 发送健康数据，标记为健康报告请求
}

