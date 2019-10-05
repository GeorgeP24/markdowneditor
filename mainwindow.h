#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "document.h"
#include <QListWidgetItem>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void initSlots();

private slots:
    void on_actOpen_triggered();
    void on_actSave_triggered();
    void on_actSaveAs_triggered();
    void on_actNew_triggered();
    void on_list_itemClicked(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;
    QString m_filePath;
    Document m_content;

    bool isModified() const;
    void openFile();
    void openTextByIODevice(const QString &fileName);
    void saveFileAs();
    void saveFile();
    void addFileName();
};




#endif // MAINWINDOW_H
