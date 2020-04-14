#include "edit_page.h"
#include "ui_edit_page.h"
#include <QBitmap>
#include <QPalette>
#include "ui_select_color_page.h"
#include <QDebug>
#include "widget.h"
#include <QPainter>

static int count =0;

Edit_page::Edit_page(Widget* page, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Edit_page)
  , m_editColor(0,0,0)
{
    ui->setupUi(this);
    QBitmap bmp(this->size());
    bmp.fill();
    QPainter p(&bmp);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::black);
    p.setRenderHint(QPainter::Antialiasing);
    p.drawRoundedRect(bmp.rect(),6,6);
    setMask(bmp);
    pNotebook = page;
    setWindowOpacity(0.9);
    setWindowFlags(Qt::FramelessWindowHint);
    set_all_btn_attribute();
    set_select_color_page();
    set_text_editing_page();
    connect(ui->textEdit,&QTextEdit::textChanged,this,&Edit_page::textChangedSlot);
}

Edit_page::~Edit_page()
{
    delete ui;
}

void Edit_page::set_select_color_page()
{
  color_page = new select_color_page();
  ui->textEdit->setFrameShape(QFrame::NoFrame);
  connect(color_page->ui->blue_btn, SIGNAL(clicked()), this, SLOT(blue_btn_change()) );
  connect(color_page->ui->pink_btn, SIGNAL(clicked()), this, SLOT(pink_btn_change()) );
  connect(color_page->ui->dark_green_btn, SIGNAL(clicked()), this, SLOT(dark_green_btn_change()) );
  connect(color_page->ui->orang_btn, SIGNAL(clicked()), this, SLOT(orang_btn_change()) );
  connect(color_page->ui->Violet_btn, SIGNAL(clicked()), this, SLOT(Violet_btn_change()) );
  connect(color_page->ui->Golden_btn, SIGNAL(clicked()), this, SLOT(Golden_btn_change()) );
  connect(color_page->ui->light_blue_btn, SIGNAL(clicked()), this, SLOT(light_blue_btn_change()) );
  connect(color_page->ui->light_green_btn, SIGNAL(clicked()), this, SLOT(light_green_btn_change()) );
  connect(color_page->ui->yellow_btn, SIGNAL(clicked()), this, SLOT(yellow_btn_change()) );
  connect(color_page->ui->wight_btn, SIGNAL(clicked()), this, SLOT(wight_btn_change()) );
}

void Edit_page::set_text_editing_page()
{
    text_edit_page = new Text_editing();
}



void Edit_page::textChangedSlot()
{
    emit texthasChanged();
    qDebug() << "emit textchange";
}

void Edit_page::blue_btn_change()
{

    ui->widget->setStyleSheet("background:rgba(76,119,231,1);\
                        border-radius:4px;");
    m_editColor = QColor(76,119,231);
    emit colorhasChanged(m_editColor);
    qDebug() << "emit colorhasChanged";
}

void Edit_page::pink_btn_change()
{
    ui->widget->setStyleSheet("background:rgba(250,108,99,1);\
                        border-radius:4px;");
    m_editColor = QColor(250,108,99);
    emit colorhasChanged(m_editColor);
}

void Edit_page::dark_green_btn_change()
{
     ui->widget->setStyleSheet("background:rgba(15,161,90,1);\
                         border-radius:4px;");
     m_editColor = QColor(15,161,90);
     emit colorhasChanged(m_editColor);
}

void Edit_page::orang_btn_change()
{
     ui->widget->setStyleSheet("background:rgba(255,151,47,1);\
                         border-radius:4px;");

     m_editColor = QColor(255,151,47);
     emit colorhasChanged(m_editColor);
}

void Edit_page::Violet_btn_change()
{
     ui->widget->setStyleSheet("background:rgba(186,123,216,1);\
                         border-radius:4px;");
     m_editColor = QColor(186,123,216);
     emit colorhasChanged(m_editColor);
}

void Edit_page::Golden_btn_change()
{
     ui->widget->setStyleSheet("background:rgba(248,209,93,1);\
                         border-radius:4px;");
     m_editColor = QColor(248,209,93);
     emit colorhasChanged(m_editColor);
}

void Edit_page::light_blue_btn_change()
{
     ui->widget->setStyleSheet("background:rgba(42,162,217,1);\
                         border-radius:4px;");
     m_editColor = QColor(42,162,217);
     emit colorhasChanged(m_editColor);
}

void Edit_page::light_green_btn_change()
{
     ui->widget->setStyleSheet("background:rgba(110,207,67,1);\
                         border-radius:4px;");
     m_editColor = QColor(110,207,67);
     emit colorhasChanged(m_editColor);
}

void Edit_page::yellow_btn_change()
{
     ui->widget->setStyleSheet("background:rgba(250,243,175,1);\
                         border-radius:4px;");
     m_editColor = QColor(250,243,175);
     emit colorhasChanged(m_editColor);
}

void Edit_page::wight_btn_change()
{
     ui->widget->setStyleSheet("background:rgba(236,238,242,1);\
                         border-radius:4px;");
     m_editColor = QColor(236,238,242);
     emit colorhasChanged(m_editColor);
}

void Edit_page::set_all_btn_attribute()
{
    pixmap1 = QPixmap(":/image/1x/便签颜色@2x.png");
    pixmap2 = QPixmap(":/image/1x/调整.png");
    pixmap3 = QPixmap(":/image/1x/color.png");
    ui->chang_btn->setIcon(pixmap2);
    ui->color_btn->setIcon(pixmap3);
}

void Edit_page::on_color_btn_clicked()
{
    QPointF position = this->pos();
    //QPointF position = parentWidget()->pos();
    color_page->move(position.x(),position.y()+224);
    color_page->show();
}

void Edit_page::on_chang_btn_clicked()
{
    QPointF position = this->pos();
    //QPointF position = parentWidget()->pos();
    text_edit_page->move(position.x(),position.y()+224);
    text_edit_page->show();
}

void Edit_page::contextMenuEvent(QContextMenuEvent *event)
{
    Q_UNUSED(event);
    QMenu * menu = new QMenu(this);


    QAction * delete_the_widget = new QAction(tr("关闭"));
    QAction * t1 = new QAction(tr("新建"));
    QAction * t2 = new QAction(tr("删除"));
    QAction * t3 = new QAction(tr("打开便签本"));
    connect(t1, SIGNAL(triggered()), this, SLOT(add_new_page()));
    connect(delete_the_widget, SIGNAL(triggered()), this, SLOT(clear_the_page()));
    connect(t3, SIGNAL(triggered()), this, SLOT(show_note_page()));
    menu->addAction(delete_the_widget);
    menu->addAction(t1);
    menu->addAction(t2);
    menu->addAction(t3);
    menu->move(cursor().pos()); //让菜单显示的位置在鼠标的坐标上
    menu->show();
}

void Edit_page::clear_the_page()
{
    this->close();
}

void Edit_page::add_new_page()
{
    Edit_page *new_page = new Edit_page(this->pNotebook);
    this->pNotebook->m_editors.push_back(new_page);
    new_page->show();
}

void Edit_page::show_note_page()
{
    pNotebook->show();
}

