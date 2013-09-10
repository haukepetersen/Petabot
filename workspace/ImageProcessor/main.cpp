#include <QCoreApplication>
#include <imageprocessor.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ImageProcessor processor;
    processor.run();
    return a.exec();
}
