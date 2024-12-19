#pragma once

#include <QTimer>
#include <dots/Event.h>
#include <dots/Subscription.h>
#include <QtQuick/QQuickItem>
#include <dots/tools/logging.h>

class DotsSubscription: public QQuickItem
{
  Q_OBJECT
  Q_PROPERTY(QString name READ name WRITE setName FINAL)
  QML_ELEMENT

  public:
    DotsSubscription(QQuickItem *parent = nullptr);

    QString name() const { return m_name; }
    void setName(const QString &name) {
      subscribe(name.toStdString());
      m_name = name;
    }

    void createType();
    void subscribe(const std::string& typeName);

  signals:
    void eventReceived(const QObject* obj);

   private:
     void handleDotsEvent(const dots::Event<>& e);
     std::optional<dots::Subscription> m_subscription;


     QString m_name;

     QTimer m_timer;
};

