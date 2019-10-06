#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "preview.h"
#include <QDir>
#include <QFileDialog>
#include <QVector>
#include <QList>
#include <QFile>
#include <QFontDatabase>
#include <QMessageBox>
#include <QWebChannel>
#include <QDialog>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->editor->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    ui->preview->setContextMenuPolicy(Qt::NoContextMenu);

    ui->splitter->setStretchFactor(0,2);
    ui->splitter->setStretchFactor(1,7);
    ui->splitter->setStretchFactor(2,1);

    Preview *page = new Preview(this);
    ui->preview->setPage(page);


    QWebChannel *channel = new QWebChannel(this);
    channel->registerObject(QStringLiteral("content"), &m_content);
    page->setWebChannel(channel);
    ui->preview->setUrl(QUrl("qrc:/index.html"));
    
    QFile defaultTextFile(":/default.md");
    defaultTextFile.open(QIODevice::ReadOnly);
    ui->editor->setPlainText(defaultTextFile.readAll());

    initSlots();
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::isModified() const
{
    return ui->editor->document()->isModified();
}


void MainWindow::initSlots(){
    connect(ui->editor, &QPlainTextEdit::textChanged,[this]() { m_content.setText(ui->editor->toPlainText()); });
}

void MainWindow::on_actNew_triggered()
{
    if (isModified()) {
        QMessageBox::StandardButton button = QMessageBox::question(this, windowTitle(),
                                                                   tr("文档发生了修改,需要保存么?"),QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes);
        if (button == QMessageBox::Yes) saveFile();
        else if (button == QMessageBox::Cancel) return;
    }
    m_filePath.clear();
    ui->editor->setPlainText(tr("## New file"));
    ui->editor->document()->setModified(false);
}

void MainWindow::on_actSaveAs_triggered()
{
    saveFileAs();
    addFileName();
}

void MainWindow::on_actOpen_triggered()
{
    if (isModified()) {
        QMessageBox::StandardButton button = QMessageBox::question(this, windowTitle(),
                                                                   tr("文档发生了修改,需要保存么?"),QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes);
        if (button == QMessageBox::Yes) saveFile();
        else if (button == QMessageBox::Cancel) return;
    }
    openFile();
    addFileName();
}

void MainWindow::on_actSave_triggered()
{
    saveFile();
}

void MainWindow::openFile()
{
    m_filePath = QDir::currentPath();
//    qDebug()<<"打开文件"<<m_filePath;
    QString title = "打开一个文件";
    QString filter = "(*.md);;(*.markdown)";
    QString fileName = QFileDialog::getOpenFileName(this,title,"/",filter);
    if(fileName.isEmpty()) return;
    openTextByIODevice(fileName);
}

void MainWindow::openTextByIODevice(const QString &fileName)
{
    QFile m_fileName(fileName);
    if(!m_fileName.exists()) return;
    if (!m_fileName.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, windowTitle(),
                             tr("无法打开文件 %1: %2").arg(
                                 QDir::toNativeSeparators(fileName), m_fileName.errorString()));
        QMessageBox::warning(this,windowTitle(),tr("无法打开%1: %2").arg(
                                 QDir::toNativeSeparators(fileName),m_fileName.errorString()));
        return;
    }
    ui->editor->setPlainText(m_fileName.readAll());
    m_filePath = fileName;
    m_fileName.close();
}


void MainWindow::saveFileAs(){
    QString path = QFileDialog::getSaveFileName(this,
                                                tr("markdown文档保存"), "..", tr("(*.md *.markdown)"));
    if (path.isEmpty()) return;
    m_filePath = path;
    saveFile();
}

void MainWindow::saveFile()
{
    //    qDebug()<<m_filePath;
    if (m_filePath.isEmpty()) {
        saveFileAs();
        return;
    }

    QFile file(m_filePath);
    //        qDebug()<<file;
    //        qDebug()<<m_filePath;
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))  {
        QMessageBox::warning(this, windowTitle(),tr("无法打开文件 %1: %2").arg(
                                 QDir::toNativeSeparators(m_filePath), file.errorString()));
        return;
    }

    QString str =ui->editor->toPlainText();
    QByteArray strBytes = str.toUtf8();
    file.write(strBytes,strBytes.length());
    file.close();
    ui->editor->document()->setModified(false);
}

void MainWindow::addFileName()
{
    QListWidgetItem *it = new QListWidgetItem;
    it->setText(m_filePath);
   for (int i = 0;i<ui->list->count();i++)
   {
       if(it->text()==ui->list->item(i)->text()) return;
   }
    ui->list->addItem(it);
}


void MainWindow::on_list_itemClicked(QListWidgetItem *item)
{
    openTextByIODevice(item->text());
}

