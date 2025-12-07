#ifndef KEYFRAMESDOCK_H
#define KEYFRAMESDOCK_H

#include "models/keyframesmodel.h"
#include "qmltypes/qmlfilter.h"

#include <QDockWidget>
#include <QQuickWidget>
#include <QScopedPointer>

class QmlFilter;
class QmlMetadata;
class AttachedFiltersModel;
class QmlProducer;
class QMenu;

class KeyframesDock : public QDockWidget
{
    Q_OBJECT
    Q_PROPERTY(double timeScale READ timeScale WRITE setTimeScale NOTIFY timeScaleChanged)

public:
    explicit KeyframesDock(QmlProducer *qmlProducer, QWidget *parent = 0);

    KeyframesModel &model() { return m_model; }
    Q_INVOKABLE int seekPrevious();
    Q_INVOKABLE int seekNext();
    int currentParameter() const;
    double timeScale() const { return m_timeScale; }
    void setTimeScale(double value);

signals:
    void changed(); /// Notifies when a filter parameter changes.
    void setZoom(double value);
    void zoomIn();
    void zoomOut();
    void zoomToFit();
    void resetZoom();
    void seekPreviousSimple();
    void seekNextSimple();
    void newFilter(); // Notifies when the filter itself has been changed
    void timeScaleChanged();
    void dockClicked();

public slots:
    void setCurrentFilter(QmlFilter *filter, QmlMetadata *meta);
    void load(bool force = false);
    void reload();
    void onProducerModified();

protected:
    bool event(QEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private slots:
    void onDockRightClicked();
    void onKeyframeRightClicked();
    void onClipRightClicked();

private:
    void setupActions();
    QQuickWidget m_qview;
    KeyframesModel m_model;
    QmlMetadata *m_metadata;
    QmlFilter *m_filter;
    QmlProducer *m_qmlProducer;
    QMenu *m_mainMenu;
    QMenu *m_keyMenu;
    QMenu *m_keyTypePrevMenu;
    QMenu *m_keyTypeNextMenu;
    QMenu *m_clipMenu;
    double m_timeScale{1.0};
};

#endif // KEYFRAMESDOCK_H 