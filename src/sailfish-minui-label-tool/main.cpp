/****************************************************************************
**
** Copyright (C) 2017 Jolla Ltd.
** Contact: Andrew den Exter <andrew.den.exter@jolla.com>
**
****************************************************************************/

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QGuiApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QImage>
#include <QPainter>
#include <QTextLayout>
#include <QtMath>
#include <QTranslator>
#include <QXmlStreamReader>

#include <float.h>

#include <silicatheme.h>

static QString themeFontFamily(bool heading)
{
    return heading
            ? Silica::Theme::instance()->fontFamilyHeading()
            : Silica::Theme::instance()->fontFamily();
}

static int themeFontSize(const QByteArray &category)
{
    int fontSize = Silica::Theme::instance()->fontSizeMedium();
    if (!category.isEmpty()) {
        const QVariant value = Silica::Theme::instance()->property("fontSize" + category);
        if (value.isValid()) {
            fontSize = value.toInt();
        }
    }
    return fontSize;
}

struct Scale
{
    QString label;
    qreal ratio;
    qreal width;
};

class Generator
{
public:
    struct Language { QByteArray code; QTranslator *translator; };

    explicit Generator(
            const QFileInfo &tsFile,
            const QString &translationsDir,
            const QString &outputDir,
            int fontSize,
            const QString &fontFamily,
            Qt::Alignment alignment,
            const QVector<Scale> &scales)
        : m_font(fontFamily)
        , m_fontSize(fontSize)
        , m_scales(scales)
        , m_minimumWidth(6)
        , m_alignment(alignment)
    {
        const QString name = tsFile.baseName();
        for (const QString &fileName : QDir(translationsDir).entryList(
                { name + QLatin1String("-*.qm") }, QDir::Files)) {
            const QByteArray code = fileName.mid(name.length() + 1, fileName.length() - name.length() - 4).toUtf8();

            QTranslator * const translator = new QTranslator;
            if (!loadTranslation(translator, translationsDir, fileName, code)) {
                delete translator;
            }
        }
        {
            const QString fileName = name + QLatin1String("_eng_en.qm");
            const QByteArray code = QByteArrayLiteral("eng_en");

            QTranslator * const translator = new QTranslator;
            if (!loadTranslation(translator, tsFile.absolutePath(), fileName, code)
                    && !loadTranslation(translator, translationsDir, fileName, code)) {
                delete translator;
            }
        }

        for (const Scale &scale : m_scales) {
            const QString path = outputDir + QStringLiteral("/z%1/").arg(scale.label);

            QDir(path).mkpath(QStringLiteral("."));

            m_sizeDirs.append(path);
        }
    }

    ~Generator()
    {
        for (const Language &language : m_languages) {
            delete language.translator;
        }
    }

    bool hasTranslations()
    {
        return !m_languages.isEmpty();
    }

    void generateImages(const QString &translationId)
    {
        QByteArrayList codes;
        QString text;
        for (const Language &language : m_languages) {

            const QString translation = language.translator->translate(
                        nullptr, translationId.toUtf8().constData());
            if (!translation.isEmpty()) {
                codes.append(language.code);
                text.append(translation + QChar(QChar::LineSeparator));
            }
        }

        if (codes.isEmpty()) {
            return;
        }

        QFont font = m_font;

        QTextLayout layout;
        layout.setText(text);

        for (int s = 0; s < m_scales.count(); ++s) {
            QVector<int> lineNumbers;
            QVector<int> widths;

            font.setPixelSize(m_fontSize * m_scales.at(s).ratio);
            layout.setFont(font);
            layout.beginLayout();

            int imageHeight = 1;
            int imageWidth = 0;
            QTextLine line = layout.createLine();
            for (int i = 0, number = 0; i < codes.count(); ++i) {
                const int startLineNumber = number;

                line.setLineWidth(FLT_MAX);

                lineNumbers.append(number);

                const qreal width =  qMin(line.naturalTextWidth(), m_scales.at(s).width);
                int maximumWidth = 0;
                for (int length = line.textLength(); length > 0; ++number) {
                    line.setLineWidth(width);
                    line.setPosition(QPointF(0, imageHeight));

                    length -= line.textLength();

                    line.setLeadingIncluded(length == 0);

                    imageHeight = qCeil(imageHeight + line.height());

                    maximumWidth = qMax(maximumWidth, qCeil(line.naturalTextWidth()));

                    line = layout.createLine();
                }

                widths.append(maximumWidth);

                imageWidth = qMax(imageWidth, maximumWidth);
                imageHeight += 1;

                switch (m_alignment) {
                case Qt::AlignHCenter:
                    for (int n = startLineNumber; n < number; ++n) {
                        QTextLine line = layout.lineAt(n);
                        line.setPosition(QPointF(
                                    qFloor((maximumWidth - line.naturalTextWidth()) / 2),
                                    line.position().y()));

                    }
                    break;
                case Qt::AlignRight:
                    for (int n = startLineNumber; n < number; ++n) {
                        QTextLine line = layout.lineAt(n);
                        line.setPosition(QPointF(
                                    qCeil((maximumWidth - line.naturalTextWidth())),
                                    line.position().y()));

                    }
                    break;
                default:
                    break;
                }
            }

            layout.endLayout();

            lineNumbers.append(layout.lineCount() - 1);

            QImage image((qCeil(imageWidth) + 7) & ~3, imageHeight - 1, QImage::Format_Grayscale8);
            image.fill(Qt::black);

            {
                QPainter painter(&image);
                painter.setFont(font);
                painter.setPen(Qt::white);

                for (int i = 0; i < layout.lineCount(); ++i) {
                    QTextLine line = layout.lineAt(i);

                    line.draw(&painter, QPointF(0, 0));
                }
            }

            for (int i = 0; i < lineNumbers.count() - 1; ++i) {
                const QByteArray code = codes.at(i);
                QTextLine startLine = layout.lineAt(lineNumbers.at(i));
                QTextLine endLine = layout.lineAt(lineNumbers.at(i + 1) - 1);

                ushort width = widths.at(i);
                ushort height = endLine.position().y() + qCeil(endLine.height()) - startLine.position().y();

                uchar * const scanLine = image.scanLine(startLine.position().y() - 1);
                scanLine[0] = width;
                scanLine[1] = width >> 8;
                scanLine[2] = height;
                scanLine[3] = height >> 8;
                scanLine[4] = code.length();
                qstrcpy(reinterpret_cast<char *>(scanLine + 5), code.constData());
            }

            image.save(m_sizeDirs.at(s) + translationId + QLatin1String(".png"));
        }
    }

private:
    bool loadTranslation(
            QTranslator *translator,
            const QString &directory,
            const QString &fileName,
            const QByteArray &code)
    {
        if (translator->load(fileName, directory)) {
            m_minimumWidth = qMax(m_minimumWidth, code.length() + 6);
            m_languages.append({ code, translator });
            return true;
        } else {
            return false;
        }
    }

    QVector<Language> m_languages;
    QStringList m_sizeDirs;
    const QFont m_font;
    const int m_fontSize;
    const QVector<Scale> m_scales;
    int m_minimumWidth;
    const Qt::Alignment m_alignment;
};

int main(int argc, char *argv[])
{
    setenv("QT_QPA_PLATFORM", "offscreen", 1);

    QGuiApplication application(argc, argv);

    const qreal referenceScale = Silica::Theme::instance()->pixelRatio();
    const int paddingLarge = qCeil(Silica::Theme::instance()->paddingLarge() / referenceScale);

    QCommandLineOption headingOption(QStringLiteral("heading"));

    QCommandLineOption fontSizeOption(QStringLiteral("font-size"));
    fontSizeOption.setValueName(QStringLiteral("size"));
    fontSizeOption.setDefaultValue(QStringLiteral("Medium"));

    QCommandLineOption scaleOption(QStringLiteral("scale"));
    scaleOption.setValueName(QStringLiteral("scale"));
    scaleOption.setDefaultValues({
        QStringLiteral("1.0"),
        QStringLiteral("1.25"),
        QStringLiteral("1.5"),
        QStringLiteral("1.5-large"),
        QStringLiteral("1.75"),
        QStringLiteral("2.0")
    });

    QCommandLineOption translationOption(QStringLiteral("translation"));
    translationOption.setValueName("translation");

    QCommandLineOption alignmentOption(QStringLiteral("alignment"));
    alignmentOption.setValueName(QStringLiteral("alignment"));
    alignmentOption.setDefaultValue(QStringLiteral("Left"));

    QCommandLineOption widthOption(QStringLiteral("width"));
    widthOption.setValueName(QStringLiteral("width"));
    widthOption.setDefaultValue(QString::number(540 - (2 * paddingLarge))); // 540 is the 1.0x 16:9 portrait screen width.

    QCommandLineOption largeWidthOption(QStringLiteral("large-width"));
    largeWidthOption.setValueName(QStringLiteral("width"));
    largeWidthOption.setDefaultValue(QString::number(720 - (2 * paddingLarge))); // 720 is the 1.0x 4:3 portrait screen width.

    QCommandLineOption translationDirOption(QStringLiteral("translations-dir"));
    translationDirOption.setValueName(QStringLiteral("directory"));
    translationDirOption.setDefaultValue(QStringLiteral("/usr/share/translations"));

    QCommandLineParser parser;
    parser.addPositionalArgument(QStringLiteral("ts-file"), QString());
    parser.addPositionalArgument(QStringLiteral("output-directory"), QString());
    parser.addOption(headingOption);
    parser.addOption(fontSizeOption);
    parser.addOption(scaleOption);
    parser.addOption(translationOption);
    parser.addOption(alignmentOption);
    parser.addOption(widthOption);
    parser.addOption(largeWidthOption);
    parser.addOption(translationDirOption);
    parser.addHelpOption();

    parser.process(application);

    const QStringList arguments = parser.positionalArguments();

    if (arguments.count() != 2 || parser.unknownOptionNames().count() != 0) {
        parser.showHelp(EXIT_FAILURE);
    }

    const QString fontFamily = themeFontFamily(parser.isSet(headingOption));
    const int fontSize = themeFontSize(parser.value(fontSizeOption).toUtf8());

    Qt::Alignment alignment = Qt::AlignLeft;

    if (parser.value(alignmentOption) == QLatin1String("Center")) {
        alignment = Qt::AlignHCenter;
    } else if (parser.value(alignmentOption) == QLatin1String("Right")) {
        alignment = Qt::AlignRight;
    }

    bool okay;

    const qreal width = parser.value(widthOption).toDouble(&okay);
    if (!okay) {
        parser.showHelp(EXIT_FAILURE);
    }

    const qreal largeWidth = parser.value(largeWidthOption).toDouble(&okay);
    if (!okay) {
        parser.showHelp(EXIT_FAILURE);
    }

    QVector<Scale> scales;
    for (const QString &scaleString : parser.values(scaleOption)) {
        const int length = scaleString.indexOf(QLatin1String("-large"));
        const qreal scale = scaleString.midRef(0, length).toDouble(&okay);
        if (okay) {
            scales.append({ scaleString, scale / referenceScale, (length != -1 ? largeWidth : width) * scale });
        } else {
            parser.showHelp(EXIT_FAILURE);
        }
    }
    if (scales.isEmpty()) {
        parser.showHelp(EXIT_FAILURE);
    }

    const QString translationsDir = parser.value(translationDirOption);
    if (translationsDir.isEmpty()) {
        parser.showHelp(EXIT_FAILURE);
    }

    const QStringList translationIds = parser.values(translationOption);

    const QString tsFile = arguments.at(0);
    const QString outputDir = arguments.at(1);

    QFile file(tsFile);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Unable to open" << qPrintable(tsFile) << "to read translations";
        return EXIT_FAILURE;
    }

    Generator generator(tsFile, translationsDir, outputDir, fontSize, fontFamily, alignment, scales);

    QXmlStreamReader reader(&file);

    for (;;) {
        switch (reader.readNext()) {
        case QXmlStreamReader::Invalid:
            qWarning() << qPrintable(tsFile) << "is not a valid XML document";
            return EXIT_FAILURE;
        case QXmlStreamReader::StartElement:
            if (reader.name() == QLatin1String("TS")) {
                // descend.
            } else if (reader.name() == QLatin1String("context")) {
                // descend.
            } else if (reader.name() == QLatin1String("message")) {
                const QString id = reader.attributes().value(QLatin1String("id")).toString();
                if (!id.isEmpty() && (translationIds.isEmpty() || translationIds.contains(id))) {
                    generator.generateImages(id);
                }
                // We don't care about the body.
                reader.skipCurrentElement();
                break;
            } else {
                reader.skipCurrentElement();
            }
            break;
        case QXmlStreamReader::EndDocument:
            return EXIT_SUCCESS;
        default:
            break;
        }
    }
}
