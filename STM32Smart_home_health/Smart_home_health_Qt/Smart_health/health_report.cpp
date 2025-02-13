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

    /*获取可用的音频输入设备*/
    QList<QAudioDeviceInfo> inputDevices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    /*将获取到的麦克风添加到comboBox_input*/
    foreach (const QAudioDeviceInfo &deviceInfo, inputDevices) {
        ui->comboBox_input->addItem(deviceInfo.deviceName(),QVariant::fromValue(deviceInfo));
    }

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
    /*获取状态码*/
    int state = Reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    /*读取所有返回的数据*/
    QByteArray Reply_data = Reply->readAll();

    /*200 成功*/
    if(state == 200)
    {
        // 获取响应信息
        QByteArray byteArray = QString(Reply_data).toUtf8();
        QJsonDocument jsonDocument = QJsonDocument::fromJson(byteArray); // 转为JSON格式

        if(jsonDocument.isObject() == true)
        {
            QJsonObject jsonObject = jsonDocument.object();
            // 找到token
            if(jsonObject.find("access_token") != jsonObject.end())
            {
                // 语音识别Audio_access的token
                if(Reply->manager() == baidu_Audio)
                {
                    Audio_access_token = jsonDocument["access_token"].toString();
                    qDebug() << "Audio_access_token:" << Audio_access_token << Qt::endl;

                }
                // Chat的token
                if(Reply->manager() == baidu_Chat)
                {
                    Chat_access_token = jsonDocument["access_token"].toString();
                    qDebug() << "Chat_access_token:" << Chat_access_token << Qt::endl;
                }

            }
            if(jsonObject.find("result") != jsonObject.end())
            {
                if(Reply->manager() == baidu_Audio)
                {
                    QJsonArray jsonArray = jsonDocument["result"].toArray();
                    QString result_Audio = jsonArray[0].toString();

                    //ui->textEdit_Audio->append(result_Audio);
                    qDebug() << "reslut_Audio" << result_Audio << Qt::endl;

                    // 发送Chat POST
                    result_Audio = result_Audio.trimmed();
                    baidu_Chat_Send(result_Audio);

                }
                if(Reply->manager() == baidu_Chat)
                {
                    QString result_Chat = jsonObject["result"].toString();

                    qDebug() << "reslut_chat" << result_Chat << Qt::endl;
                    //ui->textEdit_Chat->append(result_Chat);
                    ui->textBrowser_AiReport->setText(result_Chat);
                    // 将Chat得到的文本进行语音合成
                    baidu_AudioOut_Send(result_Chat);

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
        Audio_Format.setCodec("audio/pcm");
        Audio_Format.setByteOrder(QAudioFormat::LittleEndian);
        Audio_Format.setSampleType(QAudioFormat::UnSignedInt);

        // **获取选中的设备**
        QVariant selectedDevice = ui->comboBox_input->currentData();
        if (!selectedDevice.isValid()) {
            qWarning() << "No input device selected!";
            return;
        }

        QAudioDeviceInfo info = selectedDevice.value<QAudioDeviceInfo>();

        // **检查格式支持**
        if (!info.isFormatSupported(Audio_Format)) {
            qWarning() << "Default format not supported, trying to use nearest...";
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

    // **上传音频文件进行识别**
    baidu_Audio_Send();
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
    jsonObject["format"] = "pcm";
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
void health_report::baidu_Chat_Send(QString result)
{
    // Chat POST url
    QString Chat_Post_url = QString(Chat_post_url).arg(Chat_access_token);

    //组装JSON
    QJsonObject json;

    //创建一个QJSonArray用来存放message
    QJsonArray messageArray;
    // 添加对象
    QJsonObject messageObject;
    messageObject.insert("role",QJsonValue("user"));
    messageObject.insert("content",QJsonValue(result));

    //将该对象添加到数组中
    messageArray.append(messageObject);

    //将message数组放入JsonObject
    json.insert("messages",messageArray);
    //json.insert("stream",true);

    //设置头部
    QNetworkRequest Request(Chat_Post_url);
    Request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");

    QString send_json = QJsonDocument(json).toJson();
    QByteArray jsonByte = send_json.toUtf8(); // 转为UTF-8
    baidu_Chat->post(Request,jsonByte);
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

