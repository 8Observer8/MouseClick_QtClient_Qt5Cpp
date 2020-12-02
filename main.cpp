#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtGui/QMouseEvent>
#include <QtWebSockets/QWebSocket>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QDebug>

class Window : public QWidget {
    Q_OBJECT
private:
    QWebSocket m_webSocket;
    QLabel m_connection;
    QLabel m_output;
public:
    Window(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowTitle("Qt C++ Client");
        resize(300, 300);
        QVBoxLayout *vbox = new QVBoxLayout(this);
        QLabel *instruction = new QLabel("Click on the window.");
        m_connection.setText("Wait for connection...");
        QFont font = QFont("Arial", 14);
        instruction->setFont(font);
        m_connection.setFont(font);
        m_output.setFont(font);
        vbox->addWidget(instruction);
        vbox->addWidget(&m_connection);
        vbox->addWidget(&m_output);
        vbox->addStretch(1);
        connect(&m_webSocket, &QWebSocket::connected,
                 this, &Window::onConnected);
        // m_webSocket.open(QUrl("ws://localhost:3000"));
        m_webSocket.open(QUrl("wss://mouse-click-js.herokuapp.com"));
    }
private slots:
    void onConnected() {
        m_connection.setText("Connected to server");
        connect(&m_webSocket, &QWebSocket::textMessageReceived,
                this, &Window::onMessageReceived);
    }
    void onMessageReceived(QString message) {
        qDebug() << message;
        QJsonDocument doc(QJsonDocument::fromJson(message.toUtf8()));
        QJsonObject data = doc.object();
        int x = data["x"].toInt();
        int y = data["y"].toInt();
        // Show data
        m_output.setText(QString("Answer from server: %1, %2").arg(x).arg(y));
    }
private:
    void mousePressEvent(QMouseEvent *event) override {
        Q_UNUSED(event);
        qDebug() << event->x() << " " << event->y();
        QJsonObject jsonObject;
        jsonObject["x"] = event->x();
        jsonObject["y"] = event->y();
        QString message = QJsonDocument(jsonObject).toJson(QJsonDocument::Compact);
        m_webSocket.sendTextMessage(message);
    }
};

#include "main.moc"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Window w;
    w.show();
    return a.exec();
}
