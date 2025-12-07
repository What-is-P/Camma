#include "keyframesdock.h"

#include "Logger.h"
#include "actions.h"
#include "mainwindow.h"
#include "qmltypes/qmlproducer.h"
#include "qmltypes/qmlutilities.h"
#include "qmltypes/qmlview.h"
#include "settings.h"
#include "widgets/docktoolbar.h"

#include <QAction>
#include <QActionGroup>
#include <QDir>
#include <QIcon>
#include <QMenu>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QSlider>
#include <QToolButton>
#include <QUrl>
#include <QVBoxLayout>

#include <cmath>

static QmlMetadata m_emptyQmlMetadata;
static QmlFilter m_emptyQmlFilter;

KeyframesDock::KeyframesDock(QmlProducer *qmlProducer, QWidget *parent)
    : QDockWidget(tr("Keyframes"), parent)
    , m_qview(QmlUtilities::sharedEngine(), this)
    , m_qmlProducer(qmlProducer)
{
    LOG_DEBUG() << "begin";
    setObjectName("KeyframesDock");
    QIcon icon = QIcon::fromTheme("chronometer",
                                  QIcon(":/icons/oxygen/32x32/actions/chronometer.png"));
    setWindowIcon(icon);
    toggleViewAction()->setIcon(windowIcon());
    setMinimumSize(200, 50);

    setupActions();

    m_mainMenu = new QMenu(tr("Keyframes"), this);
    m_mainMenu->addAction(Actions["keyframesTrimInAction"]);
    m_mainMenu->addAction(Actions["keyframesTrimOutAction"]);
    m_mainMenu->addAction(Actions["keyframesAnimateInAction"]);
    m_mainMenu->addAction(Actions["keyframesAnimateOutAction"]);
    m_mainMenu->addAction(Actions["keyframesScrubDragAction"]);
    m_mainMenu->addAction(Actions["keyframesToggleKeyframeAction"]);
    m_mainMenu->addAction(Actions["keyframesSeekPreviousAction"]);
    m_mainMenu->addAction(Actions["keyframesSeekNextAction"]);
    QMenu *viewMenu = new QMenu(tr("View"), this);
    viewMenu->addAction(Actions["keyframesZoomOutAction"]);
    viewMenu->addAction(Actions["keyframesZoomInAction"]);
    viewMenu->addAction(Actions["keyframesZoomFitAction"]);
    m_mainMenu->addMenu(viewMenu);
    Actions.loadFromMenu(m_mainMenu);

    m_keyMenu = new QMenu(tr("Keyframe"), this);
    m_keyTypePrevMenu = new QMenu(tr("From Previous"), this);
    m_keyTypePrevMenu->addAction(Actions["keyframesTypePrevHoldAction"]);
    m_keyTypePrevMenu->addAction(Actions["keyframesTypePrevLinearAction"]);
    m_keyTypePrevMenu->addAction(Actions["keyframesTypePrevSmoothNaturalAction"]);

    QMenu *keyEaseOutMenu = new QMenu(tr("Ease Out"), this);
    icon = QIcon::fromTheme("keyframe-ease-out",
                            QIcon(":/icons/oxygen/32x32/actions/keyframe-ease-out.png"));
    keyEaseOutMenu->setIcon(icon);
    keyEaseOutMenu->addAction(Actions["keyframesTypePrevEaseOutSinuAction"]);
    keyEaseOutMenu->addAction(Actions["keyframesTypePrevEaseOutQuadAction"]);
    keyEaseOutMenu->addAction(Actions["keyframesTypePrevEaseOutCubeAction"]);
    keyEaseOutMenu->addAction(Actions["keyframesTypePrevEaseOutQuartAction"]);
    keyEaseOutMenu->addAction(Actions["keyframesTypePrevEaseOutQuintAction"]);
    keyEaseOutMenu->addAction(Actions["keyframesTypePrevEaseOutExpoAction"]);
    keyEaseOutMenu->addAction(Actions["keyframesTypePrevEaseOutCircAction"]);
    keyEaseOutMenu->addAction(Actions["keyframesTypePrevEaseOutBackAction"]);
    keyEaseOutMenu->addAction(Actions["keyframesTypePrevEaseOutElasAction"]);
    keyEaseOutMenu->addAction(Actions["keyframesTypePrevEaseOutBounAction"]);
    m_keyTypePrevMenu->addMenu(keyEaseOutMenu);
    m_keyMenu->addMenu(m_keyTypePrevMenu);
    m_keyTypeNextMenu = new QMenu(tr("To Next"), this);
    m_keyTypeNextMenu->addAction(Actions["keyframesTypeHoldAction"]);
    m_keyTypeNextMenu->addAction(Actions["keyframesTypeLinearAction"]);
    m_keyTypeNextMenu->addAction(Actions["keyframesTypeSmoothNaturalAction"]);

    QMenu *keyEaseInMenu = new QMenu(tr("Ease In"), this);
    icon = QIcon::fromTheme("keyframe-ease-in",
                            QIcon(":/icons/oxygen/32x32/actions/keyframe-ease-in.png"));
    keyEaseInMenu->setIcon(icon);
    keyEaseInMenu->addAction(Actions["keyframesTypeEaseInSinuAction"]);
    keyEaseInMenu->addAction(Actions["keyframesTypeEaseInQuadAction"]);
    keyEaseInMenu->addAction(Actions["keyframesTypeEaseInCubeAction"]);
    keyEaseInMenu->addAction(Actions["keyframesTypeEaseInQuartAction"]);
    keyEaseInMenu->addAction(Actions["keyframesTypeEaseInQuintAction"]);
    keyEaseInMenu->addAction(Actions["keyframesTypeEaseInExpoAction"]);
    keyEaseInMenu->addAction(Actions["keyframesTypeEaseInCircAction"]);
    keyEaseInMenu->addAction(Actions["keyframesTypeEaseInBackAction"]);
    keyEaseInMenu->addAction(Actions["keyframesTypeEaseInElasAction"]);
    keyEaseInMenu->addAction(Actions["keyframesTypeEaseInBounAction"]);
    m_keyTypeNextMenu->addMenu(keyEaseInMenu);

    QMenu *keyEaseInOutMenu = new QMenu(tr("Ease In/Out"), this);
    icon = QIcon::fromTheme("keyframe-ease-inout",
                            QIcon(":/icons/oxygen/32x32/actions/keyframe-ease-inout.png"));
    keyEaseInOutMenu->setIcon(icon);
    keyEaseInOutMenu->addAction(Actions["keyframesTypeEaseInOutSinuAction"]);
    keyEaseInOutMenu->addAction(Actions["keyframesTypeEaseInOutQuadAction"]);
    keyEaseInOutMenu->addAction(Actions["keyframesTypeEaseInOutCubeAction"]);
    keyEaseInOutMenu->addAction(Actions["keyframesTypeEaseInOutQuartAction"]);
    keyEaseInOutMenu->addAction(Actions["keyframesTypeEaseInOutQuintAction"]);
    keyEaseInOutMenu->addAction(Actions["keyframesTypeEaseInOutExpoAction"]);
    keyEaseInOutMenu->addAction(Actions["keyframesTypeEaseInOutCircAction"]);
    keyEaseInOutMenu->addAction(Actions["keyframesTypeEaseInOutBackAction"]);
    keyEaseInOutMenu->addAction(Actions["keyframesTypeEaseInOutElasAction"]);
    keyEaseInOutMenu->addAction(Actions["keyframesTypeEaseInOutBounAction"]);
    m_keyTypeNextMenu->addMenu(keyEaseInOutMenu);
    m_keyMenu->addMenu(m_keyTypeNextMenu);
    m_keyMenu->addAction(Actions["keyframesRemoveAction"]);
    Actions.loadFromMenu(m_keyMenu);

    m_clipMenu = new QMenu(tr("Keyframes Clip"), this);
    m_clipMenu->addAction(Actions["keyframesRebuildAudioWaveformAction"]);
    Actions.loadFromMenu(m_clipMenu);

    QVBoxLayout *vboxLayout = new QVBoxLayout();
    vboxLayout->setSpacing(0);
    vboxLayout->setContentsMargins(0, 0, 0, 0);

    DockToolBar *toolbar = new DockToolBar(tr("Keyframes Controls"));
    QToolButton *menuButton = new QToolButton();
    menuButton->setIcon(
        QIcon::fromTheme("show-menu", QIcon(":/icons/oxygen/32x32/actions/show-menu.png")));
    menuButton->setToolTip(tr("Keyframes Menu"));
    menuButton->setAutoRaise(true);
    menuButton->setPopupMode(QToolButton::QToolButton::InstantPopup);
    menuButton->setMenu(m_mainMenu);
    toolbar->addWidget(menuButton);
    toolbar->addSeparator();
    toolbar->addAction(Actions["keyframesTrimInAction"]);
    toolbar->addAction(Actions["keyframesTrimOutAction"]);
    toolbar->addAction(Actions["keyframesAnimateInAction"]);
    toolbar->addAction(Actions["keyframesAnimateOutAction"]);
    toolbar->addSeparator();
    toolbar->addAction(Actions["timelineSnapAction"]);
    toolbar->addAction(Actions["keyframesScrubDragAction"]);
    toolbar->addSeparator();
    toolbar->addAction(Actions["keyframesZoomOutAction"]);
    QSlider *zoomSlider = new QSlider();
    zoomSlider->setOrientation(Qt::Horizontal);
    zoomSlider->setMaximumWidth(200);
    zoomSlider->setMinimum(0);
    zoomSlider->setMaximum(300);
    zoomSlider->setValue(100);
    zoomSlider->setTracking(false);
    connect(zoomSlider, &QSlider::valueChanged, this, [&](int value) {
        if (!isVisible() || !m_qview.rootObject())
            return;
        emit setZoom(value / 100.0);
    });
    connect(this, &KeyframesDock::timeScaleChanged, zoomSlider, [=]() {
        double value = round(pow(m_timeScale - 0.01, 1.0 / 3.0) * 100.0);
        zoomSlider->setValue(value);
    });
    toolbar->addWidget(zoomSlider);
    toolbar->addAction(Actions["keyframesZoomInAction"]);
    toolbar->addAction(Actions["keyframesZoomFitAction"]);

    vboxLayout->setMenuBar(toolbar);

    m_qview.setFocusPolicy(Qt::StrongFocus);
    m_qview.quickWindow()->setPersistentSceneGraph(false);
#ifndef Q_OS_MAC
    m_qview.setAttribute(Qt::WA_AcceptTouchEvents);
#endif
    setWidget(&m_qview);

    QmlUtilities::setCommonProperties(m_qview.rootContext());
    m_qview.rootContext()->setContextProperty("keyframes", this);
    m_qview.rootContext()->setContextProperty("view", new QmlView(&m_qview));
    m_qview.rootContext()->setContextProperty("parameters", &m_model);
    m_qview.setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_qview.setClearColor(palette().window().color());
    m_qview.quickWindow()->setPersistentSceneGraph(false);
#ifndef Q_OS_MAC
    m_qview.setAttribute(Qt::WA_AcceptTouchEvents);
#endif
    setCurrentFilter(0, 0);
    connect(this, SIGNAL(visibilityChanged(bool)), this, SLOT(load(bool)));

    vboxLayout->addWidget(&m_qview);
    QWidget *dockContentsWidget = new QWidget();
    dockContentsWidget->setLayout(vboxLayout);
    QDockWidget::setWidget(dockContentsWidget);

    LOG_DEBUG() << "end";
}

void KeyframesDock::setupActions()
{
    QIcon icon;
    QAction *action;

    action = new QAction(tr("Set Filter Start"), this);
    action->setShortcut(QKeySequence(Qt::Key_BracketLeft));
    icon = QIcon::fromTheme("keyframes-filter-in",
                            QIcon(":/icons/oxygen/32x32/actions/keyframes-filter-in.png"));
    action->setIcon(icon);
    connect(action, &QAction::triggered, this, [&]() {
        if (m_qmlProducer && m_filter && m_filter->allowTrim()) {
            int i = m_qmlProducer->position() + m_qmlProducer->in();
            m_model.trimFilterIn(i);
        }
    });
    connect(this, &KeyframesDock::newFilter, action, [=]() {
        bool enabled = false;
        if (m_filter && m_filter->allowTrim())
            enabled = true;
        action->setEnabled(enabled);
    });
    Actions.add("keyframesTrimInAction", action);

    action = new QAction(tr("Set Filter End"), this);
    action->setShortcut(QKeySequence(Qt::Key_BracketRight));
    icon = QIcon::fromTheme("keyframes-filter-out",
                            QIcon(":/icons/oxygen/32x32/actions/keyframes-filter-out.png"));
    action->setIcon(icon);
    connect(action, &QAction::triggered, this, [&]() {
        if (m_qmlProducer && m_filter && m_filter->allowTrim()) {
            int i = m_qmlProducer->position() + m_qmlProducer->in();
            m_model.trimFilterOut(i);
        }
    });
    connect(this, &KeyframesDock::newFilter, action, [=]() {
        bool enabled = false;
        if (m_filter && m_filter->allowTrim())
            enabled = true;
        action->setEnabled(enabled);
    });
    Actions.add("keyframesTrimOutAction", action);

    action = new QAction(tr("Set First Simple Keyframe"), this);
    action->setShortcut(QKeySequence(Qt::Key_BraceLeft));
    icon = QIcon::fromTheme("keyframes-simple-in",
                            QIcon(":/icons/oxygen/32x32/actions/keyframes-simple-in.png"));
    action->setIcon(icon);
    connect(action, &QAction::triggered, this, [&]() {
        if (m_qmlProducer && m_filter && m_filter->allowAnimateIn()) {
            int i = m_qmlProducer->position() + m_qmlProducer->in() - m_filter->in();
            m_filter->setAnimateIn(i);
        }
    });
    connect(this, &KeyframesDock::newFilter, action, [=]() {
        bool enabled = false;
        if (m_filter && m_filter->allowAnimateIn())
            enabled = true;
        action->setEnabled(enabled);
    });
    Actions.add("keyframesAnimateInAction", action);

    action = new QAction(tr("Set Second Simple Keyframe"), this);
    action->setShortcut(QKeySequence(Qt::Key_BraceRight));
    icon = QIcon::fromTheme("keyframes-simple-out",
                            QIcon(":/icons/oxygen/32x32/actions/keyframes-simple-out.png"));
    action->setIcon(icon);
    connect(action, &QAction::triggered, this, [&]() {
        if (m_qmlProducer && m_filter && m_filter->allowAnimateOut()) {
            int i = m_filter->out() - (m_qmlProducer->position() + m_qmlProducer->in());
            m_filter->setAnimateOut(i);
        }
    });
    connect(this, &KeyframesDock::newFilter, action, [=]() {
        bool enabled = false;
        if (m_filter && m_filter->allowAnimateOut())
            enabled = true;
        action->setEnabled(enabled);
    });
    Actions.add("keyframesAnimateOutAction", action);

    action = new QAction(tr("Scrub While Dragging"), this);
    icon = QIcon::fromTheme("scrub_drag", QIcon(":/icons/oxygen/32x32/actions/scrub_drag.png"));
    action->setIcon(icon);
    action->setCheckable(true);
    action->setChecked(Settings.keyframesDragScrub());
    connect(action, &QAction::triggered, this, [&](bool checked) {
        Settings.setKeyframesDragScrub(checked);
    });
    connect(&Settings, &ShotcutSettings::keyframesDragScrubChanged, action, [=]() {
        action->setChecked(Settings.keyframesDragScrub());
    });
    Actions.add("keyframesScrubDragAction", action);

    action = new QAction(tr("Zoom Keyframes Out"), this);
    action->setShortcut(QKeySequence(Qt::ALT | Qt::Key_Minus));
    icon = QIcon::fromTheme("zoom-out", QIcon(":/icons/oxygen/32x32/actions/zoom-out.png"));
    action->setIcon(icon);
    connect(action, &QAction::triggered, this, [&]() {
        if (!isVisible() || !m_qview.rootObject())
            return;
        emit zoomOut();
    });
    Actions.add("keyframesZoomOutAction", action);

    action = new QAction(tr("Zoom Keyframes In"), this);
    action->setShortcut(QKeySequence(Qt::ALT | Qt::Key_Plus));
    icon = QIcon::fromTheme("zoom-in", QIcon(":/icons/oxygen/32x32/actions/zoom-in.png"));
    action->setIcon(icon);
    connect(action, &QAction::triggered, this, [&]() {
        if (!isVisible() || !m_qview.rootObject())
            return;
        emit zoomIn();
    });
    Actions.add("keyframesZoomInAction", action);

    action = new QAction(tr("Zoom Keyframes To Fit"), this);
    action->setShortcut(QKeySequence(Qt::ALT | Qt::Key_0));
    icon = QIcon::fromTheme("zoom-fit-best",
                            QIcon(":/icons/oxygen/32x32/actions/zoom-fit-best.png"));
    action->setIcon(icon);
    connect(action, &QAction::triggered, this, [&]() {
        if (!isVisible() || !m_qview.rootObject())
            return;
        emit zoomToFit();
    });
    Actions.add("keyframesZoomFitAction", action);

    // Actions to modify previous keyframes
    QActionGroup *keyframeTypePrevActionGroup = new QActionGroup(this);
    keyframeTypePrevActionGroup->setExclusive(true);

    action = new QAction(tr("Hold"), this);
    connect(action, &QAction::triggered, this, [&]() {
        if (!isVisible() || !m_qview.rootObject())
            return;
        int currentTrack = m_qview.rootObject()->property("currentTrack").toInt();
        for (auto keyframeIndex : m_qview.rootObject()->property("selection").toList()) {
            m_model.setInterpolation(currentTrack,
                                     keyframeIndex.toInt() - 1,
                                     KeyframesModel::DiscreteInterpolation);
        }
    });
    icon = QIcon::fromTheme("keyframe-hold",
                            QIcon(":/icons/oxygen/32x32/actions/keyframe-hold.png"));
    action->setIcon(icon);
    keyframeTypePrevActionGroup->addAction(action);
    Actions.add("keyframesTypePrevHoldAction", action);

    action = new QAction(tr("Linear"), this);
    connect(action, &QAction::triggered, this, [&]() {
        if (!isVisible() || !m_qview.rootObject())
            return;
        int currentTrack = m_qview.rootObject()->property("currentTrack").toInt();
        for (auto keyframeIndex : m_qview.rootObject()->property("selection").toList()) {
            m_model.setInterpolation(currentTrack,
                                     keyframeIndex.toInt() - 1,
                                     KeyframesModel::LinearInterpolation);
        }
    });
    icon = QIcon::fromTheme("keyframe-linear",
                            QIcon(":/icons/oxygen/32x32/actions/keyframe-linear.png"));
    action->setIcon(icon);
    keyframeTypePrevActionGroup->addAction(action);
    Actions.add("keyframesTypePrevLinearAction", action);

    action = new QAction(tr("Smooth"), this);
    connect(action, &QAction::triggered, this, [&]() {
        if (!isVisible() || !m_qview.rootObject())
            return;
        int currentTrack = m_qview.rootObject()->property("currentTrack").toInt();
        for (auto keyframeIndex : m_qview.rootObject()->property("selection").toList()) {
            m_model.setInterpolation(currentTrack,
                                     keyframeIndex.toInt() - 1,
                                     KeyframesModel::SmoothNaturalInterpolation);
        }
    });
    icon = QIcon::fromTheme("keyframe-smooth",
                            QIcon(":/icons/oxygen/32x32/actions/keyframe-smooth.png"));
    action->setIcon(icon);
    keyframeTypePrevActionGroup->addAction(action);
    Actions.add("keyframesTypePrevSmoothNaturalAction", action);

    action = new QAction(tr("Ease Out Sinusoidal"), this);
    connect(action, &QAction::triggered, this, [&]() {
        if (!isVisible() || !m_qview.rootObject())
            return;
        int currentTrack = m_qview.rootObject()->property("currentTrack").toInt();
        for (auto keyframeIndex : m_qview.rootObject()->property("selection").toList()) {
            m_model.setInterpolation(currentTrack,
                                     keyframeIndex.toInt() - 1,
                                     KeyframesModel::EaseOutSinusoidal);
        }
    });
    icon = QIcon::fromTheme("ease-out-sinu",
                            QIcon(":/icons/oxygen/32x32/actions/ease-out-sinu.png"));
    action->setIcon(icon);
    keyframeTypePrevActionGroup->addAction(action);
    Actions.add("keyframesTypePrevEaseOutSinuAction", action);

    action = new QAction(tr("Ease Out Quadratic"), this);
    connect(action, &QAction::triggered, this, [&]() {
        if (!isVisible() || !m_qview.rootObject())
            return;
        int currentTrack = m_qview.rootObject()->property("currentTrack").toInt();
        for (auto keyframeIndex : m_qview.rootObject()->property("selection").toList()) {
            m_model.setInterpolation(currentTrack,
                                     keyframeIndex.toInt() - 1,
                                     KeyframesModel::EaseOutQuadratic);
        }
    });
    icon = QIcon::fromTheme("ease-out-quad",
                            QIcon(":/icons/oxygen/32x32/actions/ease-out-quad.png"));
    action->setIcon(icon);
    keyframeTypePrevActionGroup->addAction(action);
    Actions.add("keyframesTypePrevEaseOutQuadAction", action);

    action = new QAction(tr("Ease Out Cubic"), this);
    connect(action, &QAction::triggered, this, [&]() {
        if (!isVisible() || !m_qview.rootObject())
            return;
        int currentTrack = m_qview.rootObject()->property("currentTrack").toInt();
        for (auto keyframeIndex : m_qview.rootObject()->property("selection").toList()) {
            m_model.setInterpolation(currentTrack,
                                     keyframeIndex.toInt() - 1,
                                     KeyframesModel::EaseOutCubic);
        }
    });
    icon = QIcon::fromTheme("ease-out-cube",
                            QIcon(":/icons/oxygen/32x32/actions/ease-out-cube.png"));
    action->setIcon(icon);
    keyframeTypePrevActionGroup->addAction(action);
    Actions.add("keyframesTypePrevEaseOutCubeAction", action);

    action = new QAction(tr("Ease Out Quartic"), this);
    connect(action, &QAction::triggered, this, [&]() {
        if (!isVisible() || !m_qview.rootObject())
            return;
        int currentTrack = m_qview.rootObject()->property("currentTrack").toInt();
        for (auto keyframeIndex : m_qview.rootObject()->property("selection").toList()) {
            m_model.setInterpolation(currentTrack,
                                