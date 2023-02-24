#include "jkqtpexampleapplication.h"
#include <QElapsedTimer>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QWindow>
#include <QWidget>
#include "jkqtplotter/jkqtplotter.h"


JKQTPExampleApplication::JKQTPExampleApplication(int &argc, char **argv, bool _waitForScreenshotReady):
    QApplication(argc, argv),
    waitForScreenshotReady(_waitForScreenshotReady),
    readyForScreenshot(!_waitForScreenshotReady),
    saveScreenshot(false),
    saveSmallScreenshot(false),
    saveScreenshotPlot(false),
    saveSmallScreenshotPlot(false),
    scaleDownFromHighDPI(false),
    iterateFunctorSteps(false),
    iterateFunctorStepsSupressInitial(false),
    screenshotBasename("screenshot")
{
    screenshotDir=QDir::current();
    //for (int i=0; i<argc; i++) std::cout<<"  argv["<<i<<"]="<<argv[i]<<"\n";
}

JKQTPExampleApplication::~JKQTPExampleApplication()
{

}

void JKQTPExampleApplication::addExportStepFunctor(const std::function<void ()> &f)
{
    functors<<f;
}

void JKQTPExampleApplication::readCmdLine() {
    QCommandLineParser parser;
    parser.setApplicationDescription(QString("JKQTPlotter example program '%1'").arg(applicationName()));
    parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption outputDirectoryOption("screenshotdir", "write results into this directory.", "outputdir", applicationDirPath());
    parser.addOption(outputDirectoryOption);
    QCommandLineOption basenameOption("screenshotbasename", "basename for screenshots.", "basename", QFileInfo(applicationFilePath()).baseName());
    parser.addOption(basenameOption);
    QCommandLineOption screenshotOption(QStringList()<<"screenshot", "save screenshot(s) of the window(s).");
    parser.addOption(screenshotOption);
    QCommandLineOption smallscreenshotOption(QStringList()<<"smallscreenshot", "save small screenshot(s) of the window(s).");
    parser.addOption(smallscreenshotOption);
    QCommandLineOption screenshotPlotOption(QStringList()<<"screenshotplot", "save screenshot(s) of the plot(s).");
    parser.addOption(screenshotPlotOption);
    QCommandLineOption smallscreenshotPlotOption(QStringList()<<"smallscreenshotplot", "save screenshot(s) of the plot(s).");
    parser.addOption(smallscreenshotPlotOption);
    QCommandLineOption scaleDownFromHighDPIOption(QStringList()<<"scalescreenshotdownfromhighdpi", "if on high-dpi device, rescale to standard size.");
    parser.addOption(scaleDownFromHighDPIOption);
    QCommandLineOption disablehighdpiOption(QStringList()<<"disablehighdpi", "idisable high-dpi support.");
    parser.addOption(disablehighdpiOption);
    QCommandLineOption iterateFunctorStepsOption(QStringList()<<"iteratefunctorsteps", "iterate over steps defined by addExportStepFunctor() in code.");
    parser.addOption(iterateFunctorStepsOption);
    QCommandLineOption iterateFunctorStepsSupressInitialOption(QStringList()<<"iteratefunctorsteps_suppressinitial", "iterate over steps defined by addExportStepFunctor() in code.");
    parser.addOption(iterateFunctorStepsSupressInitialOption);

    parser.process(*this);

    screenshotDir=QDir(parser.value(outputDirectoryOption));
    screenshotBasename=parser.value(basenameOption).split(',');
    saveScreenshot = parser.isSet(screenshotOption);
    saveSmallScreenshot = parser.isSet(smallscreenshotOption);
    saveScreenshotPlot = parser.isSet(screenshotPlotOption);
    saveSmallScreenshotPlot = parser.isSet(smallscreenshotPlotOption);
    scaleDownFromHighDPI = parser.isSet(scaleDownFromHighDPIOption);
    iterateFunctorSteps = parser.isSet(iterateFunctorStepsOption);
    iterateFunctorStepsSupressInitial = parser.isSet(iterateFunctorStepsSupressInitialOption);
}

QRect JKQTPExampleApplication::getBoundsWithoutColor(QImage qImage, const QColor &exclusionColor)
{
    QRect ofTheKing;

    int maxX = 0; int minX = qImage.width();
    int maxY = 0; int minY = qImage.height();

    for(int x=0; x < qImage.width(); x++)
        for(int y=0; y < qImage.height(); y++)
            if (QColor::fromRgb(qImage.pixel(x, y)) != exclusionColor)
            {
                if(x < minX) minX = x;
                if(x > maxX) maxX = x;
                if(y < minY) minY = y;
                if(y > maxY) maxY = y;
            }

    if (minX > maxX || minY > maxY)
        ofTheKing=QRect();
    else
        ofTheKing.setCoords(minX, minY, maxX+1, maxY+1);

    return ofTheKing;
 }

int JKQTPExampleApplication::exec()
{
    readCmdLine();

    QList<JKQTPlotter*> plotterList;
    auto checkPlottersResizeDone=[plotterList]() {
        if (plotterList.isEmpty()) return true;
        for (JKQTPlotter* p: plotterList) {
            if (p->isResizeTimerRunning()) return false;
        }
        return true;
    };
    for (QWidget* w: allWidgets()) {
        JKQTPlotter* plot=dynamic_cast<JKQTPlotter*>(w);
        if (plot) plotterList<<plot;
    }
    QWidgetList widgets=topLevelWidgets();
    std::sort(widgets.begin(), widgets.end(), [](const QWidget* a, const QWidget* b) {
        if (a && b) return a->windowTitle().toLower()<b->windowTitle().toLower();
        if (a) return true;
        if (b) return false;
        return a<b;
    });

    auto saveWidget=[&](QWidget* w, int iVisible) {
            JKQTPlotter* plot=dynamic_cast<JKQTPlotter*>(w);
            QString bn=screenshotBasename.value(iVisible, screenshotBasename.value(0));
            if (iVisible>0 && screenshotBasename.value(iVisible, "")=="") {
                bn+=QString("_win%1").arg(iVisible, 2, 10, QLatin1Char('0'));
            }
            if (w) {
                QPixmap pix_win=w->grab();
                /*QPixmap pix;
                 if (screenshotIncludeWindowTitle) {
                    pix=w->screen()->grabWindow(0, w->frameGeometry().x(), w->frameGeometry().y(), w->frameGeometry().width(), w->frameGeometry().height());
                } else {
                    pix=pix_win;
                }*/
                if (saveScreenshot || (saveScreenshotPlot&&!plot)) {
                    if (scaleDownFromHighDPI && pix_win.devicePixelRatio()>1.0) {
                        pix_win.scaled((QSizeF(pix_win.size())/pix_win.devicePixelRatio()).toSize()).save(screenshotDir.absoluteFilePath(bn+".png"));
                    } else {
                        pix_win.save(screenshotDir.absoluteFilePath(bn+".png"));
                    }
                }
                if (saveSmallScreenshot || (saveSmallScreenshotPlot&&!plot)) {
                    QPixmap img=pix_win.scaledToWidth(150, Qt::SmoothTransformation);
                    img.save(screenshotDir.absoluteFilePath(bn+"_small.png"));
                }
            }
            if (plot) {
                QString bnp=bn+"_plot";
                QImage gr=plot->grabPixelImage();

                if (saveScreenshotPlot) {
                    QString fn=bn+".png";
                    if (saveScreenshot) fn=bnp+".png";
                    if (scaleDownFromHighDPI && gr.devicePixelRatio()>1.0) {
                        gr.scaled((QSizeF(gr.size())/gr.devicePixelRatio()).toSize()).save(screenshotDir.absoluteFilePath(fn));
                    } else {
                        gr.save(screenshotDir.absoluteFilePath(fn));
                    }
                }
                if (saveSmallScreenshotPlot) {
                    QString fn=bn+"_small.png";
                    if (saveSmallScreenshot) fn=bnp+"_small.png";
                    QImage img=gr.scaledToWidth(150, Qt::SmoothTransformation);
                    img.save(screenshotDir.absoluteFilePath(fn));
                }
            }
    };


    if (iterateFunctorSteps) {
        QVector<std::function<void(void)>> localfunctors=functors;
        if (!iterateFunctorStepsSupressInitial) localfunctors.prepend([](){});
        int iVisible=0;
        //std::cout<<localfunctors.size()<<" functors\n";
        for (auto& f: localfunctors) {
            //std::cout<<"iVisible="<<iVisible<<" f="<<std::boolalpha<<static_cast<bool>(f)<<"\n";
            if (f) f();
            //JKQTPlotter::setGlobalResizeDelay(10);
            QElapsedTimer timer;
            timer.start();
            while(timer.elapsed()<JKQTPlotter::getGlobalResizeDelay()*3/2 || !readyForScreenshot || !checkPlottersResizeDone()) {
                QApplication::processEvents();
                QApplication::processEvents();
            }
            timer.start();
            while(timer.elapsed()<50) {
                QApplication::processEvents();
                QApplication::processEvents();
            }
            for (int i=0; i<widgets.size(); i++) {
                QWidget* w=widgets[i];
                if (w->isVisible()) {
                    saveWidget(w, iVisible);
                    iVisible++;
                }
            }
        }
        return 0;
    } else if (saveScreenshot||saveSmallScreenshot||saveScreenshotPlot||saveSmallScreenshotPlot) {

        //std::cout<<"non-functor-mode\n";
        //JKQTPlotter::setGlobalResizeDelay(10);
        QElapsedTimer timer;
        timer.start();
        while(timer.elapsed()<JKQTPlotter::getGlobalResizeDelay()*3/2 || !readyForScreenshot || !checkPlottersResizeDone()) {
            QApplication::processEvents();
            QApplication::processEvents();
        }
        timer.start();
        while(timer.elapsed()<50) {
            QApplication::processEvents();
            QApplication::processEvents();
        }
        int iVisible=0;
        for (int i=0; i<widgets.size(); i++) {
            QWidget* w=widgets[i];
            if (w->isVisible()) {
                saveWidget(w, iVisible);
                iVisible++;
            }

        }
        return 0;
    } else {
        return QApplication::exec();
    }
}

void JKQTPExampleApplication::notifyReadyForScreenshot()
{
    readyForScreenshot=true;
}
