#include "greeting.h"

#include <QCoreApplication>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#else
#include <QRegExp>
#endif
#include <QStringList>

#include <qhttpserver.h>
#include <qhttprequest.h>
#include <qhttpresponse.h>

/// Greeting

Greeting::Greeting()
{
    QHttpServer *server = new QHttpServer(this);
    connect(server, SIGNAL(newRequest(QHttpRequest*, QHttpResponse*)),
            this, SLOT(handleRequest(QHttpRequest*, QHttpResponse*)));
            
    server->listen(QHostAddress::Any, 8080);
}

void Greeting::handleRequest(QHttpRequest *req, QHttpResponse *resp)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QRegularExpression exp("^/user/([a-z]+)$");
    QRegularExpressionMatch match = exp.match(req->path());

    if (match.hasMatch())
#else
    QRegExp exp("^/user/([a-z]+)$");
    if( exp.indexIn(req->path()) != -1 )
#endif
    {
        resp->setHeader("Content-Type", "text/html");
        resp->writeHead(200);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        QString name = match.captured(1);
#else
        QString name = exp.capturedTexts()[1];
#endif
        QString body = tr("<html><head><title>Greeting App</title></head><body><h1>Hello %1!</h1></body></html>");
        resp->end(body.arg(name).toUtf8());
    }
    else
    {
        resp->writeHead(403);
        resp->end(QByteArray("You aren't allowed here!"));
    }
}

/// main

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    Greeting greeting;
    app.exec();
}
