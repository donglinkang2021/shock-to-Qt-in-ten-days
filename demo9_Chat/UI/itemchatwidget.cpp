#include "itemchatwidget.h"
#include "ui_itemchatwidget.h"

ItemChatWidget::ItemChatWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ItemChatWidget)
{
    ui->setupUi(this);
}

ItemChatWidget::~ItemChatWidget()
{
    delete ui;
}

int ItemChatWidget::SetMyData(QString username, QString picon, QString txt)
{
    //头像
    QToolButton* imgbtn = new QToolButton(this);
    imgbtn->setIcon(QIcon(picon));
    imgbtn->setIconSize(QSize(40,40));
    imgbtn->setGeometry(500,10,40,40);
    imgbtn->setStyleSheet("QToolButton:{border:none;}");
    imgbtn->show();

    QFontMetrics fm(this->font());

    //id
    QLabel* idlab = new QLabel(this);
    // 创建一个新的QFont对象，并设置新的字体大小
    QFont newFont = idlab->font();
    newFont.setPointSize(5);
    // 将新的字体应用到idlab
    idlab->setFont(newFont);
    idlab->setText(username);
    idlab->setGeometry(
        485-fm.horizontalAdvance(username), // ax
        10,                                            // ay
        fm.horizontalAdvance(username),     // aw
        fm.height()                                    // ah
    );

    //文本
    QLabel* txtlab = new QLabel(this);
    txtlab->setStyleSheet(
        "QLabel{background-color:"+color+";\
          border:5px solid "+color+";\
          border-top-left-radius:15px;\
          border-bottom-left-radius:15px;\
          border-bottom-right-radius:15px;}"
    );

    int txtheight = fm.height();
    countLine = 0;
    longestWidth = 0;
    txt = Splite(txt,fm);
//    qDebug()<<countLine;
    txtlab->setText(txt);
    txtlab->setGeometry(
        490-longestWidth-30,
        30,
        longestWidth+30,
        txtheight*countLine+20
    );
    txtlab->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
    txtlab->setWordWrap(true);

    // 显示时间
    QDateTime datetime = QDateTime::currentDateTime();
    QString str = datetime.toString("hh:mm:ss");
//    qDebug()<<str;
    QLabel* timelab = new QLabel(this);
    timelab->setStyleSheet(
        "font-size:10px;\
         color:rgb(255,0,0)"
    );

    timelab->setText(str);
    timelab->setGeometry(
        530-fm.horizontalAdvance(str),
        50+txtheight*countLine,
        fm.horizontalAdvance(str),
        fm.height()
    );

//    return 30+10+txtheight*countLine+40;
    return 50+txtheight*countLine+fm.height()+10;
}

int ItemChatWidget::SetFriendsData(QString username, QString picon, QString txt)
{
    //头像
    QToolButton* imgbtn = new QToolButton(this);
    imgbtn->setIcon(QIcon(picon));
    imgbtn->setIconSize(QSize(40,40));
    imgbtn->setGeometry(10,10,40,40);
    imgbtn->setStyleSheet("QToolButton:{border:none;}");
    imgbtn->show();

    QFontMetrics fm(this->font());

    //id
    QLabel* idlab = new QLabel(this);
    // 创建一个新的QFont对象，并设置新的字体大小
    QFont newFont = idlab->font();
    newFont.setPointSize(5);
    // 将新的字体应用到idlab
    idlab->setFont(newFont);
    idlab->setText(username);
    idlab->setGeometry(70,10,fm.horizontalAdvance(username)+20,fm.height());

    //文本
    QLabel* txtlab = new QLabel(this);
    txtlab->setStyleSheet("QLabel{background-color:rgb(255,255,255);border:2px solid "+color+";\
                            border-top-right-radius:15px;\
                            border-bottom-left-radius:15px;\
                            border-bottom-right-radius:15px}");
    int txtheight = fm.height();
    countLine = 0;
    longestWidth = 0;
    countLine = 0;
    txt = Splite(txt,fm);
    txtlab->setText(txt);
    txtlab->setGeometry(70, 30, longestWidth+30, txtheight*countLine+35);
    txtlab->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
    txtlab->setWordWrap(true);
    return 30+10+txtheight*countLine+35;
}

int ItemChatWidget::SetFriendsImg(QString imgname, QString picon ,QString imgpth)
{
    //头像
    QToolButton* imgbtn = new QToolButton(this);
    imgbtn->setIcon(QIcon(picon));
    imgbtn->setIconSize(QSize(40,40));
    imgbtn->setGeometry(10,10,40,40);
    imgbtn->setStyleSheet("QToolButton:{border:none;}");
    imgbtn->show();

    QFontMetrics fm(this->font());

    //id
    QLabel* idlab = new QLabel(this);
    idlab->setText(imgname);
    idlab->setGeometry(70,10,fm.horizontalAdvance(imgname)+20,fm.height());

    //图片
    QToolButton* img = new QToolButton(this);
    img->setStyleSheet("QToolButton{border:none}");
    img->setToolButtonStyle(Qt::ToolButtonIconOnly);
    img->setIcon(QIcon(imgpth));
    QImage* pimg = new QImage(imgpth);
    int width = 150;
    int height = 150*pimg->height()/pimg->width();
    img->setIconSize(QSize(width,height));
    img->setGeometry(70, 32, width+30, height+35);
    //connect(img,SIGNAL(clicked()),this,SLOT(downloadImg()));//点击图片可以下载图片
    return 30+10+height+35;
}

void ItemChatWidget::setColor(QString c)
{
    color = c;
}

QString ItemChatWidget::Splite(QString txt, QFontMetrics& font)
{
    // 换行
    int txtwidth = txt.size(); //当前字符串长度
    //    qDebug()<<txt.indexOf("\n");
    if(txt.indexOf("\n") != -1)
    {
        QString qstrLeftData = txt.left(txt.indexOf("\n"));
        //        qDebug()<<qstrLeftData;
        QString qstrMidData = txt.mid(txt.indexOf("\n")+1);
        //        qDebug()<<qstrMidData;
        return Splite(qstrLeftData, font) + "\n" + Splite(qstrMidData, font);
    }
    int txtwidth2 = font.horizontalAdvance(txt); //当前字符串所占像素长度
    //qDebug()<<txtwidth;
    //    qDebug()<<txtwidth2;
    if(txtwidth2<=200)  //不需要继续划分
    {
        countLine++;
        if(txtwidth2>longestWidth)
            longestWidth = txtwidth2;
        //        qDebug()<<longestWidth;
        return txt;
    }
    else
    {
        countLine++;
        longestWidth = 200;
        QString qstrLeftData = txt.left(txtwidth*200/txtwidth2);
        //        qDebug()<<qstrLeftData;
        QString qstrMidData = txt.mid(txtwidth*200/txtwidth2);
        //        qDebug()<<qstrMidData;
        return qstrLeftData + "\n" + Splite(qstrMidData, font);
    }
}

int ItemChatWidget::SetMyImg(QString imgname,QString picon ,QString imgpth)
{
    //头像
    QToolButton* imgbtn = new QToolButton(this);
    imgbtn->setIcon(QIcon(picon));
    imgbtn->setIconSize(QSize(40,40));
    imgbtn->setGeometry(500,10,40,40);
    imgbtn->setStyleSheet("QToolButton:{border:none;}");
    imgbtn->show();

    QFontMetrics fm(this->font());

    //id
    QLabel* idlab = new QLabel(this);
    idlab->setText(imgname);
    idlab->setGeometry(
        485-fm.horizontalAdvance(imgname),
        10,
        fm.horizontalAdvance(imgname),
        fm.height()+4
    );

    //图片
    QToolButton* img = new QToolButton(this);
    img->setStyleSheet("QToolButton{border:none}");
    img->setToolButtonStyle(Qt::ToolButtonIconOnly);
    img->setIcon(QIcon(imgpth));
    QImage* pimg = new QImage(imgpth);
    int width = 150;
    int height = 150*pimg->height()/pimg->width();
    img->setIconSize(QSize(width,height));
    img->setGeometry(
        490-width-30, 28,
        width+30, height+30
    );
    //connect(img,SIGNAL(clicked()),this,SLOT(downloadImg()));//点击图片可以下载图片

    QDateTime datetime = QDateTime::currentDateTime();
    QString str = datetime.toString("hh:mm:ss");
    //    qDebug()<<str;
    QLabel* timelab = new QLabel(this);
    timelab->setStyleSheet("font-size:10px;color:rgb(255,0,0)");
    timelab->setText(str);
    timelab->setGeometry(530-fm.horizontalAdvance(str),height+40,
                         fm.horizontalAdvance(str),fm.height());

    return 30+10+height+25;
}

