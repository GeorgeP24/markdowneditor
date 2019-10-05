#ifndef PREVIEW_H
#define PREVIEW_H

#include <QWebEnginePage>

class Preview : public QWebEnginePage
{
    Q_OBJECT
public:
    explicit Preview(QObject *parent = nullptr) : QWebEnginePage(parent) {}

protected:
    bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame);
};

#endif // PREVIEW_H
