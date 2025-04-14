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

health_report::health_report(health_home *home, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::health_report)
    , home(home)
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
    if (shouldStopChat) {
        if (Audio_in) {
            Audio_in->stop();
            QTimer::singleShot(1000, this, SLOT(on_autoStopAudio()));
            ui->pushButton_AiChat->setEnabled(false);
        }
        shouldStopChat = false;
        ui->pushButton_AiChat->setText("开始聊天");
        return;
    }

    QAudioDeviceInfo selectedDevice = QAudioDeviceInfo::defaultInputDevice();
    if (selectedDevice.isNull()) {
        qDebug() << "没有可用的音频输入设备！";
        QMessageBox::critical(this, "错误", "没有检测到麦克风，请检查音频设备！");
        return;
    }

  //  qDebug() << "默认音频输入设备:" << selectedDevice.deviceName();
    QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    for (const QAudioDeviceInfo &device : devices) {
    //    qDebug() << "可用输入设备:" << device.deviceName();
    }

    Audio_Format.setSampleRate(48000);
    Audio_Format.setChannelCount(1);
    Audio_Format.setSampleSize(16);
    Audio_Format.setCodec("audio/pcm");
    Audio_Format.setByteOrder(QAudioFormat::LittleEndian);
    Audio_Format.setSampleType(QAudioFormat::SignedInt);

    if (!selectedDevice.isFormatSupported(Audio_Format)) {
        qDebug() << "默认设备不支持 48000Hz！尝试其他设备...";
        selectedDevice = QAudioDeviceInfo();
        for (const QAudioDeviceInfo &device : devices) {
            if (device.isFormatSupported(Audio_Format)) {
                selectedDevice = device;
                qDebug() << "使用支持的设备:" << device.deviceName();
                break;
            }
        }
        if (selectedDevice.isNull()) {
            qDebug() << "没有设备支持 48000Hz！尝试 44100Hz...";
            Audio_Format.setSampleRate(44100);
            for (const QAudioDeviceInfo &device : devices) {
                if (device.isFormatSupported(Audio_Format)) {
                    selectedDevice = device;
                    qDebug() << "使用支持的设备 (44100Hz):" << device.deviceName();
                    break;
                }
            }
        }
        if (selectedDevice.isNull()) {
            qDebug() << "没有设备支持音频格式！";
            QMessageBox::critical(this, "错误", "当前音频格式（48kHz/44.1kHz，16-bit）不受支持！");
            return;
        }
    }

    // 确保目录存在
    QDir().mkpath(QFileInfo(File_PATH).absolutePath());
    destinationFile.setFileName(File_PATH);
    if (!destinationFile.open(QIODevice::WriteOnly)) {
        qDebug() << "无法打开音频文件:" << File_PATH << ", 错误:" << destinationFile.errorString();
        QMessageBox::critical(this, "错误", "无法创建音频文件：" + destinationFile.errorString());
        return;
    }

    writeWavHeader(destinationFile, Audio_Format, 0);

    Audio_in = new QAudioInput(selectedDevice, Audio_Format, this);
    Audio_in->start(&destinationFile);
    qDebug() << "开始录音，采样率:" << Audio_Format.sampleRate() << "Hz，文件:" << File_PATH;
    ui->pushButton_AiChat->setText("停止聊天");
    ui->pushButton_AiChat->setEnabled(true);
    shouldStopChat = true;

    // 延长到 5 秒
    QTimer::singleShot(5000, this, SLOT(on_autoStopAudio()));
}

// 重写wav头文件
void health_report::writeWavHeader(QFile &file, const QAudioFormat &format, qint64 dataLength)
{
    QByteArray header(44, 0);
    int sampleRate = format.sampleRate();
    int channels = format.channelCount();
    int sampleSize = format.sampleSize() / 8;

    // RIFF 头
    header.replace(0, 4, "RIFF");
    qint32 riffSize = dataLength + 36;
    header[4] = static_cast<char>(riffSize & 0xff);
    header[5] = static_cast<char>((riffSize >> 8) & 0xff);
    header[6] = static_cast<char>((riffSize >> 16) & 0xff);
    header[7] = static_cast<char>((riffSize >> 24) & 0xff);
    header.replace(8, 4, "WAVE");

    // fmt 子块
    header.replace(12, 4, "fmt ");
    header[16] = 16; // 子块大小
    header[20] = 1; // PCM = 1
    header[22] = static_cast<char>(channels & 0xff);
    header[23] = static_cast<char>((channels >> 8) & 0xff);
    header[24] = static_cast<char>(sampleRate & 0xff);
    header[25] = static_cast<char>((sampleRate >> 8) & 0xff);
    header[26] = static_cast<char>((sampleRate >> 16) & 0xff);
    header[27] = static_cast<char>((sampleRate >> 24) & 0xff);
    int byteRate = sampleRate * channels * sampleSize;
    header[28] = static_cast<char>(byteRate & 0xff);
    header[29] = static_cast<char>((byteRate >> 8) & 0xff);
    header[30] = static_cast<char>((byteRate >> 16) & 0xff);
    header[31] = static_cast<char>((byteRate >> 24) & 0xff);
    int blockAlign = channels * sampleSize;
    header[32] = static_cast<char>(blockAlign & 0xff);
    header[33] = static_cast<char>((blockAlign >> 8) & 0xff);
    header[34] = static_cast<char>(sampleSize * 8 & 0xff);
    header[35] = static_cast<char>((sampleSize * 8 >> 8) & 0xff);

    // data 子块
    header.replace(36, 4, "data");
    header[40] = static_cast<char>(dataLength & 0xff);
    header[41] = static_cast<char>((dataLength >> 8) & 0xff);
    header[42] = static_cast<char>((dataLength >> 16) & 0xff);
    header[43] = static_cast<char>((dataLength >> 24) & 0xff);

    file.seek(0);
    file.write(header);
}

void health_report::on_autoStopAudio()
{
    if (Audio_in) {
        Audio_in->stop();
        delete Audio_in;
        Audio_in = nullptr;
    }

    if (destinationFile.isOpen()) {
        destinationFile.flush();
        qint64 fileSize = destinationFile.size();
        qint64 dataLength = fileSize > 44 ? fileSize - 44 : 0;
        if (dataLength > 0) {
            writeWavHeader(destinationFile, Audio_Format, dataLength);
        }
        destinationFile.close();
        qDebug() << "录音文件已关闭:" << File_PATH;
    }

    ui->pushButton_AiChat->setText("开始聊天");

    // 转换音频格式
    QString filePath = File_PATH;

    // 检查音频文件是否存在
    QFile audioFile(filePath);
    if (!audioFile.exists()) {
        qDebug() << "音频文件不存在:" << filePath;
        QMessageBox::warning(this, "错误", "录制的音频文件不存在！");
        return;
    }

    qint64 fileSize = audioFile.size();
    qDebug() << "录音文件大小:" << fileSize << "字节";
    if (fileSize <= 44) {
        qDebug() << "录音文件为空，可能录音失败！";
        QMessageBox::warning(this, "错误", "录音文件为空，请检查麦克风！");
        return;
    }


// 指定 FFmpeg 路径
#ifdef Q_OS_WIN
    // Windows 路径
    QString ffmpegPath = "D://Qt//ffmpeg//bin//ffmpeg.exe";
#else
    // Linux（Ubuntu）路径，优先使用系统安装的 ffmpeg
    QString ffmpegPath = "ffmpeg"; // 使用 PATH 中的 ffmpeg
    // QString ffmpegPath = "/usr/bin/ffmpeg";
#endif

    // 检查 FFmpeg 是否存在
    QFile ffmpegFile(ffmpegPath);
    if (ffmpegPath != "ffmpeg" && !ffmpegFile.exists()) {
        qDebug() << "FFmpeg 可执行文件不存在:" << ffmpegPath;
        QMessageBox::critical(this, "错误", "无法找到 FFmpeg，请确保已安装！");
        return;
    }



    // 构建 FFmpeg 参数
    QStringList arguments;
    arguments << "-y"                  // 覆盖输出文件
              << "-i" << filePath     // 输入文件
              << "-ar" << "16000"     // 采样率
              << "-ac" << "1"         // 单声道
              << "-acodec" << "pcm_s16le" // 编码
              << outputFilePath;      // 输出文件


    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start(ffmpegPath, arguments);
    if (process.waitForFinished(3000)) {
        QString ffmpegOutput = process.readAll();
//        qDebug() << "FFmpeg 输出:" << ffmpegOutput;
        if (process.exitCode() == 0) {
            QFile outputFile(outputFilePath);
            qint64 convertedSize = outputFile.size();
            qDebug() << "Audio conversion Success!!";
            qDebug() << "转换后文件大小:" << convertedSize << "字节";
            if (convertedSize <= 44) {
                qDebug() << "转换后文件为空！";
                QMessageBox::warning(this, "错误", "音频转换生成了空文件！");
                return;
            }
            QFile::remove(filePath);
            QFile::rename(outputFilePath, filePath);
            baidu_Audio_Send();
        } else {
            qDebug() << "Audio conversion failed with exit code:" << process.exitCode();
            QMessageBox::warning(this, "错误", "音频转换失败，退出码：" + QString::number(process.exitCode()));
        }
    } else {
        qDebug() << "FFmpeg 进程失败:" << process.errorString();
        QMessageBox::warning(this, "错误", "无法运行 FFmpeg：" + process.errorString());
    }
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
    if (!shouldStopChat) {
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
    // 从 health_home 获取最新的 MQTT 数据
    QString healthData = QString("心率: %1 bpm, 血氧: %2%, 体温: %3°C, 室内PM2.5: %4 μg/m³ 请给出三条建议")
                             .arg(home->getHeartRate())
                             .arg(home->getBloodOxygen())
                             .arg(home->getPeopleTemp())
                             .arg(home->getPM25());
    baidu_Chat_Send(healthData, HealthReportRequest); // 发送健康数据
}

