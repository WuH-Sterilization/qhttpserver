#include "bodydata.h"

#include <QCoreApplication>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#else
#include <QRegExp>
#endif
#include <QStringList>
#include <QDebug>

#include <qhttpserver.h>
#include <qhttprequest.h>
#include <qhttpresponse.h>

/// BodyData

BodyData::BodyData()
{
    QHttpServer *server = new QHttpServer(this);
    connect(server, SIGNAL(newRequest(QHttpRequest*, QHttpResponse*)),
        this, SLOT(handleRequest(QHttpRequest*, QHttpResponse*)));
        
    server->listen(QHostAddress::Any, 8080);
}

void BodyData::handleRequest(QHttpRequest *req, QHttpResponse *resp)
{
    new Responder(req, resp);
}

/// Responder

Responder::Responder(QHttpRequest *req, QHttpResponse *resp)
    : m_req(req)
    , m_resp(resp)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QRegularExpression exp("^/user/([a-z]+)$");
    QRegularExpressionMatch match = exp.match(req->path());

    if (!match.hasMatch())
#else
    QRegExp exp("^/user/([a-z]+$)");
    if (exp.indexIn(req->path()) == -1)
#endif
    {
        resp->writeHead(403);
        resp->end(QByteArray("You aren't allowed here!"));
        /// @todo There should be a way to tell request to stop streaming data
        return;
    }

    resp->setHeader("Content-Type", "text/html");
    resp->writeHead(200);
    
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QString name = match.captured(1);
#else
    QString name = exp.capturedTexts()[1];
#endif
    QString bodyStart = tr("<html><head><title>BodyData App</title></head><body><h1>Hello %1!</h1><p>").arg(name);
    resp->write(bodyStart.toUtf8());

    connect(req, SIGNAL(data(const QByteArray&)), this, SLOT(accumulate(const QByteArray&)));
    connect(req, SIGNAL(end()), this, SLOT(reply()));
    connect(m_resp, SIGNAL(done()), this, SLOT(deleteLater()));
}

Responder::~Responder()
{
}

void Responder::accumulate(const QByteArray &data)
{
    m_resp->write(data);
}

void Responder::reply()
{
    m_resp->end(QByteArray("</p></body></html>"));
}

/// main

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    BodyData bodydata;
    app.exec();
}
