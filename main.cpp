#include "trading.h"

using namespace std;

int main(int argc, char *argv[]) {
    QApplication app(argc,argv);

    // Bout de code permettant d'avoir nos boutons usuels dans la langue locale du PC lors de l'exécution
    QString locale = QLocale::system().name().section('_', 0, 0);
    QTranslator translator;
    translator.load(QString("qt_") + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&translator);



    InterfaceFinale itf;
    itf.show();

    return app.exec();
}
