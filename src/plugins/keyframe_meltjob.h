#ifndef MELTJOB_H
#define MELTJOB_H

#include "abstractjob.h"

#include <MltProfile.h>
#include <QTemporaryFile>

class MeltJob : public AbstractJob
{
    Q_OBJECT
public:
    MeltJob(const QString &name,
            const QString &xml,
            int frameRateNum,
            int frameRateDen,
            QThread::Priority priority = Settings.jobPriority());
    MeltJob(const QString &name, const QStringList &args, int frameRateNum, int frameRateDen);
    MeltJob(const QString &name,
            const QString &xml,
            const QStringList &args,
            int frameRateNum,
            int frameRateDen);
    virtual ~MeltJob();
    QString xml();
    QString xmlPath() const { return m_xml->fileName(); }
    void setIsStreaming(bool streaming);
    void setUseMultiConsumer(bool multi = true);
    void setInAndOut(int in, int out);

public slots:
    void start() override;
    void onViewXmlTriggered();

protected slots:
    virtual void onOpenTiggered();
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus) override;
    void onShowFolderTriggered();
    void onShowInFilesTriggered();
    void onReadyRead() override;

protected:
    QScopedPointer<QTemporaryFile> m_xml;

private:
    bool m_isStreaming;
    int m_previousPercent;
    QStringList m_args;
    int m_currentFrame;
    Mlt::Profile m_profile;
    bool m_useMultiConsumer;
    int m_in{-1};
    int m_out{-1};
};

#endif // MELTJOB_H

protected:
      /Camma/src/plugins/keyframe.h
protected:
      /Camma/src/plugins/keyframe2.cpp